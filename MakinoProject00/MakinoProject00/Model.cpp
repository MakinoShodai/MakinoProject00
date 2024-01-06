#include "Model.h"
#include "ApplicationClock.h"
#include "ModelRegistry.h"

// Create mesh data and texture data
void ACModel::CreateMesh() {
    // Get static data of model
    const CStaticModelData* staticData = GetStaticData();

    // Initialize meshes
    UINT meshNum = staticData->GetMeshNum();
    InitializeMesh(meshNum, 1);
    for (UINT i = 0; i < meshNum; ++i) {
        const ModelInfo::Mesh* mesh = staticData->GetMesh(i);

        // Set mesh buffer
        SetMeshInfo(i, mesh->meshbuffer.GetVertexWeakPtr(), mesh->meshbuffer.GetIndexWeakPtr(), mesh->meshbuffer.GetTopologyType());

        // Set standard texture
        SetTexture(i, 0, CTexture(
            Utl::Dx::ShaderString(Utl::Dx::ShaderType::Pixel, "mainTex"),
            *staticData->GetMaterial(mesh->materialIndex)->texture.Get()));
    }
}

// Constructor
CBasicModel::CBasicModel(CGameObject* owner, GraphicsLayer layer, const std::wstring& modelPath) 
    : m_model(&CModelRegistry::GetAny().GetModel(modelPath))
    , ACModel(GraphicsComponentType::BasicModel, owner, layer) {
    // Create mesh data and texture data
    CreateMesh();
}

// Constructor
CSkeletalModel::CSkeletalModel(CGameObject* owner, GraphicsLayer layer, const std::wstring& modelPath)
    : m_model(CModelRegistry::GetAny().GetModel(modelPath))
    , ACModel(GraphicsComponentType::SkeletalModel, owner, layer) {
    // Create mesh data and texture data
    CreateMesh();
}

// Pre drawing processing
void CSkeletalModel::PreDraw() {
    m_model.StepAnim((float)CAppClock::GetMain().GetDeltaTime());
}
