/**
 * @file   DynamicCbMaterial.h
 * @brief  This file handles dynamic CB allocator class for material.
 * 
 * @author Shodai Makino
 * @date   2024/1/24
 */

#ifndef __DYNAMIC_CB_MATERIAL_H__
#define __DYNAMIC_CB_MATERIAL_H__

#include "DynamicCbAllocator.h"

/** @brief Material in hlsl */
struct MaterialInHlsl {
    /** @brief Color */
    Colorf color;
    /** @brief Specular shininess property */
    Vector2f shininessPeroperty;
};

 /** @brief CB allocator class for material */
class CDynamicCbMaterial : public ACDynamicCbAllocatorPerMesh<MaterialInHlsl> {
public:
    CDynamicCbMaterial(UINT maxUsePerFrame) : ACDynamicCbAllocatorPerMesh(maxUsePerFrame, "Material") {}

protected:
    /** @brief Allocate data */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACGraphicsComponent* component, UINT meshIndex);
};

#endif // !__DYNAMIC_CB_MATERIAL_H__
