#include "GraphicsComponent.h"
#include "GraphicsLayerRegistry.h"
#include "GraphicsPipelineState.h"
#include "GameObject.h"
#include "Scene.h"
#include "DescriptorHeapPool.h"
#include "SceneRegistry.h"

// Constructor
ACGraphicsComponent::ACGraphicsComponent(GraphicsComponentType type, CGameObject* owner, GraphicsLayer layer)
    : m_type(type)
    , m_gameObj(owner->WeakFromThis())
    , m_layer(layer)
    , m_matNumPerMesh(0)
    , m_color(Colorf::Ones())
    , m_texCoordParam(Vector4f(1.0f, 1.0f, 0.0f, 0.0f))
    , m_isActive(true) {
}

// Destructor
ACGraphicsComponent::~ACGraphicsComponent() {
}

// Processing when this component is added to an object
void ACGraphicsComponent::Awake() {
#ifdef _EDITOR
    if (CSceneRegistry::GetAny().IsEditorMode()) {
        // Register this graphics component in the registry
        m_gameObj->GetScene()->GetGraphicsLayerRegistry()->Register(this);
    }
#endif // _EDITOR
}

// Processing for the first draw frame
void ACGraphicsComponent::Start() {
#ifdef _EDITOR
    if (false == CSceneRegistry::GetAny().IsEditorMode()) { 
        // Register this graphics component in the registry
        m_gameObj->GetScene()->GetGraphicsLayerRegistry()->Register(this);
    }
#else
    // Register this graphics component in the registry
    m_gameObj->GetScene()->GetGraphicsLayerRegistry()->Register(this);
#endif // !_EDITOR

}

// Process to be called at instance destruction
void ACGraphicsComponent::OnDestroy() {
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

// Ask for textures assignment information to be built
bool ACGraphicsComponent::AskToAllocateTextures(UINT meshNum, const CGraphicsPipelineState& gpso, UINT totalNum) {
    CDescriptorHeapPool* pool = &CDescriptorHeapPool::GetMain();

    for (UINT i = 0; i < m_matNumPerMesh; ++i) {
        CTexture* tex = m_materials[meshNum].texture[i].Get();
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

// Set numeric part of the material to all meshes
void ACGraphicsComponent::SetNumericMaterialToAllMesh(const PerMeshMaterialNumeric& numeric) {
    for (UINT i = 0; i < m_meshWrappers.Size(); ++i) {
        m_materials[i].numeric = numeric;
    }
}

// Prepare arrays of the meshes and its materials
void ACGraphicsComponent::InitializeMesh(UINT meshNum, UINT texNumPerMesh) {
    // Initialize the number of meshes array
    m_meshWrappers.Resize(meshNum);

    // Allocate keys to the all meshes
    CIntKeyGenerater* meshKeyGenerater = m_gameObj->GetScene()->GetMeshKeyGenerater();
    for (UINT i = 0; i < meshNum; ++i) {
        m_meshWrappers[i].key = meshKeyGenerater->GenerateKey();
    }

    // Initialize the number of materials arrays
    m_materials = CUniquePtr<PerMeshMaterial[]>::Make(meshNum);
    if (texNumPerMesh > 0) {
        for (size_t i = 0; i < meshNum; ++i) {
            m_materials[i].texture = CUniquePtr<CUniquePtr<CTexture>[]>::Make(texNumPerMesh);
        }
    }

    // Set sent argument
    m_matNumPerMesh = texNumPerMesh;
}
