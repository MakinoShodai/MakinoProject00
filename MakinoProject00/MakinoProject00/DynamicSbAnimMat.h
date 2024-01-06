/**
 * @file   DynamicSbAnimMat.h
 * @brief  This file handles the SB allocater for animation matrices.
 * 
 * @author Shodai Makino
 * @date   2023/12/20
 */

#ifndef __DYNAMIC_SB_ANIM_MAT_H__
#define __DYNAMIC_SB_ANIM_MAT_H__

#include "DynamicSbAllocator.h"

/** @brief Structured buffer allocator class that handles animation matrices */
class CDynamicSbAnimMat : public ACDynamicSbAllocatorPerMesh<DirectX::XMFLOAT4X4> {
public:
    /** @brief Constructor */
    CDynamicSbAnimMat() : ACDynamicSbAllocatorPerMesh("animMatrices") {}

protected:
    /** @brief Allocate data for graphics component */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACGraphicsComponent* component, UINT meshIndex) override;
    /** @brief Allocate data for model */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CSkeletalModel* model, UINT meshIndex) override;

    /** @brief Create buffer for graphics component */
    void CreateBuffer(ACGraphicsComponent* component) override;
    /** @brief Create buffer for model */
    void CreateBuffer(CSkeletalModel* model) override;
};

#endif // !__DYNAMIC_SB_ANIM_MAT_H__
