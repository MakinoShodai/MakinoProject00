#include "Model.h"
#include "ApplicationClock.h"
#include "ModelRegistry.h"
#include "SceneRegistry.h"

// Constructor
ACModel::ACModel(GraphicsComponentType type, CGameObject* owner, GraphicsLayer layer, bool isSubstance, int additionalTexID)
    : m_isSubstance(isSubstance)
    , m_additionalTexID(additionalTexID)
    , ACGraphicsComponent(type, owner, layer) {
}

// Processing for the first draw frame
void ACModel::Start() {
    ACGraphicsComponent::Start();
    if (m_isSubstance) {
        ACModel* transparent = GetTransparentComponent();
        if (transparent != nullptr) {
            transparent->ACGraphicsComponent::Start();
        }
    }
}

// Process to be called at instance destruction
void ACModel::OnDestroy() {
    ACGraphicsComponent::OnDestroy();
    if (m_isSubstance) {
        ACModel* transparent = GetTransparentComponent();
        if (transparent != nullptr) {
            transparent->ACGraphicsComponent::OnDestroy();
        }
    }
}

// Create opacity mesh data and texture data
void ACModel::CreateOpacityMesh() {
    // Get static data of model
    const CStaticModelData* staticData = GetStaticData();

    // Initialize meshes
    UINT meshNum = staticData->GetOpacityMeshNum();
    InitializeMesh(meshNum, 1);
    for (UINT i = 0; i < meshNum; ++i) {
        const ModelInfo::Mesh* mesh = staticData->GetOpacityMesh(i);
        const ModelInfo::Material* material = staticData->GetMaterial(mesh->materialIndex);

        // Set mesh buffer
        SetMeshInfo(i, mesh->meshbuffer.GetVertexWeakPtr(), mesh->meshbuffer.GetIndexWeakPtr(), mesh->meshbuffer.GetTopologyType());

        // Set standard texture
        SetTexture(i, 0, CTexture(
            Utl::Dx::ShaderString(Utl::Dx::ShaderType::Pixel, "mainTex"),
            material->GetTex(m_additionalTexID)));

        // Set numeric part of the material
        SetNumericPartMaterial(i, PerMeshMaterialNumeric(material->shininess, material->shininessScale));
    }
}

// Create transparent mesh data and texture data
void ACModel::CreateTransparentMesh() {
    // Get static data of model
    const CStaticModelData* staticData = GetStaticData();

    // Initialize meshes
    UINT meshNum = staticData->GetTransparentMeshNum();
    InitializeMesh(meshNum, 1);
    for (UINT i = 0; i < meshNum; ++i) {
        const ModelInfo::Mesh* mesh = staticData->GetTransparentMesh(i);
        const ModelInfo::Material* material = staticData->GetMaterial(mesh->materialIndex);

        // Set mesh buffer
        SetMeshInfo(i, mesh->meshbuffer.GetVertexWeakPtr(), mesh->meshbuffer.GetIndexWeakPtr(), mesh->meshbuffer.GetTopologyType());

        // Set standard texture
        SetTexture(i, 0, CTexture(
            Utl::Dx::ShaderString(Utl::Dx::ShaderType::Pixel, "mainTex"),
            material->GetTex(m_additionalTexID)));

        // Set numeric part of the material
        SetNumericPartMaterial(i, PerMeshMaterialNumeric(material->shininess, material->shininessScale));
    }
}

// Constructor
CBasicModel::CBasicModel(CGameObject* owner, GraphicsLayer layer, const std::wstring& modelPath, int additionalTexID)
    : m_aspectParam(&CModelRegistry::GetAny().GetModel(modelPath))
    , ACModel(GraphicsComponentType::BasicModel, owner, layer, true, additionalTexID) {
    // Create mesh data and texture data
    CreateOpacityMesh();
}

