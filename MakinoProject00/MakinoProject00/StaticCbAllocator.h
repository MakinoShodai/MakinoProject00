/**
 * @file   StaticCbAllocator.h
 * @brief  This file handles abstract class of static constant buffer that don't need to be updated on a per-object basis.
 * 
 * @author Shodai Makino
 * @date   2023/11/22
 */

#ifndef __STATIC_CB_ALLOCATOR_H__
#define __STATIC_CB_ALLOCATOR_H__

#include "StaticResource.h"
#include "AbstractCbAllocator.h"

/**
   @brief This class allocates data for constant buffer view that don't need to be updated on a per-object basis
   @tparam ConstBuffType type of constant buffer view
*/
template<class ConstBuffType>
class ACStaticCbAllocator : public IStaticResource, public ACCbAllocator<ConstBuffType> {
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

    /** @brief Get the name of cbuffer defined in hlsl */
    const std::string& GetName() override final { return ACCbAllocator<ConstBuffType>::m_hlslBufferName; }

protected:
    /**
       @brief Constcurtor
       @param maxUsePerFrame The name of cbuffer defined in hlsl
       @param hlslBufferName The maximum number of buffers that can be used in one frame
    */
    ACStaticCbAllocator(std::string hlslBufferName, UINT maxUsePerFrame = 1)
        : ACCbAllocator<ConstBuffType>(maxUsePerFrame, std::move(hlslBufferName))
        , m_isOneAllocated(false)
    {}

    /**
       @brief Destructor
    */
    virtual ~ACStaticCbAllocator() = default;

    /**
       @brief Advance to the next data assignment?
       @return Result
    */
    virtual bool IsAdvanceNextAllocate() { return false; }

    /**
       @brief Refresh process that must be called at the end of every frame in derived class
    */
    virtual void RefreshDerived() {}

private:
    /** @brief Already allocated one data? */
    bool m_isOneAllocated;
};

// Get handle of allocated data in constant buffer
template<class ConstBuffType>
Utl::Dx::CPU_DESCRIPTOR_HANDLE ACStaticCbAllocator<ConstBuffType>::GetAllocatedDataHandle() {
    // If you haven't already allocated data
    if (IsAdvanceNextAllocate() || false == m_isOneAllocated) {
        m_isOneAllocated = true;

        // Allodate and return data
        return AllocateData();
    }

    // Return previously allocated data
    return ACCbAllocator<ConstBuffType>::GetPrevAllocatedData();
}

// Refresh process that must be called at the end of every frame
template<class ConstBuffType>
void ACStaticCbAllocator<ConstBuffType>::Refresh() {
    // Refreshing process in derived class
    RefreshDerived();

    // Refresh ACCbAllocator
    ACCbAllocator<ConstBuffType>::RefreshCbAllocator();

    // Lower the flag
    m_isOneAllocated = false;
}

#endif // !__STATIC_CB_ALLOCATOR_H__
