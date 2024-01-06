/**
 * @file   DescriptorHeapPool.h
 * @brief  This file handles pool class that manages descriptor heap for graphics pipeline state.
 * 
 * @author Shodai Makino
 * @date   2023/12/12
 */

#ifndef __DESCRIPTOR_HEAP_POOL_H__
#define __DESCRIPTOR_HEAP_POOL_H__

#include "Singleton.h"
#include "DescriptorHeap.h"
#include "SwapChain.h"

// Forward declaration
class CGraphicsPipelineState;

/** @brief Pool class that manages descriptor heap for graphics pipeline state */
class CDescriptorHeapPool : public ACMainThreadSingleton<CDescriptorHeapPool> {
    // Friend declarations
    friend class ACSingletonBase;
    friend class CGraphicsPipelineState;

public:
    /**
       @brief Destructor
    */
    ~CDescriptorHeapPool() = default;

    /**
       @brief Initialize
       @param numPerHeap The number of resources per descriptor heap
       @param initNum Initial number of descriptor heaps
    */
    void Initialize(UINT numPerHeap, UINT initNum = 1);

    /**
       @brief Drawing preparation processing necessary every frame
    */
    void ReadyToDraw();

    /**
       @brief Get CPU handle for descriptor heap
       @param offset Offset after the currently used region in a descriptor heap
       @return CPU handle
    */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT offset) { 
        return m_descriptorHeap[CSwapChain::GetMain().GetNextBackBufferIndex()][m_currentArrayIndex].GetCpuHandle(
            m_currentUsedRegion + offset);
    }

    /**
       @brief Get GPU handle for descriptor heap
       @param offset Offset after the currently used region in a descriptor heap
       @return GPU handle
    */
    Utl::Dx::GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT offset) {
        return m_descriptorHeap[CSwapChain::GetMain().GetNextBackBufferIndex()][m_currentArrayIndex].GetGpuHandle(
            m_currentUsedRegion + offset);
    }

protected:
    /**
       @brief Constructor
    */
    CDescriptorHeapPool() : ACMainThreadSingleton(0), m_numPerDescriptorHeap(0), m_currentArrayIndex(0), m_currentUsedRegion(0) {}

private:
    /**
       @brief Check if there is available capacity remaining in the descriptor heap
       @param num Number to be used
       @return If there is enough available capacity, return true
    */
    bool CheckAvailableCapacity(UINT num) { return (m_currentUsedRegion + num <= m_numPerDescriptorHeap); }

    /**
       @brief Determine the use of the region in a descriptor heap
       @param usedNum Number of regions used
    */
    void DetermineUseRegion(UINT usedNum) { m_currentUsedRegion += usedNum; }

    /**
       @brief Advance to the next descriptor heap
    */
    void AdvanceNextDescriptorHeap();

private:
    /** @brief Descriptor heap objects array */
    std::vector<CDescriptorHeap> m_descriptorHeap[SCREEN_BUFFERING_NUM];
    /** @brief The number of resources per descriptor heap */
    UINT m_numPerDescriptorHeap;
    /** @brief The current index of descriptor heaps array */
    UINT m_currentArrayIndex;
    /** @brief Currently used region of a descriptor heap */
    UINT m_currentUsedRegion;
};

#endif // !__DESCRIPTOR_HEAP_POOL_H__
