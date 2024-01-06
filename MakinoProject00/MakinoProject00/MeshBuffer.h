/**
 * @file   MeshBuffer.h
 * @brief  This file handles vertex and index buffers needed to construct the mesh.
 * 
 * @author Shodai Makino
 * @date   2023/11/23
 */

#ifndef __MESH_BUFFER_H__
#define __MESH_BUFFER_H__

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniquePtr.h"

/**
   @brief This class handles, pointers to vertex and index buffers, needed to construct the mesh. 
   @details 
   This mesh buffer has no vertex and index buffers entities
*/
class CMeshBuffer {
public:
    /**
       @brief Constructor
    */
    CMeshBuffer() : m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_topologyType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {}

    /**
       @brief Set the information needed to construct the mesh
       @param vertex Weak pointer to vertex buffer
       @param index Weak pointer to index buffer
       @param topology Topology type
    */
    void SetInfo(CWeakPtr<const CVertexBuffer> vertex, CWeakPtr<const CIndexBuffer> index, D3D_PRIMITIVE_TOPOLOGY topology);
    
    /** @brief Get the pointer to vertex buffer */
    const CVertexBuffer* GetVertex() const { return m_vertexBuffer.Get(); }
    /** @brief Get the pointer to index buffer */
    const CIndexBuffer* GetIndex() const { return m_indexBuffer.Get(); }
    /** @brief Get the weak pointer to vertex buffer */
    CWeakPtr<const CVertexBuffer> GetVertexWeakPtr() const { return m_vertexBuffer; }
    /** @brief Get the weak pointer to index buffer */
    CWeakPtr<const CIndexBuffer> GetIndexWeakPtr() const { return m_indexBuffer; }
    /** @brief Get the topology type */
    D3D_PRIMITIVE_TOPOLOGY GetTopologyType() const { return m_topologyType; }

protected:
    /** @brief Pointer to vertex buffer */
    CWeakPtr<const CVertexBuffer> m_vertexBuffer;
    /** @brief Pointer to index buffer */
    CWeakPtr<const CIndexBuffer> m_indexBuffer;
    /** @brief Topology type */
    D3D_PRIMITIVE_TOPOLOGY m_topologyType;
};

/**
   @brief This class handles vertex and index buffers entities, needed to construct the mesh
   @details
   This mesh buffer has vertex and index buffers entities
*/
class CMeshBufferFull : public CMeshBuffer {
public:
    /**
       @brief Constructor
    */
    CMeshBufferFull();

    /** @brief Disallow setting vertex and index buffers externally */
    void SetInfo(CWeakPtr<const CVertexBuffer> vertex, CWeakPtr<const CIndexBuffer> index, D3D_PRIMITIVE_TOPOLOGY topology) = delete;

    /**
       @brief Set the information needed to construct the mesh
       @param topology Topology type
    */
    void SetInfo(D3D_PRIMITIVE_TOPOLOGY topology) { m_topologyType = topology; }

    /** @brief Get vertex buffer entity */
    CVertexBuffer* GetVertexEntity() { return m_vertexBufferEntity.Get(); }
    /** @brief Get index buffer entity */
    CIndexBuffer* GetIndexEntity() { return m_indexBufferEntity.Get(); }

private:
    /** @brief Vertex buffer entity */
    CUniquePtrWeakable<CVertexBuffer> m_vertexBufferEntity;
    /** @brief Index buffer entity */
    CUniquePtrWeakable<CIndexBuffer> m_indexBufferEntity;
};

#endif // !__MESH_BUFFER_H__

