/**
 * @file   IndexBuffer.h
 * @brief  This file handles index buffer.
 * 
 * @author Shodai Makino
 * @date   2023/11/14
 */

#ifndef __INDEX_BUFFER_H__
#define __INDEX_BUFFER_H__

#include "GpuOptimizedResource.h"

/** @brief Is index type UINT? */
template<typename TIndex>
using IsIndexTypeUINT = std::disjunction<
    std::is_same<TIndex, uint8_t>,
    std::is_same<TIndex, uint16_t>,
    std::is_same<TIndex, uint32_t>
>;

/** @brief This class handles index buffer */
class CIndexBuffer : public ACGpuOptimizedResource {
public:
    /**
       @brief Constructor
    */
    CIndexBuffer() : ACGpuOptimizedResource(), m_ibView(), m_numIndices(0), m_bytesPerIndex(0){}

    /**
       @brief Create index buffer
       @tparam Index Type

       @param indicesData Indices data
       @param numIndices The number of indices
    */
    template<class TIndex>
        requires IsIndexTypeUINT<TIndex>::value
    void Create(const void* indicesData, UINT numIndices);

    /** @brief Get index buffer view */
    const D3D12_INDEX_BUFFER_VIEW* GetView() const { return &m_ibView; }
    /** @brief Get the number of indices */
    const UINT GetNumIndices() const { return m_numIndices; }

protected:
    /**
       @brief Update heap properties and resource descriptor, for resource
    */
    void UpdateDataForResource(D3D12_HEAP_PROPERTIES* heapProp, D3D12_RESOURCE_DESC* resDesc) override;

    /**
       @brief Set command to copy data
    */
    void CopyRegion(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* srcResource) override;


    /**
       @brief Return DXGI_FORMAT based on the type of index
       @return Supported DXGI_FORMAT
    */
    template <typename TIndex>
        requires IsIndexTypeUINT<TIndex>::value
    DXGI_FORMAT GetIndexFormat();

private:
    /** @brief Index buffer view */
    D3D12_INDEX_BUFFER_VIEW m_ibView;
    /** @brief The number of indices */
    UINT m_numIndices;
    /** @brief Bytes per index */
    UINT m_bytesPerIndex;
};

// Create index buffer
template<class TIndex>
    requires IsIndexTypeUINT<TIndex>::value
void CIndexBuffer::Create(const void* indicesData, UINT numIndices) {
    // Copy index information
    m_numIndices = numIndices;
    m_bytesPerIndex = sizeof(TIndex);

    // Calculate bytes of all indices
    UINT64 bytesAllIndices = static_cast<UINT64>(m_numIndices) * m_bytesPerIndex;

    // Define mapping function
    auto mappingFunc = [&](UINT8* mappedBuffer) {
        memcpy(mappedBuffer, indicesData, bytesAllIndices);
    };

    // Create resource
    CreateResource(bytesAllIndices, D3D12_RESOURCE_STATE_INDEX_BUFFER, mappingFunc);

    // Create index buffer view
    m_ibView.BufferLocation = m_resource->GetGPUVirtualAddress();
    m_ibView.Format = GetIndexFormat<TIndex>();
    m_ibView.SizeInBytes = (UINT)bytesAllIndices;
}

// Return DXGI_FORMAT based on the type of index
template<typename TIndex>
    requires IsIndexTypeUINT<TIndex>::value
DXGI_FORMAT CIndexBuffer::GetIndexFormat() {
    // 8
    if constexpr (std::is_same_v<TIndex, uint8_t>) {
        return DXGI_FORMAT_R8_UINT;
    }
    // 16
    else if constexpr (std::is_same_v<TIndex, uint16_t>) {
        return DXGI_FORMAT_R16_UINT;
    }
    // 32
    else if constexpr (std::is_same_v<TIndex, uint32_t>) {
        return DXGI_FORMAT_R32_UINT;
    }
    // Exception
    else {
        static_assert(IsIndexTypeUINT<TIndex>::value, "Invalid index type");
        return DXGI_FORMAT_UNKNOWN;
    }
}

#endif // !__IDNEX_BUFFER_H__
