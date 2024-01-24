/**
 * @file   GraphicsPipelineState.h
 * @brief  This file handles graphics pipeline states.
 * 
 * @author Shodai Makino
 * @date   2023/11/9
 */

#ifndef __GRAPHICS_PIPELINE_STATE_H__
#define __GRAPHICS_PIPELINE_STATE_H__

#include "GraphicsPipelineElements.h"
#include "GraphicsComponent.h"
#include "DynamicCbAllocator.h"
#include "GraphicsLayer.h"
#include "GraphicsObjectAsset.h"
#include "DescriptorHeap.h"
#include "DescriptorHeapPool.h"
#include "Shader.h"
#include "StaticResource.h"
#include "SwapChain.h"

// Forward declaration
class ACScene;

/** @brief This class handles graphics pipeline state */
class CGraphicsPipelineState {
public:
    /**
       @brief Constructor
    */
    CGraphicsPipelineState();

    /**
       @brief Destructor
    */
    ~CGraphicsPipelineState();

    /**
       @brief Create graphics pipeline state
       @param scene Scene that has this graphics pipeline state
       @param gpsoName The name of this gpso. Used for debugging
       @param setting Graphics pipeline state setting
       @param useLayer Layers handled by this GPSO
       @param useTypes Graphics component types handled by this GPSO
    */
    void Create(ACScene* scene, const std::wstring gpsoName, const Gpso::GPSOSetting& setting, std::initializer_list<GraphicsLayer> useLayers, std::initializer_list<GraphicsComponentType> useTypes);

    /**
       @brief Create graphics pipeline state
       @param scene Scene that has this graphics pipeline state
       @param gpsoName The name of this gpso. Used for debugging
       @param setting Graphics pipeline state setting
       @param useLayer Layers handled by this GPSO
       @param useTypes Graphics component types handled by this GPSO
    */
    void Create(ACScene* scene, const std::wstring gpsoName, const Gpso::GPSOSetting& setting, std::initializer_list<GraphicsLayer> useLayers, std::vector<GraphicsComponentType> useTypes);

    /**
       @brief Set this graphics pipeline state to command list
    */
    void SetCommand();

    /**
       @brief Get the offset of a dynamic resource
       @param resName The name of a dynamic resource in hlsl
       @return If a dynamic resource with the same name is found, return its offset. if not, return -1
    */
    int GetDynamicResourceOffset(const Utl::Dx::ShaderString& resName) const { auto it = m_dynamicResNameToOffset.find(resName); return (it != m_dynamicResNameToOffset.end()) ? (int)it->second : -1; }

    /**
       @brief Add asset for graphics object
       @param asset Asset to add
       @details
       If you want to remove the added assets, destroy the original CUniquePtrWeakable.
    */
    void AddGraphicsObjectAsset(CWeakPtr<CGraphicsObjectAsset> asset);

    /**
       @brief Processing at end of a frame
    */
    void EndFrameProcess();

private:
    /**
       @brief Set sent graphics components to command list
       @param componentsArray Dynamic array of graphics components
       @param device DirectX device object
       @param cmdList Graphics command list object
       @param pool Descriptor heap pool class
    */
    template <class T>
    void SetCommandForComponent(const std::vector<T*>& componentsArray, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, CDescriptorHeapPool* pool);

    /**
       @brief Bind information other than dynamic resources, of a shader to be used, to this class
       @param shader Shader to be used
       @param gpipeline Structure for Graphics pipeline state setting
       @param rootParams Pointer to root parameters array
       @param samplers Pointer to sampler descriptors array
    */
    void BindShaderInfo(ACShader* shader, D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpipeline, 
        std::vector<D3D12_ROOT_PARAMETER>* rootParams, std::vector<D3D12_STATIC_SAMPLER_DESC>* samplers);

    /**
       @brief Bind information about dynamic resources, of a shader to be used, to this class
       @param shader Shader to be used
       @param gpipeline Structure for Graphics pipeline state setting
       @param rootParams Pointer to root parameters array
    */
    void BindShaderInfoDynamic(ACShader* shader, D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpipeline,
        std::vector<D3D12_ROOT_PARAMETER>* rootParams);

    /**
       @brief Copy and bind static resources to a descriptor heap
    */
    void UpdateStaticResources();

    /**
       @brief Checks if the descriptor heap region overflows, and if so, advances to the next descriptor heap
    */
    void CheckRegionOverflow();

private:
    /** @brief Weak pointer to the scene that has this graphics pipeline state */
    CWeakPtr<ACScene> m_scene;
    /** @brief Graphics pipeline state object */
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_gpso;
    /** @brief Root signature object */
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
    /** @brief Resource state of depth stencil used */
    D3D12_RESOURCE_STATES m_dsvState;

