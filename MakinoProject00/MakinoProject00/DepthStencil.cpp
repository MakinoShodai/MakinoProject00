#include "DepthStencil.h"
#include "CommandManager.h"
#include "UtilityForException.h"

// Create depth stencil resource
void CDepthStencil::Create(DXGI_FORMAT format, UINT width, UINT height, D3D12_DEPTH_STENCIL_VALUE defaultClearValue) {
    // Create heap property
    Utl::Dx::HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
    // Create resource descriptor
    D3D12_RESOURCE_DESC resDesc = {};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resDesc.Alignment = 0;
    resDesc.Width = width;
    resDesc.Height = height;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.Format = format;
    resDesc.SampleDesc.Count = 1;
    resDesc.SampleDesc.Quality = 0;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // Depth stencil

    // Create default clear value
    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = format;
    clearValue.DepthStencil = defaultClearValue;

    // Create render target
    CApplication::GetAny().GetDXDevice()->CreateCommittedResource(
        &heapProp, D3D12_HEAP_FLAG_NONE,
        &resDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue, IID_PPV_ARGS(m_depthStencil.GetAddressOf())
    );

    // Create descriptor heap for DSV
    m_dsvDescriptorHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
    // Create depth stencil view
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = format;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    CApplication::GetAny().GetDXDevice()->CreateDepthStencilView(m_depthStencil.Get(), &dsvDesc, m_dsvDescriptorHeap.GetCpuHandle(0));

    // Create descriptor heap for SRV
    UINT srvNum = IsStencilUseFormat(format) ? 2 : 1;
    m_srvDescriptorHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, srvNum);
    // Create shader resource view for depth buffer
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;              // 2D texture
    srvDesc.Format                        = ConvertDSVFormat2SRVFormatForDepth(format); // Supported format
    srvDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;   // default
    srvDesc.Texture2D.MipLevels           = 1;                                          // No mipmap
    srvDesc.Texture2D.MostDetailedMip     = 0;                                          // No mipmap
    srvDesc.Texture2D.PlaneSlice          = 0;                                          // No plane slice
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;                                       // No limits on the level of detail
    CApplication::GetAny().GetDXDevice()->CreateShaderResourceView(m_depthStencil.Get(), &srvDesc, m_srvDescriptorHeap.GetCpuHandle(0));

    // Initialize properties for shader resource view
    m_srvPropertyForDepth = CUniquePtrWeakable<Utl::Dx::SRVProperty>::Make();
    m_srvPropertyForDepth->stateMaker->Create(m_depthStencil.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
    m_srvPropertyForDepth->width = width;
    m_srvPropertyForDepth->height = height;
    m_srvPropertyForDepth->cpuHandle = m_srvDescriptorHeap.GetCpuHandle(0);

    // If a stencil buffer uses
    if (srvNum == 2) {
        // Create shader resource view for stencil buffer
        srvDesc.Format = ConvertDSVFormat2SRVFormatForStencil(format); // Supported format
        CApplication::GetAny().GetDXDevice()->CreateShaderResourceView(m_depthStencil.Get(), &srvDesc, m_srvDescriptorHeap.GetCpuHandle(1));

        // Initialize properties for shader resource view
        m_srvPropertyForStencil = CUniquePtrWeakable<Utl::Dx::SRVPropertyNoStateMaker>::Make();
        m_srvPropertyForStencil->stateController = m_srvPropertyForDepth->stateMaker.GetWeakPtr();
        m_srvPropertyForStencil->width = width;
        m_srvPropertyForStencil->height = height;
        m_srvPropertyForStencil->cpuHandle = m_srvDescriptorHeap.GetCpuHandle(1);
    }
}

// Clear the depth stencil
void CDepthStencil::Clear(D3D12_DEPTH_STENCIL_VALUE clearValue, D3D12_CLEAR_FLAGS clearFlag) {
    // Check state
    StateTransition(D3D12_RESOURCE_STATE_DEPTH_WRITE);

    // Clear
    CCommandManager::GetMain().GetGraphicsCmdList()->ClearDepthStencilView(
        GetCPUHandleDSV(), clearFlag, clearValue.Depth, clearValue.Stencil, 0, nullptr);
}

// Convert DSV format to SRV format for depth buffer
DXGI_FORMAT CDepthStencil::ConvertDSVFormat2SRVFormatForDepth(DXGI_FORMAT dsvFormat) {
    switch (dsvFormat) {
    case DXGI_FORMAT_D16_UNORM:
        return DXGI_FORMAT_R16_UNORM;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
    case DXGI_FORMAT_D32_FLOAT:
        return DXGI_FORMAT_R32_FLOAT;
    default:
        throw Utl::Error::CFatalError(L"The SRV format for depth buffer corresponding to the specified DSV format is not defined.");
    }
}

// Convert DSV format to SRV format for stencil buffer
DXGI_FORMAT CDepthStencil::ConvertDSVFormat2SRVFormatForStencil(DXGI_FORMAT dsvFormat) {
    switch (dsvFormat) {
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
    default:
        throw Utl::Error::CFatalError(L"The SRV format for stencil buffer corresponding to the specified DSV format is not defined.");
    }
}

// Is specified format uses stencil buffer?
bool CDepthStencil::IsStencilUseFormat(DXGI_FORMAT dsvFormat) {
    switch (dsvFormat) {
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return true;
    default:
        return false;
    }
}
