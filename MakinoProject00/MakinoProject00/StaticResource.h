/**
 * @file   StaticResource.h
 * @brief  This file handles abstract class of static resources that don't need to be updated on a per-object basis.
 * 
 * @author Shodai Makino
 * @date   2023/11/22
 */

#ifndef __STATIC_RESOURCE_H__
#define __STATIC_RESOURCE_H__

#include "Singleton.h"
#include "UtilityForDirectX.h"
#include "SwapChain.h"

// Forward declarations
class ACScene;
class CStaticResourceRegistry;

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

/** @brief Abstract class of static resource that do not need to be updated on a per-object basis */
template <class Derived>
class ACStaticResource : public IStaticResource, public ACSingletonBase<Derived> {
public:
    /**
       @brief Get handle of allocated data in resource
       @return Resource CPU handle
    */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle() override final;

    /**
       @brief Refresh process that must be called at the end of every frame
    */
    void Refresh() override final;

    /** @brief Get the name of resource defined in hlsl */
    const std::string& GetName() override { return m_hlslResouceName; }

protected:
    /**
       @brief Constructor
       @param hlslResourceName The name of resouce defined in hlsl
    */
    ACStaticResource(const std::string& hlslResourceName) : ACSingletonBase<Derived>(-5), m_hlslResouceName(hlslResourceName), m_isAllocated(false), m_currentOffset(0){}

    /**
       @brief Destructor
    */
    virtual ~ACStaticResource() = default;

    /**
       @brief Returns a handle to a resource that has already been allocated
       @return Resource CPU handle
    */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE GetResourceHandle() = 0;

    /**
       @brief Allocate new data
       @return Resource CPU handle
    */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData() = 0;

    /**
       @brief Get resource CPU handle to use the previous data as it is
       @return Resource CPU handle
    */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE GetCPUHandle() = 0;

    /**
       @brief Refresh process that must be called at the end of every frame, for derived class
    */
    virtual void RefreshDerived() {}

    /** @brief Get current offset in buffer */
    UINT GetCurrentOffset() { return m_currentOffset; }

private:
    /** @brief The name of resouce defined in hlsl */
    const std::string m_hlslResouceName;
    /** @brief Is the data already allocated? */
    bool m_isAllocated;
    /** @brief Current offset in buffer */
    UINT m_currentOffset;
};

// Refresh process that must be called at the end of every frame
template<class Derived>
void ACStaticResource<Derived>::Refresh() {
    // Reset flag
    m_isAllocated = false;

    // Refresh process by derived class
    RefreshDerived();

    // Change current offset
    m_currentOffset = (m_currentOffset + 1) % SCREEN_BUFFERING_NUM;
}

// Get handle of allocated data in resource
template<class Derived>
Utl::Dx::CPU_DESCRIPTOR_HANDLE ACStaticResource<Derived>::GetAllocatedDataHandle() {
    // If you have already assigned data, return it.
    if (m_isAllocated) {
        return GetResourceHandle();
    }

    // Allocate new data
    m_isAllocated = true;
    return AllocateData();
}


#endif // !__STATIC_RESOURCE_H__
