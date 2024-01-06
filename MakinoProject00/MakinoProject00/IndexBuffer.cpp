#include "IndexBuffer.h"

// Update heap properties and resource descriptor, for resource
void CIndexBuffer::UpdateDataForResource(D3D12_HEAP_PROPERTIES* heapProp, D3D12_RESOURCE_DESC* resDesc) {
    // Fast access from GPU
    heapProp->Type = D3D12_HEAP_TYPE_DEFAULT;
}

// Set command to copy data
void CIndexBuffer::CopyRegion(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* srcResource) {
    cmdList->CopyBufferRegion(m_resource.Get(), 0, srcResource, 0, static_cast<UINT64>(m_numIndices) * m_bytesPerIndex);
}
