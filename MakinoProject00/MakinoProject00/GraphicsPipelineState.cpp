#include "GraphicsPipelineState.h"
#include "CommandManager.h"
#include "DynamicCbRegistry.h"
#include "DynamicSbRegistry.h"
#include "GraphicsLayerRegistry.h"
#include "StaticResourceRegistry.h"
#include "Scene.h"
#include "DescriptorHeapPool.h"
#include "UtilityForException.h"

// Constructor
CGraphicsPipelineState::CGraphicsPipelineState()
    : m_scene(nullptr)
    , m_dsvState(D3D12_RESOURCE_STATE_DEPTH_READ)
    , m_totalDynamicDescRangeNum(0)
    , m_totalDynamicTextureNum(0)
    , m_totalStaticDescRangeNum(0)
#ifdef _DEBUG
    , m_gpsoName()
#endif // _DEBUG
{ }

// Destructor
CGraphicsPipelineState::~CGraphicsPipelineState() {
}

// Create graphics pipeline state
void CGraphicsPipelineState::Create(ACScene* scene, const std::wstring gpsoName, const Gpso::GPSOSetting& setting, 
    std::initializer_list<GraphicsLayer> useLayers, std::initializer_list<GraphicsComponentType> useTypes) {
    // Pass to function for std::vector
    Create(scene, gpsoName, setting, useLayers, std::vector<GraphicsComponentType>(useTypes.begin(), useTypes.end()));
}

// Create graphics pipeline state
void CGraphicsPipelineState::Create(ACScene* scene, const std::wstring gpsoName, const Gpso::GPSOSetting& setting, 
    std::initializer_list<GraphicsLayer> useLayers, std::vector<GraphicsComponentType> useTypes) {
    if (m_gpso != nullptr) {
        throw Utl::Error::CFatalError(L"This gpso has been already created!" + gpsoName);
    }
    
    // Initialize variables
    m_scene = scene->WeakFromThis();
#ifdef _DEBUG
    m_gpsoName = gpsoName;
#endif // _DEBUG

    // Assign all layers to be used
    for (auto it : useLayers) {
        m_useLayers.push_back(it);
    }

    // Assign all graphics component types to be used
    m_useComponentTypes = std::move(useTypes);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

    // Bind rasterizer state
    Gpso::BindRasterizerState(&gpipeline.RasterizerState, setting.rasterizerState);

    // Render target setting
    gpipeline.NumRenderTargets = (UINT)setting.rtvFormats.size();
    gpipeline.BlendState.AlphaToCoverageEnable = setting.isUsingMultisampleAlpha;
    gpipeline.BlendState.IndependentBlendEnable = TRUE; // All blend state settings are considered as be independent
    for (UINT i = 0; i < gpipeline.NumRenderTargets && i < 8; ++i) {
        // Assign blend states to each render target
        Gpso::BindBlendState(&gpipeline.BlendState.RenderTarget[i], setting.rtvFormats[i].second);

        // Use specified format
        gpipeline.RTVFormats[i] = setting.rtvFormats[i].first;
    }

    // Depth stencil setting
    Gpso::BindDepthSetting(&gpipeline.DepthStencilState, setting.depth);
    Gpso::BindStencilSetting(&gpipeline.DepthStencilState, setting.stencil);
    gpipeline.DSVFormat = setting.dsvFormat;

    // Check if writing to the depth stencil buffer is necessary
    bool isNeedToWrite = (gpipeline.DepthStencilState.DepthWriteMask == D3D12_DEPTH_WRITE_MASK_ALL);
    if (isNeedToWrite == false && gpipeline.DepthStencilState.StencilWriteMask != 0) {
        isNeedToWrite =
            gpipeline.DepthStencilState.BackFace.StencilFailOp != D3D12_STENCIL_OP_KEEP ||
            gpipeline.DepthStencilState.BackFace.StencilDepthFailOp != D3D12_STENCIL_OP_KEEP ||
            gpipeline.DepthStencilState.BackFace.StencilPassOp != D3D12_STENCIL_OP_KEEP ||
            gpipeline.DepthStencilState.FrontFace.StencilFailOp != D3D12_STENCIL_OP_KEEP ||
            gpipeline.DepthStencilState.FrontFace.StencilDepthFailOp != D3D12_STENCIL_OP_KEEP ||
            gpipeline.DepthStencilState.FrontFace.StencilPassOp != D3D12_STENCIL_OP_KEEP;
    }
    m_dsvState = (isNeedToWrite) ? D3D12_RESOURCE_STATE_DEPTH_WRITE : D3D12_RESOURCE_STATE_DEPTH_READ;

    // Copy other settings
    gpipeline.PrimitiveTopologyType = setting.primitiveTopologyType;
    gpipeline.IBStripCutValue = setting.ibStripCutValue;
    gpipeline.SampleDesc = setting.sampleDesc;
    gpipeline.SampleMask = setting.sampleMask;

    // Bind shader data
    std::vector<D3D12_ROOT_PARAMETER> rootParams;
    std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;
    BindShaderInfo(setting.vs, &gpipeline, &rootParams, &samplers);
    BindShaderInfo(setting.ps, &gpipeline, &rootParams, &samplers);
    // Bind shader data for dynamic resources
    BindShaderInfoDynamic(setting.vs, &gpipeline, &rootParams);
    BindShaderInfoDynamic(setting.ps, &gpipeline, &rootParams);

    // Create root signature description
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    // Set descriptor tables
    if (rootParams.size() > 0) {
        rootSignatureDesc.pParameters = &rootParams[0];
        rootSignatureDesc.NumParameters = (UINT)rootParams.size();
    }
    else {
        rootSignatureDesc.pParameters = nullptr;
        rootSignatureDesc.NumParameters = 0;
    }
    // Set sampler states
    if (samplers.size() > 0) {
        rootSignatureDesc.pStaticSamplers = &samplers[0];
        rootSignatureDesc.NumStaticSamplers = (UINT)samplers.size();
    }
    else {
        rootSignatureDesc.pStaticSamplers = nullptr;
        rootSignatureDesc.NumStaticSamplers = 0;
    }

    // Create binary code for root signature
    ID3DBlob* rootSigBlob = nullptr;
    HR_CHECK(L"Create binary code for root signature",
        D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, nullptr));

    // Create root signature object
    HR_CHECK(L"Create root signature object",
        CApplication::GetAny().GetDXDevice()->CreateRootSignature(
            0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf())));
    rootSigBlob->Release();

    // Set root signature
    gpipeline.pRootSignature = m_rootSignature.Get();

    // Create graphics pipeline state object
    HR_CHECK(L"Create graphics pipeline state object",
        CApplication::GetAny().GetDXDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(m_gpso.GetAddressOf())));

    // Send this graphics pipeline state to the scene
    m_scene->AddGraphicsPipelineStatePointer(this);
}

