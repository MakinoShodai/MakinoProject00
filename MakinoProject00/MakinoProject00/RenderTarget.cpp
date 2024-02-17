#include "RenderTarget.h"
#include "CommandManager.h"

// Initialize
ACRenderTarget* ACRenderTarget::ms_currentAppliedRenderTarget = nullptr;

// Apply this render target to the destination render target by calling 'OMSetRenderTargets'
void ACRenderTarget::Apply(CDepthStencil* dsv, std::initializer_list<ACRenderTarget*> otherRT) {
    // Check state
    StateTransition(D3D12_RESOURCE_STATE_RENDER_TARGET);

    // Create viewport
    D3D12_VIEWPORT viewPort = {};
    viewPort.Width    = (FLOAT)m_width;  // Set the resolution as it is
    viewPort.Height   = (FLOAT)m_height; // Set the resolution as it is
    viewPort.TopLeftX = 0;               // Set the resolution as it is
    viewPort.TopLeftY = 0;               // Set the resolution as it is
    viewPort.MaxDepth = 1.0f;            // Depth is 0～1
    viewPort.MinDepth = 0.0f;            // Depth is 0～1
    // Create scissor rectangle
    D3D12_RECT scissorRect = {};
    scissorRect.left   = 0;                                // No clipping
    scissorRect.top    = 0;                                // No clipping
    scissorRect.right  = scissorRect.left + (LONG)m_width; // No clipping
    scissorRect.bottom = scissorRect.top + (LONG)m_height; // No clipping
    // Set viewport and scissor rectangle
    auto cmdList = CCommandManager::GetMain().GetGraphicsCmdList();
    cmdList->RSSetViewports(1, &viewPort);
    cmdList->RSSetScissorRects(1, &scissorRect);

    // Calculate the number of all render targets
    UINT rtNum = (UINT)otherRT.size() + 1;

    // Create array of render targets
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles(rtNum);
    
    // Assign itself to the first element
    rtvHandles[0] = GetCPUHandleRTV();
    
    // Set other render targets in array
    UINT i = 0;
    for (auto it : otherRT) {
        rtvHandles[1 + i] = it->GetCPUHandleRTV();
        i++;
    }
    
    // State check process to be called when applying a destination for drawing
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
    if (dsv != nullptr) {
        dsv->CheckStateAtApplying();
        dsvHandle = dsv->GetCPUHandleDSV();
    }

    // Apply render targets to the destination render targets
    cmdList->OMSetRenderTargets(rtNum, &rtvHandles[0], FALSE, (dsv != nullptr) ? &dsvHandle : nullptr);

    // Set this render target to the current applied render target
    ms_currentAppliedRenderTarget = this;
}

// Clear the color of all pixels on this render target
void ACRenderTarget::Clear(const Colorf& color) {
    // Check state
    StateTransition(D3D12_RESOURCE_STATE_RENDER_TARGET);

    // Clear
    CCommandManager::GetMain().GetGraphicsCmdList()->ClearRenderTargetView(GetCPUHandleRTV(), color.GetArrayPtr(), 0, nullptr);
}

// Set resolution
void ACRenderTarget::SetResolution(ID3D12Resource* renderTarget) {
    // Get resolution by render target descriptor
    D3D12_RESOURCE_DESC desc = renderTarget->GetDesc();
    m_width = (UINT)desc.Width;
    m_height = (UINT)desc.Height;
}

// Create render target resource
void CRenderTargetTexture::Create(DXGI_FORMAT format, UINT width, UINT height, const Colorf& defaultClearColor) {
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
    resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // Render target

    // Create default clear value
    D3D12_CLEAR_VALUE clearValue;
    clearValue.Format = format;
    clearValue.Color[0] = defaultClearColor[0];
    clearValue.Color[1] = defaultClearColor[1];
    clearValue.Color[2] = defaultClearColor[2];
    clearValue.Color[3] = defaultClearColor[3];

    // Create render target
    CApplication::GetAny().GetDXDevice()->CreateCommittedResource(
        &heapProp, D3D12_HEAP_FLAG_NONE,
        &resDesc, D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clearValue, IID_PPV_ARGS(m_renderTarget.GetAddressOf())
    );

    // Create descriptor heap for RTV
    m_rtvDescriptorHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
    // Create render target view
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format               = format;                        // Specified format
    rtvDesc.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D; // 2D texture
    rtvDesc.Texture2D.MipSlice   = 0;                             // Mipmap is not used
    rtvDesc.Texture2D.PlaneSlice = 0;                             // Not use
    CApplication::GetAny().GetDXDevice()->CreateRenderTargetView(m_renderTarget.Get(), &rtvDesc, m_rtvDescriptorHeap.GetCpuHandle(0));

    // Create descriptor heap for SRV
    m_srvDescriptorHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 1);
    // Create shader resource view
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;            // 2D texture
    srvDesc.Format                        = format;                                   // Specified format
    srvDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // default
    srvDesc.Texture2D.MipLevels           = 1;                                        // No mipmap
    srvDesc.Texture2D.MostDetailedMip     = 0;                                        // No mipmap
    srvDesc.Texture2D.PlaneSlice          = 0;                                        // No plane slice
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;                                     // No limits on the level of detail
    CApplication::GetAny().GetDXDevice()->CreateShaderResourceView(m_renderTarget.Get(), &srvDesc, m_srvDescriptorHeap.GetCpuHandle());

    // Set resolution
    SetResolution(m_renderTarget.Get());

    // Initialize properties for shader resource view
    m_srvProperty = CUniquePtrWeakable<Utl::Dx::SRVProperty>::Make();
    m_srvProperty->stateMaker->Create(m_renderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_srvProperty->width = m_width;
    m_srvProperty->height = m_height;
    m_srvProperty->cpuHandle = m_srvDescriptorHeap.GetCpuHandle();
}
