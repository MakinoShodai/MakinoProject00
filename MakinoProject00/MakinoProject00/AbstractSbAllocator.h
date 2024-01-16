/**
 * @file   AbstractSbAllocator.h
 * @brief  This file handles abstract class that manages structured buffer allocater.
 * 
 * @author Shodai Makino
 * @date   2023/12/16
 */

#ifndef __ABSTRACT_SB_ALLOCATOR_H__
#define __ABSTRACT_SB_ALLOCATOR_H__

#include "StructuredBufferResource.h"
#include "DescriptorHeap.h"
#include "SwapChain.h"
#include "StableHandleVector.h"
#include "Application.h"

/** @brief Per graphics component structured buffer data */
template <class StructBuffType>
struct SbDataPerComponent {
    /** @brief Structured buffers equal to the number of screen buffering */
    CStructuredBufferResource<StructBuffType> structuredBuffers[SCREEN_BUFFERING_NUM];
    /** @brief Descriptor heap equal to the number of screen buffering */
    CDescriptorHeap descriptorHeaps[SCREEN_BUFFERING_NUM];
};

/** @brief Structured buffer data per mesh */
template <class StructBuffType>
struct SbDataPerMesh {
    /** @brief Handle for per graphics component structured buffer data */
    WeakStableHandle<SbDataPerComponent<StructBuffType>> handle;
    /** @brief Index of mesh that graphics component has */
    UINT meshIndex;
    /** @brief The number of elements to be used */
    UINT elementNum;
    /** @brief The offset of buffer to be used */
    UINT offset;
    /** @brief Last updated frame number */
    UINT lastUpdateFrame;
};

/** @brief Key state of ACSbAllocator */
enum class SbAllocatorKeyState {
    /** @brief Key doesn't exist */
    NotExist,
    /** @brief Not yet calculated */
    NotCalculated,
    /** @brief Already calculated */
    Calculated,
};

/**
   @brief This class allocates data for structured buffer
   @tparam StructBuffType type of structured buffer
*/
template <class StructBuffType>
class ACSbAllocater {
protected:
    /**
       @brief Constructor
       @param hlslBufferName The name of cbuffer defined in hlsl
       @param typeSize Size of this structured buffer type
    */
    ACSbAllocater(std::string hlslBufferName) 
        : m_hlslBufferName(std::move(hlslBufferName)), m_currentFrameNumber(1)
    {}

    /**
       @brief Destructor
    */
    virtual ~ACSbAllocater() = default;

    /**
       @brief Copy data directly
       @param key Key of a mesh
       @param src Data to be copy
       @return CPU handle for data copied
       @note The Map operation is called within the function. However, the Unmap operation is not called
    */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE DirectDataCopy(MeshKey key, const StructBuffType* src);

    /**
       @brief Create a per graphics component structured buffer
       @param size Size of array that structured buffer has
       @param viewNum Number of views created from the structured buffer
    */
    void CreatePerComponentBuffer(UINT size, UINT viewNum);

    /**
       @brief Create a per mesh data
       @param meshIndex Index of mesh that graphics component has
       @param meshKey Key of mesh
       @param elementNum The number of elements to be used
       @param offset The offset of buffer to be used
    */
    void CreatePerMeshData(UINT meshIndex, MeshKey meshKey, UINT elementNum, UINT offset);

    /**
       @brief Release buffer and per mesh data
       @param meshKey Key to be released
    */
    void Release(MeshKey meshKey);

    /**
       @brief Refresh process that must be called at the end of every frame
    */
    void RefreshSbAllocator();

    /**
       @brief Check for key state
       @param key Key to be checked
       @return Result
    */
    inline SbAllocatorKeyState CheckKey(MeshKey key);

    /** @brief Get CPU handle for each descriptor heap */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetCPUHandle(MeshKey key);

protected:
    /** @brief The name of cbuffer defined in hlsl */
    const std::string m_hlslBufferName;

private:
    /** @brief Per graphics component structured buffers */
    StableHandleVector<SbDataPerComponent<StructBuffType>> m_buffers;
    /** @brief Map that connect mesh keys to per mesh data */
    std::unordered_map<MeshKey, SbDataPerMesh<StructBuffType>> m_dataPerMesh;
    /** @brief Current frame number to check if data has been updated */
    UINT m_currentFrameNumber;
};