// Set this graphics pipeline state to command list
void CGraphicsPipelineState::SetCommand() {
    // Variable declarations
    ID3D12GraphicsCommandList* cmdList = CCommandManager::GetMain().GetGraphicsCmdList();
    CDescriptorHeapPool* pool = &CDescriptorHeapPool::GetMain();
    ID3D12Device* device = CApplication::GetAny().GetDXDevice();

#ifdef _DEBUG
    // Begin GPSO event
    PIX_BEGIN_EVENT(cmdList, PIX_COLOR_DEFAULT, (L"GPSO Event: " + m_gpsoName).c_str());
#endif // _DEBUG

    // Transition the state of a current applied depth stencil
    CDepthStencil* currentAppliedDSV = ACRenderTarget::GetCurrentAppliedDepthStencil();
    if (currentAppliedDSV != nullptr) {
        currentAppliedDSV->StateTransition(m_dsvState);
    }

    // Set gpso and root signature
    cmdList->SetPipelineState(m_gpso.Get());
    cmdList->SetGraphicsRootSignature(m_rootSignature.Get());

    // If the pool has no available region for any object, advance to the next descriptor heap
    if (!pool->CheckAvailableCapacity(m_totalStaticDescRangeNum + m_totalDynamicDescRangeNum)) {
        pool->AdvanceNextDescriptorHeap();
    }

    // Update static resources
    UpdateStaticResources();

    // Begin event to render external objects
    PIX_BEGIN_EVENT(cmdList, PIX_COLOR_DEFAULT, L"Rendering Event: External Objects");
    // Process all externally specified assets
    for (auto it = m_externalGraphicsObjectAssets.begin(); it != m_externalGraphicsObjectAssets.end(); ++it) {
        // If it doesn't exists, erase it from array and proceed to next
        if ((*it) == nullptr) {
            m_externalGraphicsObjectAssets.erase(it);
            it--;
            continue;
        }

        // Checks if the descriptor heap region overflows, and if so, advances to the next descriptor heap
        CheckRegionOverflow();

        // Allocate cbvs to the descriptor heap
        const auto* cbvArray = (*it)->GetCBVsArray();
        for (auto& cbv : *cbvArray) {
            // Allocate assets with matching names, if any
            auto it = m_dynamicResNameToOffset.find(cbv.first);
            if (it != m_dynamicResNameToOffset.end()) {
                device->CopyDescriptorsSimple(1,
                    pool->GetCPUHandle(it->second),
                    cbv.second, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            }
        }

        // Create dynamic textures for graphics asset
        if (m_totalDynamicTextureNum > 0) {
            // Ask for textures assignment information to be built
            bool isAllAllocate = (*it)->AskToAllocateSRV(*this, m_totalDynamicTextureNum);

            // If the necessary texture for this GPSO is not found, output a message
            if (!isAllAllocate) {
                OutputDebugString(L"Warning! The necessary texture for this GPSO is not found\n");
            }
        }

        // Get the asset's vertex buffer and index buffer 
        const CMeshBuffer* meshBuff = (*it)->GetMeshBuff();
        const CVertexBuffer* vertBuff = meshBuff->GetVertex();
        const CIndexBuffer* indexBuff = meshBuff->GetIndex();
        if (vertBuff == nullptr || indexBuff == nullptr) {
            OutputDebugString(L"Warning! No vertex or index buffer is allocated for the graphics component\n");
        }
        // Draw object
        else {
            // Associate descriptor heap with root parameter
            UINT tableSize = (UINT)m_dynamicRootDescriptorTableOffset.size();
            for (UINT i = 0; i < tableSize; ++i) {
                cmdList->SetGraphicsRootDescriptorTable(m_totalStaticDescRangeNum + i,
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
    // End event to render external objects
    PIX_END_EVENT(cmdList);

    // Begin event to render each layers objects
    PIX_BEGIN_EVENT(cmdList, PIX_COLOR_DEFAULT, L"Rendering Event: Each Layers Objects");
    // Process all objects belonging to the layers used
    for (GraphicsLayer layer : m_useLayers) {
        // Begin event to render layer objects
        PIX_BEGIN_EVENT(cmdList, PIX_COLOR_DEFAULT, (L"Rendering Event: LayerName." + GraphicsLayerEnumToString(layer)).c_str());

        // Get all components belonging to a layer
        const EachGraphicsComponentVector& componentsArray = m_scene->GetGraphicsLayerRegistry()->GetLayerComponents(layer);

        // Set graphics components to command list
        for (GraphicsComponentType type : m_useComponentTypes) {
            ConstGraphicsComponentVectorVariantPtr arrayPtr = componentsArray.GetTypeArrayConstPtr(type);

            std::visit([this, device, cmdList, pool](auto& arrayPtr) {
                this->SetCommandForComponent(*arrayPtr, device, cmdList, pool);
            }, arrayPtr);
        }

        // Begin event to render layer objects
        PIX_END_EVENT(cmdList);
    }
    // End event to render each layers objects
    PIX_END_EVENT(cmdList);

#ifdef _DEBUG
    // End GPSO event
    PIX_END_EVENT(cmdList);
#endif // _DEBUG
}

// Add asset for graphics object
void CGraphicsPipelineState::AddGraphicsObjectAsset(CWeakPtr<CGraphicsObjectAsset> asset) {
    m_externalGraphicsObjectAssets.push_back(asset);
}

// Processing at end of a frame
void CGraphicsPipelineState::EndFrameProcess() {
    // Clear all CBVs of external object assets
    for (auto& it : m_externalGraphicsObjectAssets) {
        if (it != nullptr) {
            it->ClearCBVs();
        }
    }
}

// Bind information other than dynamic resources, of a shader to be used, to this class
void CGraphicsPipelineState::BindShaderInfo(ACShader* shader, D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpipeline, 
    std::vector<D3D12_ROOT_PARAMETER>* rootParams, std::vector<D3D12_STATIC_SAMPLER_DESC>* samplers) {
    // Null check
    if (shader == nullptr) { return; }

    // Bind this GPSO to the shader
    shader->BindStaticDescriptorTable(rootParams, samplers);
    shader->BindGPSO(gpipeline);

    // Static resources
    auto& nameToIndexMap = shader->GetDescriptorNameToInfoStaticMap();
    for (auto& it : nameToIndexMap) {
        // Search for the same named static resource class, in the list of static resources, and associate it with offset
        IStaticResource* res = CStaticResourceRegistry::GetAny().GetStaticResource(it.first);
        if (res != nullptr) {
            m_staticResources.push_back(std::make_pair(res, m_totalStaticDescRangeNum + it.second.index));
        }
        // If it is not found
        else {
            // Error processing
            throw Utl::Error::CFatalError(L"The static resource contained in the loaded shader, does not exist.");
        }
    }

    // If you have a static descriptor table, add the offset of the root descriptor table for static descriptor table
    UINT size = (UINT)nameToIndexMap.size();
    if (nameToIndexMap.size() > 0) {
        m_staticRootDescriptorTableOffset.push_back(m_totalStaticDescRangeNum);
    }

    // Add the total number of static descriptor range
    m_totalStaticDescRangeNum += size;
}

// Bind information about dynamic resources, of a shader to be used, to this class
void CGraphicsPipelineState::BindShaderInfoDynamic(ACShader* shader, D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpipeline,
    std::vector<D3D12_ROOT_PARAMETER>* rootParams) {
    // Null check
    if (shader == nullptr) { return; }

    // Bind this GPSO to the shader
    shader->BindDynamicDescriptorTable(rootParams);

    // Organize the shader's descriptor table information and store it in this class
    auto& nameToIndexMap = shader->GetDescriptorNameToInfoDynamicMap();
    for (auto& it : nameToIndexMap) {
        // Associate the shader type and dynamic descriptor range name, with offset
        m_dynamicResNameToOffset.emplace(Utl::Dx::ShaderString(shader->GetShaderType(), it.first), m_totalDynamicDescRangeNum + it.second.index);

        switch (it.second.type) {
        // If it is constant buffer
        case D3D_SIT_CBUFFER:
        {
            // Search for the same named constant buffer class, that need to be updated on a per-object basis, 
            // in the list of constant buffers, and associate it with offset
            IDynamicResource* allocator = CDynamicCbRegistry::GetAny().GetCbAllocator(it.first);
            if (allocator) {
                m_dynamicAllocators.emplace_back(std::make_pair(allocator, m_totalDynamicDescRangeNum + it.second.index));
            }
            // If it is not found
            else {
                // Error processing
                throw Utl::Error::CFatalError(L"The constant buffer contained in the loaded shader, does not exist.");
            }
        }
            break;
        // If it is structured buffer
        case D3D_SIT_STRUCTURED:
        {
            // Search for the same named structured buffer class, that need to be updated on a per-object basis, 
            // in the list of structured buffers, and associate it with offset
            IDynamicResource* allocator = m_scene->GetDynamicSbRegistry()->GetSbAllocator(it.first);
            if (allocator) {
                m_dynamicAllocators.emplace_back(std::make_pair(allocator, m_totalDynamicDescRangeNum + it.second.index));
            }
            // If it is not found
            else {
                // Error processing
                throw Utl::Error::CFatalError(L"The constant buffer contained in the loaded shader, does not exist.");
            }
        }
            break;
        // If it is texture
        case D3D_SIT_TEXTURE:
            // Add total number of dynamic textures
            m_totalDynamicTextureNum++;
            break;
        }
    }

    // If you have a dynamic descriptor table, add the offset of the root descriptor table for dynamic descriptor table
    UINT size = (UINT)nameToIndexMap.size();
    if (size > 0) {
        m_dynamicRootDescriptorTableOffset.push_back(m_totalDynamicDescRangeNum);
    }

    // Add the total number of dynamic descriptor range
    m_totalDynamicDescRangeNum += size;
}

// Copy and bind static resources to a descriptor heap
void CGraphicsPipelineState::UpdateStaticResources() {
    CDescriptorHeapPool* pool = &CDescriptorHeapPool::GetMain();

    // Copy all static resources to a descriptor heap
    UINT staticResNum = (UINT)m_staticResources.size();
    for (UINT i = 0; i < staticResNum; ++i) {
        // Copy a static resource
        CApplication::GetAny().GetDXDevice()->CopyDescriptorsSimple(1,
            pool->GetCPUHandle(m_staticResources[i].second),
            m_staticResources[i].first->GetAllocatedDataHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    // Associate descriptor heap with root parameter
    UINT tableSize = (UINT)m_staticRootDescriptorTableOffset.size();
    for (UINT i = 0; i < tableSize; ++i) {
        // Set root descriptor table
        CCommandManager::GetMain().GetGraphicsCmdList()->SetGraphicsRootDescriptorTable(i,
            pool->GetGPUHandle(m_staticRootDescriptorTableOffset[i]));
    }

    // Determine the use of the region based on the number of all static resources
    pool->DetermineUseRegion(m_totalStaticDescRangeNum);
}

// Checks if the descriptor heap region overflows, and if so, advances to the next descriptor heap
void CGraphicsPipelineState::CheckRegionOverflow() {
    CDescriptorHeapPool* pool = &CDescriptorHeapPool::GetMain();

    // If the descriptor heap region overflows, and if so, advance to the next descriptor heap
    if (!pool->CheckAvailableCapacity(m_totalDynamicDescRangeNum)) {
        pool->AdvanceNextDescriptorHeap();

        // Update static resources
        UpdateStaticResources();
    }
}
