/**
 * @file   Collider3D.h
 * @brief  This file handles 3D abstract collider.
 * 
 * @author Shodai Makino
 * @date   2023/07/19
 */

#ifndef __COLLIDER_3D_H__
#define __COLLIDER_3D_H__

#include "Component.h"
#include "BoudingVolume.h"
#include "BVNode.h"
#include "StableHandleVector.h"
#include "ColliderTag.h"

// Forward declarations
namespace Mkpe { 
    class CColliderWrapper; 
    class CContactColliderList;
}

/** @brief Type of collider class */
enum ColliderType : UINT {
    /** @brief Sphere */
    Sphere = 0,
    /** @brief Capsule */
    Capsule,
    /** @brief Box */
    Box,
    /** @brief Max number of collider type */
    Max,
};

/** @brief Name of callback function at collider Awake processing */
static inline const std::string CALLBACK_COLLIDER_AWAKE = "ACCollider3DAwake";

/** @brief Collider abstract class in 3D coordinate space */
class ACCollider3D : public ACComponent {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this component
       @param type Collider type
       @param offset The offset of the center coordinates of the collider
    */
    ACCollider3D(CGameObject* owner, ColliderType type, const Vector3f& offset);

    /**
       @brief Destructor
    */
    virtual ~ACCollider3D() = 0 {}

    /**
       @brief Awake processing
    */
    void Awake();

    /**
       @brief Start processing
    */
    void Start() override final;
    
    /**
       @brief Update processing
    */
    void Update() override final {}

    /**
       @brief Processing at destruction
    */
    void OnDestroy() override;

    /**
       @brief Generate a reduced version of the local inertia tensor
       @param local Variable to receive the result
    */
    virtual void GenerateInvInertiaTensorLocal(Vector3f* local) = 0;

    /**
       @brief Get support vector in local coordinate space
       @param v Input vector
       @return Support vector in local coordinate space
    */
    virtual Vector3f GetLocalSupportVector(const Vector3f& v) const = 0;

    /**
       @brief Generate bounding volume
       @return Generated bounding volume
    */
    Mkpe::BoundingVolume GenerateBoudingVolume() const;

    /**
       @brief Check if scaling and rotation updates are needed, update if necessary
    */
    void CheckScalingRotation();

    /**
       @brief Update required values according to scale and rotation without checks
    */
    void ScalingRotationUpdate();

    /**
       @brief Is the ID sent the same as my leaf node ID?
       @param id ID to be checked
       @return Result
    */
    bool CheckMyLeafNodeID(Mkpe::Dbvt::NodeID id) const;

    /** @brief Get collider type */
    const ColliderType GetColliderType() const { return m_type; }

    /** @brief Get the maximum extent to which a vertex is shifted by rotation */
    float GetMaxRotateScalar() const;

    /** @brief Get the center coordinates considering offset */
    Vector3f GetCenter() const { return GetTransform().pos + m_scalingOffset; }

    /** @brief Set the offset of the center coordinates of the collider */
    void SetOffset(const Vector3f& offset);
    /** @brief Get the offset of the center coordinates of the collider */
    const Vector3f& GetOffset() const { return m_offset; }

    /** @brief Set to tag of this collider */
    void SetTag(ColliderTag tag) { if (Utl::CountBits((ColliderTagType)tag) == 1) { m_tag = tag; } }
    /** @brief Get tag of this collider */
    ColliderTag GetTag() const { return m_tag; }
    /** @brief Is the tag applicable to the sent mask position? */
    bool CheckTag(ColliderTag mask) { return (ColliderTagType)(m_tag & mask) != 0; }

    /** @brief Does the other's tag collide or overlap with this collider? */
    inline bool CheckTagEither(ACCollider3D* other) { return CColliderTagRegistry::GetAny().IsEither(m_tag, other->GetTag()); }
    /** @brief Does the other's tag collide with this collider? */
    inline bool CheckTagCollide(ACCollider3D* other) { return CColliderTagRegistry::GetAny().IsCollide(m_tag, other->GetTag()); }
    /** @brief Does the other's tag overlap with this collider? */
    inline bool CheckTagOverlap(ACCollider3D* other) { return CColliderTagRegistry::GetAny().IsOverlap(m_tag, other->GetTag()); }

    /** @brief Set the overlap flag */
    void SetIsOverlap(bool isOverlap) { m_isOverlap = isOverlap; }
    /** @brief Can it overlap with the contact opponent? */
    inline bool IsOverlap() const { return m_isOverlap; }

    /** @brief Get list class of the contact colliders for each categories */
    const Mkpe::CContactColliderList& GetContactColliderdLists() const;

protected:
    /** @brief Get this collider wrapper */
    Mkpe::CColliderWrapper* GetWrapper();
    /** @brief Get this collider wrapper */
    const Mkpe::CColliderWrapper* GetWrapper() const;

    /**
       @brief Update the scaling offset of the center coordinates of the collider
    */
    void UpdateScalingOffset();

    /**
       @brief Update scaling variables in derived classes
    */
    virtual void UpdateScaling() = 0;

    /**
       @brief Update bounding volume in local coordinate space
    */
    virtual void UpdateLocalBoudingVolume() = 0;

protected:
    /** @brief Handle to own wrapper */
    WeakStableHandle<CUniquePtrWeakable<Mkpe::CColliderWrapper>> m_wrapperHandle;
    /** @brief Bounding volume in local coordinate space */
    Mkpe::BoundingVolume m_localBV;
    /** @brief Type of this collider */
    const ColliderType m_type;
    /** @brief Offset of the center coordinates of the collider */
    Vector3f m_offset;
    /** @brief Scaling offset of the center coordinates of the collider to actual value */
    Vector3f m_scalingOffset;

    /** @brief Tag of this collider */
    ColliderTag m_tag;
    /** @brief Can it overlap with the contact opponent? */
    bool m_isOverlap;
};

#endif // !__COLLIDER_3D_H__
