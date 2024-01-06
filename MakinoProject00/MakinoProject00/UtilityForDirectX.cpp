#include "UtilityForDirectX.h"
#include "CommandManager.h"

/** @brief Vertex shader attribute */
const char VERTEX_ATTRIBUTE = 'V';
/** @brief Pixe shader attribute */
const char PIXEL_ATTRIBUTE = 'P';

// Create simple heap properties
Utl::Dx::HEAP_PROPERTIES::HEAP_PROPERTIES(D3D12_HEAP_TYPE type) {
    Type                 = type;                            // Sent type
    CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; // Not CUSTOM
    MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;       // Not CUSTOM
    CreationNodeMask     = 0;                               // Single adapter
    VisibleNodeMask      = 0;                               // Single adapter
}

// Create resource desc for simple buffer 
Utl::Dx::RESOURCE_DESC::RESOURCE_DESC(UINT64 size) {
    Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER; // For constant buffer
    Alignment          = 0;                               // Default alignment
    Width              = size;                            // Sent size
    Height             = 1;                               // Single
    DepthOrArraySize   = 1;                               // No texture
    MipLevels          = 1;                               // No texture
    Format             = DXGI_FORMAT_UNKNOWN;             // No texture
    SampleDesc.Count   = 1;                               // No texture
    SampleDesc.Quality = 0;                               // No texture
    Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;  // For constant buffer
    Flags              = D3D12_RESOURCE_FLAG_NONE;        // None
}

// Create transition type resource barrier
Utl::Dx::RESOURCE_BARRIER::RESOURCE_BARRIER(ID3D12Resource* res, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after) {
    Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;  // Transition type
    Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;        // Deafult
    Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; // All subresource
    Transition.pResource   = res;                                     // Set Specified resource
    Transition.StateBefore = before;                                  // Before state
    Transition.StateAfter  = after;                                   // After state
}

// Swap state before transition and state after transition
void Utl::Dx::RESOURCE_BARRIER::StateSwap() {
    auto temp = Transition.StateBefore;
    Transition.StateBefore = Transition.StateAfter;
    Transition.StateAfter = temp;
}

// Transition resource state
void Utl::Dx::ResStateController::StateTransition(D3D12_RESOURCE_STATES state) {
    // If already in the specified state, early return 
    if (m_currentState == state) {
        return;
    }

    // Transition resource state
    Utl::Dx::RESOURCE_BARRIER barrier(m_resource, m_currentState, state);
    CCommandManager::GetMain().GetGraphicsCmdList()->ResourceBarrier(1, &barrier);
    m_currentState = state;
}

// Constructor
Utl::Dx::ShaderString::ShaderString(Utl::Dx::ShaderType type, std::string str)
    : m_attributeStr(std::move(*AddAttributeToStr(type, &str)))
{ }

// Add attribute to the string indicating which shader
std::string* Utl::Dx::ShaderString::AddAttributeToStr(Utl::Dx::ShaderType type, std::string* str) {
    // Add attribute indicating which shader
    switch (type) {
    case Utl::Dx::ShaderType::Vertex:
        *str += VERTEX_ATTRIBUTE;
        break;
    case Utl::Dx::ShaderType::Pixel:
        *str += PIXEL_ATTRIBUTE;
        break;
    default:
        break;
    }
    return str;
}

// Get the shader type
const Utl::Dx::ShaderType Utl::Dx::ShaderString::GetShaderType() const {
    Utl::Dx::ShaderType ret;

    // Check attribute
    const char attribute = m_attributeStr.back();
    switch (attribute) {
        // Vertex
    case VERTEX_ATTRIBUTE:
        ret = Utl::Dx::ShaderType::Vertex;
        break;
        // Pixel
    case PIXEL_ATTRIBUTE:
        ret = Utl::Dx::ShaderType::Pixel;
        break;
        // Exceptions
    default:
        ret = Utl::Dx::ShaderType::Vertex;
        break;
    }

    return ret;
}

// Get handle for this property
Utl::Dx::SRVPropertyHandle Utl::Dx::ASRVProperty::GetHandle() {
    return SRVPropertyHandle(this);
}

// Convert 'SHADER_TYPE' to 'D3D12_SHADER_VISIBILITY'
D3D12_SHADER_VISIBILITY Utl::Dx::ConvertShaderType2ShaderVisibility(ShaderType type) {
    switch (type) {
    // Vertex
    case ShaderType::Vertex:
        return D3D12_SHADER_VISIBILITY_VERTEX;
        break;
    // Pixel
    case ShaderType::Pixel:
        return D3D12_SHADER_VISIBILITY_PIXEL;
        break;
    // Other
    default:
        return D3D12_SHADER_VISIBILITY_ALL;
        break;
    }
}

// Get the resource state for texture, applicable to the shader type
D3D12_RESOURCE_STATES Utl::Dx::ConvertShaderType2TextureResourceState(ShaderType type) {
    if (type == ShaderType::Pixel) {
        return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    }
    else {
        return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    }
}

// Convert 'DirectX::TEX_DIMENSION' to 'D3D12_SRV_DIMENSION'
D3D12_SRV_DIMENSION Utl::Dx::ConvertTexDimension2SRVDimension(DirectX::TEX_DIMENSION texDimension) {
    switch (texDimension) {
    case DirectX::TEX_DIMENSION_TEXTURE1D:
        return D3D12_SRV_DIMENSION_TEXTURE1D;
    case DirectX::TEX_DIMENSION_TEXTURE2D:
        return D3D12_SRV_DIMENSION_TEXTURE2D;
    case DirectX::TEX_DIMENSION_TEXTURE3D:
        return D3D12_SRV_DIMENSION_TEXTURE3D;
    default:
        return D3D12_SRV_DIMENSION_TEXTURE2D;
    }
}