// Destructor
CBasicModel::~CBasicModel() {
    if (m_isSubstance) {
        m_aspectParam.substanceParam.~SubstanceParam();
    }
    else {
        m_aspectParam.transparentMeshParam.~TransparentMeshParam();
    }
}

// Constructor
CSkeletalModel::CSkeletalModel(CGameObject* owner, GraphicsLayer layer, const std::wstring& modelPath, int additionalTexID)
    : m_aspectParam(CUniquePtr<CDynamicModelController>::Make(CModelRegistry::GetAny().GetModel(modelPath)))
    , ACModel(GraphicsComponentType::SkeletalModel, owner, layer, true, additionalTexID) {
    // Create mesh data and texture data
    CreateOpacityMesh();

    // If this model has a translucent mesh, create a component for the translucent mesh
    if (GetStaticData()->GetTransparentMeshNum() > 0) {
        m_aspectParam.substanceParam.transparentMeshes = CUniquePtrWeakable<CSkeletalModel>(new CSkeletalModel(this));
        m_aspectParam.substanceParam.transparentMeshes->Awake();
    }
}

// Destructor
CSkeletalModel::~CSkeletalModel() {
    if (m_isSubstance) {
        m_aspectParam.substanceParam.~SubstanceParam();
    }
    else {
        m_aspectParam.transparentMeshParam.~TransparentMeshParam();
    }
}

// Pre drawing processing
void CSkeletalModel::PreDraw() {
    if (m_isSubstance) {
        m_aspectParam.substanceParam.controller->StepAnim(CAppClock::GetMain().GetDeltaTime());
    }
}

// Get bone matrices
CUniquePtr<DirectX::XMFLOAT4X4[]> CSkeletalModel::GetBoneMatrices(UINT meshIndex) {
    CDynamicModelController* controller = GetController();

    // Create matrices array with the same size as the number of bones
    UINT boneNum = (m_isSubstance) ? controller->GetOpacityMeshBoneNum(meshIndex) : controller->GetTransparentMeshBoneNum(meshIndex);
    CUniquePtr<DirectX::XMFLOAT4X4[]> boneMat = CUniquePtr<DirectX::XMFLOAT4X4[]>::Make(boneNum);

    // If it's not a bind pose, perform standard animation matrix calculation
    if (!controller->IsBindPose()) {
        const CStaticModelData* staticData = controller->GetStaticData();
        const ModelInfo::PerInstanceData* perInstanceData = controller->GetDynamicData();
        // For opacity meshes
        if (m_isSubstance) {
            for (UINT i = 0; i < boneNum; ++i) {
                DirectX::XMStoreFloat4x4(&boneMat[i], 
                    staticData->GetOpacityMesh(meshIndex)->bones[i].invOffset *
                    perInstanceData->nodeMatrices[staticData->GetOpacityMesh(meshIndex)->bones[i].nodeIndex]);
            }
        }
        // For transparent meshes
        else {
            for (UINT i = 0; i < boneNum; ++i) {
                DirectX::XMStoreFloat4x4(&boneMat[i],
                    staticData->GetTransparentMesh(meshIndex)->bones[i].invOffset *
                    perInstanceData->nodeMatrices[staticData->GetTransparentMesh(meshIndex)->bones[i].nodeIndex]);
            }
        }
    }
    // If it's a bind pose, set identity matrices
    else {
        for (UINT i = 0; i < boneNum; ++i) {
            DirectX::XMStoreFloat4x4(&boneMat[i], DirectX::XMMatrixIdentity());
        }
    }

    return boneMat;
}

// Constructor for transparent meshes
CSkeletalModel::CSkeletalModel(CSkeletalModel* substance)
    : m_aspectParam(substance)
    , ACModel(GraphicsComponentType::SkeletalModel, substance->GetGameObj().Get(), GraphicsLayer::Transparent, false, substance->GetAdditionalTexID())
{
    // Create mesh data and texture data
    CreateTransparentMesh();
}