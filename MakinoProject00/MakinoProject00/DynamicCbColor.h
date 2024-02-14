/**
 * @file   DynamicCbColor.h
 * @brief  This file handles the CB allocater for the color parameter.
 *
 * @author Shodai Makino
 * @date   2023/12/6
 */

#ifndef __DYNAMIC_CB_COLOR_H__
#define __DYNAMIC_CB_COLOR_H__

#include "DynamicCbAllocator.h"

/** @brief CB allocator class that handles color */
class CDynamicCbColor : public ACDynamicCbAllocator<Colorf> {
public:
    CDynamicCbColor(UINT maxUsePerFrame) : ACDynamicCbAllocator(maxUsePerFrame, "Color") {}

    /**
       @brief Allocate data for no component
       @param color Color
       @return Handle for CPU
    */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateDataNoComponent(const Colorf& color);

protected:
    /** @brief Allocate data */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACGraphicsComponent* component);
};

#endif // !__DYNAMIC_CB_COLOR_H__