    /** @brief Map associating a resource name in hlsl with its index in the descriptor heap. (for dynamic resources) */
    std::unordered_map<Utl::Dx::ShaderString, UINT, Utl::Dx::ShaderStringHash> m_dynamicResNameToOffset;
    /** @brief Dynamic resource allocators and its offset */
    std::vector<std::pair<IDynamicResource*, UINT>> m_dynamicAllocators;
    /** @brief Array containing the offsets, of the root descriptor table for dynamic descriptor table, in order */
    std::vector<UINT> m_dynamicRootDescriptorTableOffset;
    /** @brief Total number of dynamic descriptor ranges */
    UINT m_totalDynamicDescRangeNum;
    /** @brief Total number of dynamic textures in descriptor range */
    UINT m_totalDynamicTextureNum;

    /** @brief All static resources that don't need to be updated on a per-object basis, and its offset */
    std::vector<std::pair<IStaticResource*, UINT>> m_staticResources;
    /** @brief Array containing the offsets, of the root descriptor table for static descriptor table, in order */
    std::vector<UINT> m_staticRootDescriptorTableOffset;
    /** @brief Total number of static descriptor ranges */
    UINT m_totalStaticDescRangeNum;
    
    /** @brief Assets for graphics objects externally */
    std::vector<CWeakPtr<CGraphicsObjectAsset>> m_externalGraphicsObjectAssets;
    /** @brief Layers handled by this GPSO */
    std::vector<GraphicsLayer> m_useLayers;
    /** @brief Graphics component types handled by this GPSO */
    std::vector<GraphicsComponentType> m_useComponentTypes;

#ifdef _DEBUG
    /** @brief The name of this gpso. Used for debbuging */
    std::wstring m_gpsoName;
#endif // _DEBUG
};

// Set sent graphics components to command list
template<class T>
void CGraphicsPipelineState::SetCommandForComponent(const std::vector<T*>& componentsArray, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, CDescriptorHeapPool* pool) {
    for (T* component : componentsArray) {
        if (!component->IsActiveOverall() || !component->IsDrawCondition()) { continue; }

        // Draw on all meshes the component has
        UINT meshNum = component->GetMeshNum();
        for (UINT meshIndex = 0; meshIndex < meshNum; ++meshIndex) {
            // Get the mesh's vertex buffer and index buffer 
            const CMeshBuffer* meshBuff = component->GetMeshBuffer(meshIndex);
            const CVertexBuffer* vertBuff = meshBuff->GetVertex();
            const CIndexBuffer* indexBuff = meshBuff->GetIndex();
            // If can't find either of them, output a message
            if (vertBuff == nullptr || indexBuff == nullptr) {
                OutputDebugString(L"Warning! No vertex or index buffer is allocated for the graphics component\n");
                continue;
            }

            // Checks if the descriptor heap region overflows, and if so, advances to the next descriptor heap
            CheckRegionOverflow();

            // Create constant buffer views, that need to be updated on a per-object basis, for component
            for (auto& cbv : m_dynamicAllocators) {
                device->CopyDescriptorsSimple(1,
                    pool->GetCPUHandle(cbv.second),
                    cbv.first->GetAllocatedDataHandle(component, meshIndex), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }

            // Create dynamic textures for component
            if (m_totalDynamicTextureNum > 0) {
                // Ask for textures assignment information to be built
                bool isAllAllocate = component->AskToAllocateTextures(meshIndex, *this, m_totalDynamicTextureNum);

                // If the necessary texture for this GPSO is not found, output a message
                if (!isAllAllocate) {
                    OutputDebugString(L"Warning! The necessary texture for this GPSO is not found\n");
                }
            }

            // Draw object
            {
                // Associate descriptor heap with root parameter
                UINT staticTableSize = (UINT)m_staticRootDescriptorTableOffset.size();
                UINT tableSize = (UINT)m_dynamicRootDescriptorTableOffset.size();
                for (UINT i = 0; i < tableSize; ++i) {
                    cmdList->SetGraphicsRootDescriptorTable(staticTableSize + i,
                        pool->GetGPUHandle(m_dynamicRootDescriptorTableOffset[i]));
                }

                // Set vertex buffer view
                cmdList->IASetVertexBuffers(0, 1, vertBuff->GetView());
                // Set index buffer view
                cmdList->IASetIndexBuffer(indexBuff->GetView());

                // Set primitive topology
                cmdList->IASetPrimitiveTopology(meshBuff->GetTopologyType());

                // Draw
                cmdList->DrawIndexedInstanced(
                    indexBuff->GetNumIndices(), // The number of indices
                    1, // The number of instance
                    0, // Index offset
                    0, // Vertex offset
                    0  // Instance offset
                );
            }

            // Determine the use of the region based on the number of dynamic resources per object
            pool->DetermineUseRegion(m_totalDynamicDescRangeNum);
        }
    }
}

#endif // !__GRAPHICS_PIPELINE_STATE_H__
