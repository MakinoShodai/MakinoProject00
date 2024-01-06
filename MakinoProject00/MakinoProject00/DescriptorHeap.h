/**
 * @file   DescriptorHeap.h
 * @brief  This file handles descriptor heap.
 * 
 * @author Shodai Makino
 * @date   2023/11/15
 */

#ifndef __DESCRIPTOR_HEAP_H__
#define __DESCRIPTOR_HEAP_H__

#include "UtilityForDirectX.h"
#include "Application.h"

/** @brief This class handles descriptor heap */
class CDescriptorHeap {
public:
    /**
       @brief Constructor
    */
    CDescriptorHeap() : m_heapType(D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES), m_cpuHandle(0), m_gpuHandle(0) {}

    /**
       @brief Create descriptor heap
       @param type Heap type
       @param flags Heap flags
       @param numDescriptors The number of descriptors
    */
    CDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT numDescriptors) 
        : m_heapType(type), m_cpuHandle(0), m_gpuHandle(0) {
        Create(type, flags, numDescriptors); 
    }

    /**
       @brief Destructor
    */
    ~CDescriptorHeap() = default;

    /**
       @brief Create descriptor heap
       @param type Heap type
       @param flags Heap flags
       @param numDescriptors The number of descriptors
    */
    void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT numDescriptors);

    /**
       @brief Get CPU handle for descriptor heap
       @param offset Offset of the heap address you want to use
       @return Offset CPU handle for descriptor heap
       @attention
       The size corresponding to heap type is multiplied to the offset, so do not multiply it beforehand
    */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetCpuHandle(size_t offset = 0) const {
        return m_cpuHandle + offset * CApplication::GetAny().GetHeapTypeSize(m_heapType);
    }

    /**
       @brief Get CPU handle for descriptor heap
       @param offset Offset of the heap address you want to use
       @return Offset CPU handle for descriptor heap
       @attention
       The size corresponding to heap type is multiplied to the offset, so do not multiply it beforehand
    */
    inline Utl::Dx::GPU_DESCRIPTOR_HANDLE GetGpuHandle(UINT64 offset = 0) const {
        return m_gpuHandle + offset * CApplication::GetAny().GetHeapTypeSize(m_heapType);
    }

    /** @brief Get Descriptor heap */
    ID3D12DescriptorHeap* GetHeap() { return m_descriptorHeap.Get(); }
    /** @brief Get Descriptor heap address */
    ID3D12DescriptorHeap** GetHeapAddress() { return m_descriptorHeap.GetAddressOf(); }

private:
    /** @brief Descriptor heap object */
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
    /** @brief Heap type */
    D3D12_DESCRIPTOR_HEAP_TYPE m_heapType;
    /** @brief CPU handle for descriptor heap */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE m_cpuHandle;
    /** @brief GPU handle for descriptor heap */
    Utl::Dx::GPU_DESCRIPTOR_HANDLE m_gpuHandle;
};

#endif // !__DESCRIPTOR_HEAP_H__
