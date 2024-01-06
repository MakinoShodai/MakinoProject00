#include "DynamicSbAnimMat.h"
#include "UtilityForException.h"

// Allocate data for graphcis component
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicSbAnimMat::AllocateData(ACGraphicsComponent* component, UINT meshIndex) {
    throw Utl::Error::CFatalError(L"Non model classes can't use animation matrices");
}

// Allocate data for model
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicSbAnimMat::AllocateData(CSkeletalModel* model, UINT meshIndex) {
    // Get model controller
    CDynamicModelController* controller = model->GetController();

    // Create matrices array with the same size as the number of bones
    UINT boneNum = controller->GetMeshBoneNum(meshIndex);
    CUniquePtr<DirectX::XMFLOAT4X4[]> boneMat = CUniquePtr<DirectX::XMFLOAT4X4[]>::Make(boneNum);

    // If it's not a bind pose, perform standard animation matrix calculation
    if (!controller->CheckAnimMode(ModelInfo::AnimMode::BindPose)) {
        for (UINT i = 0; i < boneNum; ++i) {
            controller->CalculateAnimationMatrix(&boneMat[i], meshIndex, i);
        }
    }
    // If it's a bind pose, set identity matrices
    else {
        for (UINT i = 0; i < boneNum; ++i) {
            DirectX::XMStoreFloat4x4(&boneMat[i], DirectX::XMMatrixIdentity());
        }
    }
    
    // Data copy
    return DirectDataCopy(model->GetMeshKey(meshIndex), boneMat.Get());
}

// Create buffer for graphics component
void CDynamicSbAnimMat::CreateBuffer(ACGraphicsComponent* component) {
    throw Utl::Error::CFatalError(L"Non model classes can't use animation matrices");
}

// Create buffer for model
void CDynamicSbAnimMat::CreateBuffer(CSkeletalModel* model) {
    CDynamicModelController* controller = model->GetController();

    // Calculate all bone num
    UINT meshNum = model->GetMeshNum();
    UINT bufferSize = 0;
    for (UINT i = 0; i < meshNum; ++i) {
        bufferSize += controller->GetMeshBoneNum(i);
    }
    if (bufferSize <= 0) {
        OutputDebugString(L"Warning! The model that is supposed to use the animation matrices doesn't have bones\n");
        return;
    }

    // Create a per graphics component structured buffer
    CreatePerComponentBuffer(bufferSize, meshNum);

    // Create per mesh data
    UINT currentOffset = 0;
    for (UINT i = 0; i < meshNum; ++i) {
        UINT boneNum = controller->GetMeshBoneNum(i);
        CreatePerMeshData(i, model->GetMeshKey(i), boneNum, currentOffset);
        currentOffset += boneNum;
    }
}
