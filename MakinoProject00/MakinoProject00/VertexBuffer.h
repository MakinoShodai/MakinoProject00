/**
 * @file   VertexBuffer.h
 * @brief  This file handles vertex buffer.
 * 
 * @author Shodai Makino
 * @date   2023/11/14
 */

#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__

#include "GpuOptimizedResource.h"

/** @brief Vertex buffer class */
class CVertexBuffer : public ACGpuOptimizedResource {
public:
    /**
       @brief Constructor
    */
    CVertexBuffer() : ACGpuOptimizedResource(), m_vbView(), m_numVertices(0), m_bytesPerVertex(0) {}

    /**
       @brief Create vertex buffer
       @tparam Vertex type

       @param verticesData Vertices data
       @param numVertices The number of vertices
    */
    template<class TVertex>
    void Create(const void* verticesData, UINT numVertices);

    /** @brief Get vertex buffer view */
    const D3D12_VERTEX_BUFFER_VIEW* GetView() const { return &m_vbView; }
    /** @brief Get the number of vertices */
    const UINT GetNumVertices() const { return m_numVertices; }
    
protected:
    /**
       @brief Update heap properties and resource descriptor, for resource
    */
    void UpdateDataForResource(D3D12_HEAP_PROPERTIES* heapProp, D3D12_RESOURCE_DESC* resDesc) override;

    /**
       @brief Set command to copy data
    */
    void CopyRegion(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* srcResource) override;

private:
    /** @brief Vertex buffer view */
    D3D12_VERTEX_BUFFER_VIEW m_vbView;
    /** @brief The number of vertices */
    UINT m_numVertices;
    /** @brief Bytes per vertex */
    UINT m_bytesPerVertex;
};

// Create vertex buffer
template<class TVertex>
void CVertexBuffer::Create(const void* verticesData, UINT numVertices) {
    // Copy vertex information
    m_numVertices = numVertices;
    m_bytesPerVertex = sizeof(TVertex);

    // Calculate bytes of all vertices
    UINT64 bytesAllVertices = static_cast<UINT64>(m_numVertices) * m_bytesPerVertex;

    // Define mapping function
    auto mappingFunc = [&](UINT8* mappedBuffer) {
        memcpy(mappedBuffer, verticesData, bytesAllVertices);
    };

    // Create resource
    CreateResource(bytesAllVertices, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, mappingFunc);

    // Create vertex buffer view
    m_vbView.BufferLocation = m_resource->GetGPUVirtualAddress();
    m_vbView.SizeInBytes = (UINT)bytesAllVertices;
    m_vbView.StrideInBytes = m_bytesPerVertex;
}

#endif // !__VERTEX_BUFFER_H__
