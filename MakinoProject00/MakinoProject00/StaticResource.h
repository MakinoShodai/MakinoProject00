/**
 * @file   StaticResource.h
 * @brief  This file handles interface class of static resources that don't need to be updated on a per-object basis.
 * 
 * @author Shodai Makino
 * @date   2023/11/22
 */

#ifndef __STATIC_RESOURCE_H__
#define __STATIC_RESOURCE_H__

#include "Singleton.h"
#include "UtilityForDirectX.h"
#include "SwapChain.h"

// Forward declaration
class ACScene;

/** @brief interface for static resource that do not need to be updated on a per-object basis */
class IStaticResource {
public:
    /**
       @brief Destructor
    */
    virtual ~IStaticResource() = default;

    /**
       @brief Get handle of allocated data in resource
       @return Resource CPU handle
    */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle() = 0;

    /**
       @brief Refresh process that must be called at the end of every frame
    */
    virtual void Refresh() = 0;

    /**
       @brief Processing at scene start
    */
    virtual void Start(ACScene* scene) {}

    /**
       @brief Update process in scene
    */
    virtual void Update(ACScene* scene) {}

    /** @brief Get the name of resource defined in hlsl */
    virtual const std::string& GetName() = 0;

protected:
    /**
       @brief Allocate and return data
       @return Allocated data
    */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData() = 0;
};

#endif // !__STATIC_RESOURCE_H__
