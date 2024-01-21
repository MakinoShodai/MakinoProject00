/**
 * @file   GraphicsComponent.h
 * @brief  This file handles component for graphics.
 * 
 * @author Shodai Makino
 * @date   2023/11/16
 */

#ifndef __GRAPHICS_COMPONENT_H__
#define __GRAPHICS_COMPONENT_H__

#include "UniquePtr.h"
#include "UtilityForDirectX.h"
#include "MeshBuffer.h"
#include "GraphicsLayer.h"
#include "Texture.h"
#include "UtilityForMath.h"
#include "IntKeyGenerater.h"

// Forward declarations
class CGraphicsPipelineState;
class CGameObject;

/** @brief Type of graphics component */
enum class GraphicsComponentType {
    /** @brief Sprite for UI */
    SpriteUI,
    /** @brief Sprite for 3D */
    Sprite3D,
    /** @brief Billboard */
    Billboard,
    /** @brief Non-animated model */
    BasicModel,
    /** @brief Skeltal model */
    SkeletalModel,
    /** @brief Texture shape */
    TexShape,
    /** @brief Color only shape */
    ColorShape,
    /** @brief Shape for debugging collider */
    DebugColliderShape,
};

/** @brief Key for mesh buffer */
using MeshKey = IntKey;

/** @brief Mesh buffer wrapper class for graphics component */
struct MeshWrapperForGraphics {
    /** @brief Mesh buffer */
    CMeshBuffer mesh;
    /** @brief Key for mesh buffer */
    MeshKey key;
};

/** @brief Abstract class of component of graphics */
class ACGraphicsComponent {
    /** @brief Array of unique pointers to texture */
    using UniquePtrTextureArray = CUniquePtr<CUniquePtr<CTexture>[]>;

public:
    /**
       @brief Constructor
       @param type Type of this graphics component
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
    */
    ACGraphicsComponent(GraphicsComponentType type, CGameObject* owner, GraphicsLayer layer);

    /**
       @brief Destructor
    */
    virtual ~ACGraphicsComponent();

    /**
       @brief Processing for the first draw frame
    */
    virtual void Start();

    /**
       @brief Pre drawing processing
    */
    virtual void PreDraw() {}

    /**
       @brief Drawing conditions set in derived classes
       @return Can this graphcis component draw?
    */
    inline virtual bool IsDrawCondition() { return true; }

    /**
       @brief Ask for textures assignment information to be built
       @param meshNum The number of all meshes
       @param gpso GPSO to assign textures
       @param totalNum Total number of textures needed
       @return Have you allocated the needed number of textures?
    */
    bool AskToAllocateTextures(UINT meshNum, const CGraphicsPipelineState& gpso, UINT totalNum);

    /**
       @brief Belong to a new layer
       @param layer Layer to be belonged
    */
    void SetLayer(GraphicsLayer layer);

    /** @brief Set active flag */
    void SetIsActive(bool isActive) { m_isActive = isActive; }
    /** @brief Is this graphics component itself active? */
    inline virtual bool IsActiveSelf() const { return m_isActive; }
    /** @brief Is this graphics component itself and the game object that owns it active? */
    virtual bool IsActiveOverall() const;

    /** @brief Set color */
    void SetColor(const Colorf& color) { m_color = color; }
    /** @brief Get color */
    virtual const Colorf& GetColor() const { return m_color; }

    /** @brief Get type of this graphics component */
    const GraphicsComponentType GetType() { return m_type; }
    /** @brief Get game object that is the owner of this graphics component */
    CWeakPtr<CGameObject> GetGameObj() { return m_gameObj; }
    /** @brief Get graphics layer to which this class belongs */
    GraphicsLayer GetLayer() { return m_layer; }
    /** @brief Get mesh buffer */
    const CMeshBuffer* GetMeshBuffer(UINT meshIndex) { return &m_meshWrappers[meshIndex].mesh; }
    /** @brief Get a key for mesh buffer */
    MeshKey GetMeshKey(UINT meshIndex) { return m_meshWrappers[meshIndex].key; }
    /** @brief Get the number of all meshes */
    UINT GetMeshNum() { return m_meshWrappers.Size(); }

protected:
    /**
       @brief Prepare arrays of the meshes and its textures
       @param meshNum The number of all meshes
       @param texNumPerMesh The number of textures per mesh
    */
    void InitializeMesh(UINT meshNum, UINT texNumPerMesh);

    /**
       @brief Set mesh information
       @param meshIndex Index of all meshes
       @param vertex Pointer to vertex buffer
       @param index Pointer to index buffer
       @param topology Topology type
    */
    void SetMeshInfo(UINT meshIndex, CWeakPtr<const CVertexBuffer> vertex, CWeakPtr<const CIndexBuffer> index, D3D_PRIMITIVE_TOPOLOGY topology) { m_meshWrappers[meshIndex].mesh.SetInfo(vertex, index, topology); }

    /**
       @brief Set texture
       @param meshIndex Index of all meshes
       @param texIndexPerMesh Index of textures per mesh
       @param tex Texture to use
    */
    void SetTexture(UINT meshIndex, UINT texIndexPerMesh, CTexture tex) { m_textures[meshIndex][texIndexPerMesh] = CUniquePtr<CTexture>::Make(std::move(tex)); }

    /**
       @brief Get texture
       @param meshIndex Index of all meshes
       @param texIndexPerMesh Index of textures per mesh
       @return Pointer to the texture
    */
    CTexture* GetTexture(UINT meshIndex, UINT texIndexPerMesh) { return m_textures[meshIndex][texIndexPerMesh].Get(); }

protected:
    /** @brief Game object that is the owner of this graphics component */
    CWeakPtr<CGameObject> m_gameObj;
    /** @brief Graphics layer to which this class belongs */
    GraphicsLayer m_layer;

private:
    /** @brief Type of this graphics component */
    const GraphicsComponentType m_type;
    /** @brief Is this graphics component active? */
    bool m_isActive;

    /** @brief Array of mesh buffers */
    CArrayUniquePtr<MeshWrapperForGraphics> m_meshWrappers;
    /** @brief Array of textures to be used per mesh */
    CUniquePtr<UniquePtrTextureArray[]> m_textures;
    /** @brief The number of textures per mesh */
    UINT m_texNumPerMesh;

    /** @brief Color */
    Colorf m_color;
};

#endif // !__GRAPHICS_COMPONENT_H__
