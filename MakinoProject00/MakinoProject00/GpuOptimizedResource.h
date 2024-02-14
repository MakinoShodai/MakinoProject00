/**
 * @file   GpuOptimizedBuffer.h
 * @brief  This file handles resource optimized for GPU.
 * 
 * @author Shodai Makino
 * @date   2023/11/14
 */

#ifndef __GPU_OPTIMIZED_RESOURCE_H__
#define __GPU_OPTIMIZED_RESOURCE_H__

/** @brief This class handles resource optimized for GPU */
class ACGpuOptimizedResource {
protected:
    /**
       @brief Mapping function type 
       @param UINT8* Mapped buffer
    */
    using MappingFunction = std::function<void(UINT8*)>;

public:
    /**
       @brief Constructor
    */
    ACGpuOptimizedResource() = default;

    /**
       @brief Destructor
    */
    ~ACGpuOptimizedResource() = default;

    /** @brief Get texture resource */
    ID3D12Resource* GetResource() { return m_resource.Get(); }

protected:
    /**
       @brief Create resource
       @param byteSize Bytes of all data
       @param resourceState After resource state
       @param mapFunc Mapping data function
    */
    void CreateResource(UINT64 byteSize, D3D12_RESOURCE_STATES resourceState, MappingFunction mapFunc);

    /**
       @brief Update heap properties and resource descriptor, for resource
       @param heapProp Heap properties used for resource creation
       @param resDesc Resource descriptor used for resource creation
       @details
       The heap properties and the resource descriptor sent are simple buffer ones
    */
    virtual void UpdateDataForResource(D3D12_HEAP_PROPERTIES* heapProp, D3D12_RESOURCE_DESC* resDesc) {}

    /**
       @brief Set command to copy data
       @param cmdList Command set to
       @param srcResource Copy source
    */
    virtual void CopyRegion(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* srcResource) = 0;

protected:
    /** @brief Texture resource */
    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
};

#endif // !__GPU_OPTIMIZED_RESOURCE_H__
