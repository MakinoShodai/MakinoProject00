#include "TextureResource.h"
#include "Utility.h"
#include "UtilityForDirectX.h"
#include "Application.h"
#include "CommandManager.h"
#include "UtilityForException.h"

// Create texture resource
void CTextureResource::Create(TexLoadResult* loadResult) {
    // If the sent variable for result is nullptr, early return
    if (loadResult == nullptr) { return; }

    // Copy meta data
    m_metaData = loadResult->metaData;
    // Extract raw data
    const DirectX::Image* img = loadResult->scratchImg.GetImage(0, 0, 0);

    // Copy only the necessary variables
    m_perTexProperty.width = img->width;
    m_perTexProperty.height = img->height;
    m_perTexProperty.rowPitch = img->rowPitch;
    m_perTexProperty.slicePitch = img->slicePitch;

    // Calculate aligned row pitch
    UINT alignedRowPitch = Utl::Align<UINT>(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

    // Define Mapping function
    auto mappingFunc = [&](UINT8* mappedBuffer) {
        // Copy one line at a time
        for (size_t y = 0; y < img->height; ++y) {
            // Calculate the starting address of the copy source
            UINT8* srcAddress = img->pixels + y * img->rowPitch;
            // Calculate the starting address of the copy desination
            UINT8* destAddress = mappedBuffer + y * alignedRowPitch;
            std::copy_n(srcAddress, alignedRowPitch, destAddress);
        }
    };

    // Create resource
    CreateResource(alignedRowPitch * img->height, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, mappingFunc);

    // Create descriptor heap
    m_descriptorHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);

    // Create shader resource view desc
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = Utl::Dx::ConvertTexDimension2SRVDimension(m_metaData.dimension);
    srvDesc.Format = m_metaData.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    switch (srvDesc.ViewDimension) {
        // Texture2D
    case D3D12_SRV_DIMENSION_TEXTURE2D:
        srvDesc.Texture2D.MipLevels = 1;              // No mipmap
        srvDesc.Texture2D.MostDetailedMip = 0;        // No mipmap
        srvDesc.Texture2D.PlaneSlice = 0;             // No plane slice
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f; // No limits on the level of detail
        break;
        // Exception
    default:
        throw Utl::Error::CFatalError(L"Texture dimension specified when loading texture are not supported");
    }

    // Create shader resource view
    Utl::Dx::CPU_DESCRIPTOR_HANDLE handle = m_descriptorHeap.GetCpuHandle(0);
    CApplication::GetAny().GetDXDevice()->CreateShaderResourceView(
        m_resource.Get(),
        &srvDesc,
        handle
    );

    // Initialize properties for shader resource view
    m_srvProperty = CUniquePtrWeakable<Utl::Dx::SRVProperty>::Make();
    m_srvProperty->stateMaker->Create(m_resource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    m_srvProperty->width = (UINT)img->width;
    m_srvProperty->height = (UINT)img->height;
    m_srvProperty->cpuHandle = handle;
}

// Load texture
bool CTextureResource::Load(const std::wstring& filePath, TexLoadResult* result) {
    // If the sent variable for result is nullptr, early return
    if (result == nullptr) { return false; }
    
    // Texture load
    HRESULT hr = NULL;
    if (filePath.size() >= 4 && filePath.substr(filePath.size() - 4) == L".tga") {
        hr = DirectX::LoadFromTGAFile(filePath.c_str(), // File path
            &result->metaData, result->scratchImg
        );
    }
    else {
        hr = DirectX::LoadFromWICFile(filePath.c_str(), // File path
            DirectX::WIC_FLAGS_NONE,                    // Default
            &result->metaData, result->scratchImg
        );
    }

    return SUCCEEDED(hr);
}

// Load texture
bool CTextureResource::Load(const void* data, size_t size, const std::wstring& filePath, TexLoadResult* result) {
    // If the sent variable for result is nullptr, early return
    if (result == nullptr) { return false; }

    // Texture load
    HRESULT hr = NULL;
    // .tga file
    if (filePath.size() >= 4 && filePath.substr(filePath.size() - 4) == L".tga") {
        hr = DirectX::LoadFromTGAMemory(data, size, &result->metaData, result->scratchImg);
    }
    // .png .jpg files
    else {
        hr = DirectX::LoadFromWICMemory(data, size,
            DirectX::WIC_FLAGS_NONE,
            &result->metaData, result->scratchImg
        );
    }

    return SUCCEEDED(hr);
}

// Update heap properties and resource desc for resource
void CTextureResource::UpdateDataForResource(D3D12_HEAP_PROPERTIES* heapProp, D3D12_RESOURCE_DESC* resDesc) {
    // Update only the necessary variables
    heapProp->Type            = D3D12_HEAP_TYPE_DEFAULT;                                     // Fast access from GPU
    resDesc->Dimension        = static_cast<D3D12_RESOURCE_DIMENSION>(m_metaData.dimension); // Texture dimension
    resDesc->Format           = m_metaData.format;                                           // Texture format
    resDesc->Width            = (UINT)m_metaData.width;                                      // The number of pixel width
    resDesc->Height           = (UINT)m_metaData.height;                                     // The number of pixel height
    resDesc->DepthOrArraySize = (UINT)m_metaData.arraySize;                                  // Texture array size
    resDesc->MipLevels        = (UINT16)m_metaData.mipLevels;                                // Texture mipmap level
    resDesc->Layout           = D3D12_TEXTURE_LAYOUT_UNKNOWN;                                // Not specify layout
}

// Set command to copy data
void CTextureResource::CopyRegion(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* srcResource) {
    // Calculate aligned row pitch
    UINT alignedRowPitch = Utl::Align<UINT>(m_perTexProperty.rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

    // Copy source setting
    D3D12_TEXTURE_COPY_LOCATION src;
    src.pResource                          = srcResource;                              // Intermidiate buffer
    src.Type                               = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT; // Specify footprint since it is a buffer resource
    src.PlacedFootprint.Offset             = 0;                                        // Not offset
    src.PlacedFootprint.Footprint.Format   = m_metaData.format;                        // Texture format
    src.PlacedFootprint.Footprint.Width    = (UINT)m_metaData.width;                   // The number of pixel width
    src.PlacedFootprint.Footprint.Height   = (UINT)m_metaData.height;                  // The number of pixel height
    src.PlacedFootprint.Footprint.Depth    = (UINT)m_metaData.depth;                   // Texture array size
    src.PlacedFootprint.Footprint.RowPitch = alignedRowPitch;                          // Bytes per line
    // Copy destination setting
    D3D12_TEXTURE_COPY_LOCATION dst;
    dst.pResource        = m_resource.Get();                          // Specify resource
    dst.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX; // Specify index since it is a texture resource
    dst.SubresourceIndex = 0;                                         // Not mipmap

    // Copy the intermidiate buffer to the texture resource
    cmdList->CopyTextureRegion(
        &dst,    // Copy destination
        0, 0, 0, // Copy destination start area is all 0
        &src,    // Copy source
        nullptr  // No mipmap, etc.
    );
}
