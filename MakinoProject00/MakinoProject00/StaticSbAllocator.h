/**
 * @file   StaticSbAllocator.h
 * @brief  This file handles abstract class for static structured buffer allocator.
 * 
 * @author Shodai Makino
 * @date   2024/1/24
 */

#ifndef __STATIC_SB_ALLOCATOR_H__
#define __STATIC_SB_ALLOCATOR_H__

#include "StaticResource.h"
#include "StructuredBufferResource.h"
#include "UniquePtr.h"
#include "DescriptorHeapPool.h"

 /**
    @brief This class allocates data for structured buffer view that don't need to be updated on a per-object basis
    @tparam StructBuffType type of structured buffer view
 */
template<class StructBuffType>
class ACStaticSbAllocator : public IStaticResource {
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

    /**
       @brief Copy data directly
       @param elementsNum The number of elements
       @param src Data to be copy
       @return CPU handle for data copied
       @note The Map operation is called within the function. However, the Unmap operation is not called
    */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE DirectDataCopy(UINT elementsNum, const StructBuffType* src);

    /** @brief Get the name of cbuffer defined in hlsl */
    const std::string& GetName() override final { return m_hlslBufferName; }

protected:
    /**
       @brief Constcurtor
       @param hlslBufferName The maximum number of buffers that can be used in one frame
    */
    ACStaticSbAllocator(std::string hlslBufferName)
        : m_hlslBufferName(std::move(hlslBufferName))
        , m_isOneAllocated(false)
    {
        std::fill(std::begin(m_currentElementsNum), std::end(m_currentElementsNum), 0);
        m_descriptorHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, SCREEN_BUFFERING_NUM);
        for (UINT i = 0; i < SCREEN_BUFFERING_NUM; ++i) {
            RecreateStructuredBuffer(i, 1);
        }
    }

    /**
       @brief Destructor
    */
    virtual ~ACStaticSbAllocator() = default;

    /**
       @brief Refresh process that must be called at the end of every frame in derived class
    */
    virtual void RefreshDerived() {}

    /**
       @brief Recreate structured buffer
       @param bufferIndex Index of structured buffer
       @param elementsNum The number of elements
    */
    void RecreateStructuredBuffer(UINT bufferIndex, UINT elementsNum);

private:
    /** @brief The name of cbuffer defined in hlsl */
    const std::string m_hlslBufferName;
    /** @brief Structured buffers equal to the number of screen buffering */
    CUniquePtr<CStructuredBufferResource<StructBuffType>> m_structuredBuffers[SCREEN_BUFFERING_NUM];
    /** @brief Descriptor heap */
    CDescriptorHeap m_descriptorHeap;
    /** @brief Current total number of the elements */
    UINT m_currentElementsNum[SCREEN_BUFFERING_NUM];
    /** @brief Already allocated one data? */
    bool m_isOneAllocated;
};

// Get handle of allocated data in structured buffer
template<class StructBuffType>
Utl::Dx::CPU_DESCRIPTOR_HANDLE ACStaticSbAllocator<StructBuffType>::GetAllocatedDataHandle() {
    // If you haven't already allocated data
    if (false == m_isOneAllocated) {
        m_isOneAllocated = true;

        // Allodate and return data
        return AllocateData();
    }

    // Return previously allocated data
    return m_descriptorHeap.GetCpuHandle(CSwapChain::GetMain().GetNextBackBufferIndex());
}

// Refresh process that must be called at the end of every frame
template<class StructBuffType>
void ACStaticSbAllocator<StructBuffType>::Refresh() {
    // Refreshing process in derived class
    RefreshDerived();

    // Lower the flag
    m_isOneAllocated = false;
}

// Copy data directly
template<class StructBuffType>
Utl::Dx::CPU_DESCRIPTOR_HANDLE ACStaticSbAllocator<StructBuffType>::DirectDataCopy(UINT elementsNum, const StructBuffType* src) {
    // Get next back buffer index
    UINT bufferIndex = CSwapChain::GetMain().GetNextBackBufferIndex();

    if (elementsNum == 0) {
        return m_descriptorHeap.GetCpuHandle(bufferIndex);
    }

    if (elementsNum != m_currentElementsNum[bufferIndex]) {
        // Recreate structrued buffer
        RecreateStructuredBuffer(bufferIndex, elementsNum);
    }

    // Perform map operation
    m_structuredBuffers[bufferIndex]->Map();

    // Update data of the structured buffer
    m_structuredBuffers[bufferIndex]->DataCopy(src, elementsNum, 0);

    // Get specified CPU handle from descriptor heap
    return m_descriptorHeap.GetCpuHandle(bufferIndex);
}

// Recreate structured buffer
template<class StructBuffType>
void ACStaticSbAllocator<StructBuffType>::RecreateStructuredBuffer(UINT bufferIndex, UINT elementsNum) {
    // Recreate structured buffer
    m_structuredBuffers[bufferIndex].Reset(new CStructuredBufferResource<StructBuffType>());
    m_structuredBuffers[bufferIndex]->Create(elementsNum);

    // Set the number of elements
    m_currentElementsNum[bufferIndex] = elementsNum;

    // Create shader resource view desc
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = elementsNum;
    srvDesc.Buffer.StructureByteStride = (UINT)sizeof(StructBuffType);
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

    // Recreate shader resource view
    CApplication::GetAny().GetDXDevice()->CreateShaderResourceView(
        m_structuredBuffers[bufferIndex]->GetResource(), &srvDesc,
        m_descriptorHeap.GetCpuHandle(bufferIndex)
    );
}

#endif // !__STATIC_SB_ALLOCATOR_H__
