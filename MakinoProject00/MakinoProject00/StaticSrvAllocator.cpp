#include "StaticSrvAllocator.h"

// Refresh process that must be called at the end of every frame
void ACStaticSRVAllocator::Refresh() {
    // Refresh process by derived class
    RefreshDerived();
}

// Allocate SRV handle
void ACStaticSRVAllocator::Allocate(CRenderTargetTexture* rtt, bool isUsedPixelShader) {
    m_srvHandle = rtt->GetSRVPropertyHandle();
    m_resState = (isUsedPixelShader) ? D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
}

// Allocate SRV handle
void ACStaticSRVAllocator::Allocate(CDepthStencil* dsv, bool isForStencil, bool isUsedPixelShader) {
    if (false == isForStencil) {
        m_srvHandle = dsv->GetSRVPropertyHandleForDepth();
    }
    else {
        m_srvHandle = dsv->GetSRVPropertyHandleForStencil();
    }
    m_resState = (isUsedPixelShader) ? D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
}

// Allocate SRV handle
void ACStaticSRVAllocator::Allocate(const std::wstring& texPath, bool isUsedPixelShader) {
    m_srvHandle = CTextureRegistry::GetAny().GetSRVPropertyHandle(texPath);
    m_resState = (isUsedPixelShader) ? D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
}

// Get handle of allocated data in resource
Utl::Dx::CPU_DESCRIPTOR_HANDLE ACStaticSRVAllocator::GetAllocatedDataHandle() {
    m_srvHandle.GetStateController()->StateTransition(m_resState);
    // Allocate new data
    return m_srvHandle.GetCPUHandle();
}
