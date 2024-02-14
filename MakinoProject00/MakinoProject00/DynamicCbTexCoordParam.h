/**
 * @file   DynamicCbTexCoordParam.h
 * @brief  This file handles the CB allocater for the texture coordinate parameter.
 * 
 * @author Shodai Makino
 * @date   2024/2/3
 */

#ifndef __DYNAMIC_CB_TEX_COORD_PARAM_H__
#define __DYNAMIC_CB_TEX_COORD_PARAM_H__

#include "DynamicCbAllocator.h"

 /** @brief CB allocator class that handles color */
class CDynamicCbTexCoordParam : public ACDynamicCbAllocator<Vector4f> {
public:
    CDynamicCbTexCoordParam(UINT maxUsePerFrame) : ACDynamicCbAllocator(maxUsePerFrame, "TexCoordParam") {}

    /**
       @brief Allocate data for no component
       @param tiling Tiling
       @param offset Offset
       @return Handle for CPU
    */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateDataNoComponent(const Vector2f& tiling = Vector2f::Ones(), const Vector2f& offset = Vector2f::Zero());

protected:
    /** @brief Allocate data */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACGraphicsComponent* component);
};


#endif // !__DYNAMIC_CB_TEX_COORD_PARAM_H__
