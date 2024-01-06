#include "GraphicsObjectAsset.h"
#include "GraphicsPipelineState.h"
#include "DescriptorHeapPool.h"

//  Add a shader resource view
void CGraphicsObjectAsset::AddSRV(CTexture tex) {
    m_srvs.push_back(std::move(tex));
}

// Add a constant buffer view 
void CGraphicsObjectAsset::AddCBV(Utl::Dx::ShaderString hlslName, Utl::Dx::CPU_DESCRIPTOR_HANDLE cpuHandle) {
    m_cbvs.push_back(std::make_pair(std::move(hlslName), cpuHandle));
}

// Ask for SRVs assignment information to be built
bool CGraphicsObjectAsset::AskToAllocateSRV(const CGraphicsPipelineState& gpso, UINT totalNum) {
    CDescriptorHeapPool* pool = &CDescriptorHeapPool::GetMain();

    for (auto& tex : m_srvs) {
        const Utl::Dx::ShaderString& hlslName = tex.GetHLSLName();

        // Get offset of texture resource
        int offset = gpso.GetDynamicResourceOffset(hlslName);

        // If it is not found
        if (offset < 0) {
            continue;
        }
        // If it is found
        else {
            // Get handle
            Utl::Dx::SRVPropertyHandle* handle = tex.GetHandle();

            // If instance doesn't exists, Not allocate
            if (!handle->IsInstance()) {
                continue;
            }

            // Check resource state
            handle->GetStateController()->StateTransition(Utl::Dx::ConvertShaderType2TextureResourceState(hlslName.GetShaderType()));

            // Copy the shader resource view to the sent descriptor heap
            CApplication::GetAny().GetDXDevice()->CopyDescriptorsSimple(1,
                pool->GetCPUHandle((UINT)offset),
                handle->GetCPUHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
            );

            // Decrement remain number of srv to be used
            totalNum--;

            // If all SRVs have been allocated, exit as success
            if (totalNum <= 0) {
                return true;
            }
        }
    }

    // Failure if all textures are not found
    return false;
}
