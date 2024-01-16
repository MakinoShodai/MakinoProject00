/**
 * @file   DynamicCbWorldMat.h
 * @brief  This file handles the CB allocater for the world matrix.
 * 
 * @author Shodai Makino
 * @date   2023/12/6
 */

#ifndef __DYNAMIC_CB_WORLD_MAT_H__
#define __DYNAMIC_CB_WORLD_MAT_H__

#include "DynamicCbAllocator.h"

/**
   @brief Defines the screen boundary coordinate
   @details the negative value represents the left or bottom edge and the positive value represents the right or top edge of the screen
*/
constexpr float SPRITE_SCREEN_BOUNDARY = 100.0f;

/** @brief CB allocator class that handles world matrix */
class CDynamicCbWorldMat : public ACDynamicCbAllocator<DirectX::XMFLOAT4X4> {
public:
    CDynamicCbWorldMat(UINT maxUsePerFrame) : ACDynamicCbAllocator(maxUsePerFrame, "WorldMat") {}

protected:
    /** @brief Allocate data */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACGraphicsComponent* component);

    /** @brief Allocate data for sprite UI */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CSpriteUI* sprite);
    /** @brief Allocate data for sprite 3D */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CSprite3D* sprite);
    /** @brief Allocate data for billboard */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CBillboard* billboard);
    /** @brief Allocate data for debugging collider shape */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CDebugColliderShape* shape);
};

#endif // !__DYNAMIC_CB_WORLD_MAT_H__
