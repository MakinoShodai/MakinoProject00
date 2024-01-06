#include "MeshBuffer.h"

// Set the information needed to construct the mesh
void CMeshBuffer::SetInfo(CWeakPtr<const CVertexBuffer> vertex, CWeakPtr<const CIndexBuffer> index, D3D_PRIMITIVE_TOPOLOGY topology) {
    assert(vertex != nullptr && index != nullptr);

    m_vertexBuffer = vertex;
    m_indexBuffer = index;
    m_topologyType = topology;
}

// This class handles vertex and index buffers entities, needed to construct the mesh
CMeshBufferFull::CMeshBufferFull() 
    : CMeshBuffer()
    , m_vertexBufferEntity(CUniquePtrWeakable<CVertexBuffer>::Make())
    , m_indexBufferEntity(CUniquePtrWeakable<CIndexBuffer>::Make()) {
    m_vertexBuffer = m_vertexBufferEntity.GetWeakPtr(); 
    m_indexBuffer = m_indexBufferEntity.GetWeakPtr(); 
}