// Copy data directly
template <class StructBuffType>
Utl::Dx::CPU_DESCRIPTOR_HANDLE ACSbAllocater<StructBuffType>::DirectDataCopy(MeshKey key, const StructBuffType* src) {
    // Get next back buffer index
    UINT bufferIndex = CSwapChain::GetMain().GetNextBackBufferIndex();

    // If mesh data doesn't exist in the map, return null data
    auto it = m_dataPerMesh.find(key);
    if (it == m_dataPerMesh.end()) {
        OutputDebugString(L"Warning! Structured buffer data for a sent mesh doesn't exist map!\n");
        return Utl::Dx::CPU_DESCRIPTOR_HANDLE();
    }

    // Perform map operation
    m_buffers[*it->second.handle].structuredBuffers[bufferIndex].Map();

    // Update data of the structured buffer
    m_buffers[*it->second.handle].structuredBuffers[bufferIndex].DataCopy(src, it->second.elementNum, it->second.offset);

    // Update last updated frame number
    it->second.lastUpdateFrame = m_currentFrameNumber;

    // Get specified CPU handle from descriptor heap
    return m_buffers[*it->second.handle].descriptorHeaps[bufferIndex].GetCpuHandle(it->second.meshIndex);
}

// Create a per graphics component structured buffer
template <class StructBuffType>
void ACSbAllocater<StructBuffType>::CreatePerComponentBuffer(UINT size, UINT viewNum) {
    m_buffers.EmplaceBack();
    auto& newBuffer = m_buffers[*m_buffers.BackHandle()];
    // Create structured buffers and descriptor heaps
    for (UINT i = 0; i < SCREEN_BUFFERING_NUM; ++i) {
        newBuffer.structuredBuffers[i].Create(size);
        newBuffer.descriptorHeaps[i].Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, viewNum);
    }
}

// Create a per mesh data
template <class StructBuffType>
void ACSbAllocater<StructBuffType>::CreatePerMeshData(UINT meshIndex, MeshKey meshKey, UINT elementNum, UINT offset) {
    // Emplace data in the map
    WeakStableHandle<SbDataPerComponent<StructBuffType>> handle = m_buffers.BackHandle();
    m_dataPerMesh.emplace(meshKey, SbDataPerMesh<StructBuffType>());

    // Copy needed data
    SbDataPerMesh<StructBuffType>& newData = m_dataPerMesh[meshKey];
    newData.meshIndex = meshIndex;
    newData.handle = handle;
    newData.elementNum = elementNum;
    newData.offset = offset;
    newData.lastUpdateFrame = m_currentFrameNumber - 1;

    // Create shader resource view desc
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.FirstElement = offset;
    srvDesc.Buffer.NumElements = elementNum;
    srvDesc.Buffer.StructureByteStride = (UINT)sizeof(StructBuffType);
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    
    // Create shader resource views
    ID3D12Device* device = CApplication::GetAny().GetDXDevice();
    SbDataPerComponent<StructBuffType>& buffer = m_buffers[*handle];
    for (UINT i = 0; i < SCREEN_BUFFERING_NUM; ++i) {
        device->CreateShaderResourceView(
            buffer.structuredBuffers[i].GetResource(), &srvDesc,
            buffer.descriptorHeaps[i].GetCpuHandle(meshIndex));
    }
}

// Release buffer and per mesh data
template <class StructBuffType>
void ACSbAllocater<StructBuffType>::Release(MeshKey meshKey) {
    auto it = m_dataPerMesh.find(meshKey);
    if (it != m_dataPerMesh.end()) {
        // Release per component data
        if (it->second.handle) {
            m_buffers.Erase(*it->second.handle);
        }
        // Release per mesh data
        m_dataPerMesh.erase(it);
    }
}

// Refresh process that must be called at the end of every frame
template <class StructBuffType>
void ACSbAllocater<StructBuffType>::RefreshSbAllocator() {
    // Advance the current frame number
    m_currentFrameNumber++;
}

// Check for key state
template <class StructBuffType>
inline SbAllocatorKeyState ACSbAllocater<StructBuffType>::CheckKey(MeshKey key) {
    // Check if the key exists in the map
    auto it = m_dataPerMesh.find(key);
    if (it == m_dataPerMesh.end()) {
        return SbAllocatorKeyState::NotExist;
    }
    // Check if key's data has not been calculated
    else if (it->second.lastUpdateFrame != m_currentFrameNumber) {
        return SbAllocatorKeyState::NotCalculated;
    }
    return SbAllocatorKeyState::Calculated;
}

// Get CPU handle for each descriptor heap
template <class StructBuffType>
inline Utl::Dx::CPU_DESCRIPTOR_HANDLE ACSbAllocater<StructBuffType>::GetCPUHandle(MeshKey key) {
    SbDataPerMesh<StructBuffType>& data = m_dataPerMesh[key];
    return m_buffers[*data.handle].descriptorHeaps[CSwapChain::GetMain().GetNextBackBufferIndex()].GetCpuHandle(data.meshIndex);
}

#endif // !__ABSTRACT_SB_ALLOCATOR_H__
