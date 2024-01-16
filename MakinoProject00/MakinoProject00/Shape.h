/**
 * @file   Shape.h
 * @brief  This file handles graphics component for shape.
 * 
 * @author Shodai Makino
 * @date   2024/1/4
 */

#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "GraphicsComponent.h"
#include "BoxCollider3D.h"
#include "CapsuleCollider3D.h"
#include "SphereCollider3D.h"

/** @brief Kind of shape */
enum class ShapeKind {
    /** @brief Sphere */
    Sphere = 0,
    /** @brief Box */
    Box,
    /** @brief Capsule */
    Capsule,
    /** @brief Max number of this enum class */
    Max
};

/** @brief Shape for texture */
class CTexShape : public ACGraphicsComponent {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
       @param kind Kind of this shape
       @param texFilePath Path of texture file
    */
    CTexShape(CGameObject* owner, GraphicsLayer layer, ShapeKind kind, const std::wstring& texFilePath);

private:
    /** @brief Kind of this shape */
    ShapeKind m_kind;
};

/** @brief Shape for color only */
class CColorOnlyShape : public ACGraphicsComponent {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
       @param kind Kind of this shape
       @param color Color
    */
    CColorOnlyShape(CGameObject* owner, GraphicsLayer layer, ShapeKind kind, const Colorf& color);

private:
    /** @brief Kind of this shape */
    ShapeKind m_kind;
};

/** @brief Shape for debugging collider */
class CDebugColliderShape : public ACGraphicsComponent {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
       @param collider Associate Collider
    */
    CDebugColliderShape(CGameObject* owner, GraphicsLayer layer, CWeakPtr<ACCollider3D> collider);

    /**
       @brief Drawing conditions
       @return Can this shape draw?
    */
    inline bool IsDrawCondition() override { return m_collider != nullptr; }

    /** @brief Get kind of shape */
    ShapeKind GetKind() { return m_kind; }
    /** @brief Get collider */
    CWeakPtr<ACCollider3D> GetCollider() { return m_collider; }

private:
    /** @brief Kind of this shape */
    ShapeKind m_kind;
    /** @brief Weak pointer to collider */
    CWeakPtr<ACCollider3D> m_collider;
};

/** @brief Static class that handles shape meshes */
class CShapeMesh {
public:
    /**
       @brief Initialize
    */
    static void Initialize();

    /** @brief Get mesh */
    static const CMeshBufferFull& GetMesh(ShapeKind kind) { return ms_shape[static_cast<UINT>(kind)]; }

private:
    /** @brief Create mesh buffer of sphere */
    static void CreateSphere();
    /** @brief Create mesh buffer of box */
    static void CreateBox();
    /** @brief Create mesh buffer of capsule */
    static void CreateCapsule();

private:
    /** @brief Mesh buffers of each shape */
    static CMeshBufferFull ms_shape[static_cast<UINT>(ShapeKind::Max)];
};

#endif // !__SHAPE_H__
