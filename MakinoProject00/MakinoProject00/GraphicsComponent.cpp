#include "GraphicsComponent.h"
#include "GraphicsLayerRegistry.h"
#include "GraphicsPipelineState.h"
#include "GameObject.h"
#include "Scene.h"
#include "DescriptorHeapPool.h"

// Constructor
ACGraphicsComponent::ACGraphicsComponent(GraphicsComponentType type, CGameObject* owner, GraphicsLayer layer)
    : m_type(type)
    , m_gameObj(owner->WeakFromThis())
    , m_layer(layer)
    , m_texNumPerMesh(0)
    , m_color(Colorf::Ones())
    , m_isActive(true) {
}

// Destructor
ACGraphicsComponent::~ACGraphicsComponent() {
    // Release structured buffer corresponding to this graphics component
    m_gameObj->GetScene()->GetDynamicSbRegistry()->ReleaseBuffer(this);
    
    // Release meshes keys
    CIntKeyGenerater* meshKeyGenerator = m_gameObj->GetScene()->GetMeshKeyGenerater();
    UINT meshNum = m_meshWrappers.Size();
    for (UINT i = 0; i < meshNum; ++i) {
        meshKeyGenerator->ReleaseKey(m_meshWrappers[i].key);
    }

    // Exclude this graphcis component from the registry
    m_gameObj->GetScene()->GetGraphicsLayerRegistry()->Exclude(this);
}

// Processing for the first draw frame
void ACGraphicsComponent::Start() {
    // Register this graphics component in the registry
    m_gameObj->GetScene()->GetGraphicsLayerRegistry()->Register(this);
}

// Ask for textures assignment information to be built
bool ACGraphicsComponent::AskToAllocateTextures(UINT meshNum, const CGraphicsPipelineState& gpso, UINT totalNum) {
    CDescriptorHeapPool* pool = &CDescriptorHeapPool::GetMain();

    for (UINT i = 0; i < m_texNumPerMesh; ++i) {
        CTexture* tex = m_textures[meshNum][i].Get();
        const Utl::Dx::ShaderString& hlslName = tex->GetHLSLName();

        // Get offset of texture resource
        int offset = gpso.GetDynamicResourceOffset(hlslName);

        // If it is not found
        if (offset < 0) {
            continue;
        }
        // If it is found
        else {
            // Get handle
            Utl::Dx::SRVPropertyHandle* handle = tex->GetHandle();

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

            // Decrement remain number of textures to be used
            totalNum--;

            // If all textures have been allocated, exit as success
            if (totalNum <= 0) {
                return true;
            }
        }
    }

    // Failure if all textures are not found
    return false;
}

// Belong to a new layer
void ACGraphicsComponent::SetLayer(GraphicsLayer layer) {
    CGraphicsLayerRegistry* registry = m_gameObj->GetScene()->GetGraphicsLayerRegistry();

    // Move layer
    registry->Exclude(this);
    m_layer = layer;
    registry->Register(this);
}

// Is this graphics component itself and the game object that owns it active?
bool ACGraphicsComponent::IsActiveOverall() const {
    return m_isActive && m_gameObj->IsActive();
}

// Prepare arrays of the meshes and its textures
void ACGraphicsComponent::InitializeMesh(UINT meshNum, UINT texNumPerMesh) {
    // Initialize the number of meshes array
    m_meshWrappers.Resize(meshNum);

    // Allocate keys to the all meshes
    CIntKeyGenerater* meshKeyGenerater = m_gameObj->GetScene()->GetMeshKeyGenerater();
    for (UINT i = 0; i < meshNum; ++i) {
        m_meshWrappers[i].key = meshKeyGenerater->GenerateKey();
    }

    // Initialize the number of textures arrays
    if (texNumPerMesh > 0) {
        m_textures = CUniquePtr<UniquePtrTextureArray[]>::Make(meshNum);
        for (size_t i = 0; i < meshNum; ++i) {
            m_textures[i] = UniquePtrTextureArray::Make(texNumPerMesh);
        }
    }

    // Set sent argument
    m_texNumPerMesh = texNumPerMesh;
}
