#include "DynamicSbAnimMat.h"
#include "UtilityForException.h"

// Allocate data for graphcis component
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicSbAnimMat::AllocateData(ACGraphicsComponent* component, UINT meshIndex) {
    throw Utl::Error::CFatalError(L"Non model classes can't use animation matrices");
}

// Allocate data for model
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicSbAnimMat::AllocateData(CSkeletalModel* model, UINT meshIndex) {
    // Data copy
    return DirectDataCopy(model->GetMeshKey(meshIndex), model->GetBoneMatrices(meshIndex).Get());
}

// Create buffer for graphics component
void CDynamicSbAnimMat::CreateBuffer(ACGraphicsComponent* component) {
    throw Utl::Error::CFatalError(L"Non model classes can't use animation matrices");
}

// Create buffer for model
void CDynamicSbAnimMat::CreateBuffer(CSkeletalModel* model) {
    // Calculate all bone num
    UINT meshNum = model->GetMeshNum();
    UINT bufferSize = 0;
    for (UINT i = 0; i < meshNum; ++i) {
        bufferSize += model->GetMeshBoneNum(i);
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
        UINT boneNum = model->GetMeshBoneNum(i);
        CreatePerMeshData(i, model->GetMeshKey(i), boneNum, currentOffset);
        currentOffset += boneNum;
    }
}
