/**
 * @file   AbstractCbAllocator.h
 * @brief  This file allocates data for Constant buffer view.
 * 
 * @author Shodai Makino
 * @date   2023/11/15
 */

#ifndef __ABSTRACT_CB_ALLOCATOR_H__
#define __ABSTRACT_CB_ALLOCATOR_H__

#include "ConstantBufferResource.h"
#include "DescriptorHeap.h"
#include "Application.h"
#include "SwapChain.h"
#include "UniquePtr.h"

/**
   @brief This class allocates data for constant buffer
   @tparam ConstBuffType type of constant buffe
   @details
   The following must be done in the derived class.
   Only 'UINT maxUsePerFrame' should be the argument of the constructor of the derived class
*/
template<class ConstBuffType>
class ACCbAllocator {
public:
    /** @brief  Disallow the copy constructor */
    ACCbAllocator(const ACCbAllocator&) = delete;
    /** @brief Disallow the copy assignment operator */
    ACCbAllocator& operator=(const ACCbAllocator&) = delete;
    /** @brief Disallow the move constructor */
    ACCbAllocator(ACCbAllocator&&) = delete;
    /** @brief Disallow the move assignment constructor */
    ACCbAllocator& operator=(ACCbAllocator&&) = delete;

    /**
       @brief Constructor
       @param maxUsePerFrame The maximum number of buffers that can be used in one frame
       @param hlslBufferName The name of cbuffer defined in hlsl
    */
    ACCbAllocator(UINT maxUsePerFrame, std::string hlslBufferName)
        : m_hlslBufferName(std::move(hlslBufferName)), m_numPerElement(maxUsePerFrame), m_currentOffset(0), m_currentDynamicArrayIndex(0) {
        for (UINT i = 0; i < SCREEN_BUFFERING_NUM; ++i) { AddElement(i); }
    }

    /**
       @brief Destructor
    */
    virtual ~ACCbAllocator();

protected:
    /**
       @brief Copy data directly
       @param src Data to be copy
       @return CPU handle for data copied
       @note The Map operation is called within the function. However, the Unmap operation is not called
    */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE DirectDataCopy(const ConstBuffType* src);

    /**
       @brief Refresh process that must be called at the end of every frame
    */
    void RefreshCbAllocator();

    /**
       @brief Get handle for one previous allocated data
       @return Handle for one previous allocated data
    */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE GetPrevAllocatedData();

private:
    /**
       @brief Add elements of constant buffer and descriptor heap
       @param index Index to add element
    */
    void AddElement(UINT index);

protected:
    /** @brief The name of cbuffer defined in hlsl */
    const std::string m_hlslBufferName;

private:
    /** @brief Constant buffers */
    std::vector<CConstantBufferResource<ConstBuffType>> m_constantBuffer[SCREEN_BUFFERING_NUM];
    /** @brief Descriptor heaps with constant buffer views */
    std::vector<CDescriptorHeap> m_descriptorHeap[SCREEN_BUFFERING_NUM];
    /** @brief The number of buffers per element */
    const UINT m_numPerElement;
    /** @brief Current offset in buffer */
    UINT m_currentOffset;
    /** @brief Current index in the dynamic arrays of the constant buffer and the descriptor heap */
    UINT m_currentDynamicArrayIndex;
};

// Process to be called at instance destruction
template<class ConstBuffType>
ACCbAllocator<ConstBuffType>::~ACCbAllocator() {
    // #TODO : Log processing when the maximum number of elements is not appropriate
    UINT size = (UINT)m_constantBuffer[0].size();
    if (size > 1) {

    }
}

// Copy data directly
template<class ConstBuffType>
Utl::Dx::CPU_DESCRIPTOR_HANDLE ACCbAllocator<ConstBuffType>::DirectDataCopy(const ConstBuffType* src) {
    // Get next back buffer index
    UINT bufferIndex = CSwapChain::GetMain().GetNextBackBufferIndex();

    // If the current descriptor heap limit is exceeded, advance the index in dynamic array
    if (m_currentOffset >= m_numPerElement) {
        m_currentOffset = 0;
        m_currentDynamicArrayIndex++;

        // Check for overflow of constant buffer size
        if (m_currentDynamicArrayIndex >= m_constantBuffer[bufferIndex].size()) {
            AddElement(bufferIndex);
        }
    }

    // Perform map operation
    m_constantBuffer[bufferIndex][m_currentDynamicArrayIndex].Map();

    // Update data of the constant buffer
    m_constantBuffer[bufferIndex][m_currentDynamicArrayIndex].DataCopy(src, m_currentOffset);

    // Get specified CPU handle from descriptor heap and increment a current offset
    return m_descriptorHeap[bufferIndex][m_currentDynamicArrayIndex].GetCpuHandle(m_currentOffset++);
}

// Refresh process that must be called at the end of every frame
template<class ConstBuffType>
void ACCbAllocator<ConstBuffType>::RefreshCbAllocator() {
    // Unmap
    for (UINT i = 0; i <= m_currentDynamicArrayIndex; ++i) {
        m_constantBuffer[CSwapChain::GetMain().GetNextBackBufferIndex()][i].Unmap();
    }

    // ClearData
    m_currentOffset = 0;
    m_currentDynamicArrayIndex = 0;
}

// Get handle for one previous allocated data
template<class ConstBuffType>
Utl::Dx::CPU_DESCRIPTOR_HANDLE ACCbAllocator<ConstBuffType>::GetPrevAllocatedData() {
    // Get next back buffer index
    UINT bufferIndex = CSwapChain::GetMain().GetNextBackBufferIndex();

    // If the index in the dynamic array has just been updated, retrieve using the previous index
    if (m_currentOffset == 0) {
        throw Utl::Error::CStopDrawingSceneError(L"Warning! Trying to get previous data in constant buffer without allocating data.\n");
    }

    // Get previous data
    return m_descriptorHeap[bufferIndex][m_currentDynamicArrayIndex].GetCpuHandle(m_currentOffset - 1);
}

// Add elements of constant buffer and descriptor heap
template<class ConstBuffType>
void ACCbAllocator<ConstBuffType>::AddElement(UINT index) {
    // Add constant buffer
    m_constantBuffer[index].emplace_back(m_numPerElement);
    // Add Descriptor heap
    m_descriptorHeap[index].emplace_back(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, m_numPerElement);

    // Get trailing variable
    CConstantBufferResource<ConstBuffType>& cbuf = m_constantBuffer[index].back();
    CDescriptorHeap& descriptor = m_descriptorHeap[index].back();

    // Create all constant buffer view
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.SizeInBytes = cbuf.GetSize();
    for (UINT i = 0; i < m_numPerElement; ++i) {
        // Get GPU address of constant buffer
        cbvDesc.BufferLocation = cbuf.GetGPUVirtualAddress(i);

        // Create constant buffer view
        CApplication::GetAny().GetDXDevice()->CreateConstantBufferView(&cbvDesc, descriptor.GetCpuHandle(i));
    }
}

#endif // !__ABSTRACT_CB_ALLOCATOR_H__
