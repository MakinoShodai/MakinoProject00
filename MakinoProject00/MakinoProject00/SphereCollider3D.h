/**
 * @file   SphereCollider3D.h
 * @brief  This file handles 3D sphere collider.
 *
 * @author Shodai Makino
 * @date   2023/07/19
 */

#ifndef __SPHERECOLLIDER_3D_H__
#define __SPHERECOLLIDER_3D_H__

#include "Collider3D.h"

/** @brief 3D Sphere collider component */
class CSphereCollider3D : public ACCollider3D {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this component
       @param radius The radius of sphere collider
       @param offset The offset of the center coordinates of the sphere collider
    */
    CSphereCollider3D(CGameObject* owner, float radius = 0.5f, const Vector3f& offset = Vector3f::Zero());
    /**
       @brief Destrutor
    */
    ~CSphereCollider3D() override {}

    /** @brief Get scaling radius of sphere collider */
    float GetScalingRadius() const { return m_scalingRadius; }
    /** @brief Get radius of sphere collider */
    float GetRadius() const { return m_radius; }
    /** @brief Set radius of sphere collider */
    void SetRadius(float radius);

    /**
       @brief Generate a reduced version of the local inertia tensor
       @param local Variable to receive the result
    */
    void GenerateInvInertiaTensorLocal(Vector3f* local) override;

    /**
       @brief Get support vector in local coordinate space
       @param v Input vector
       @return Support vector in local coordinate space
    */
    Vector3f GetLocalSupportVector(const Vector3f& v) const override;

private:
    /**
       @brief Update bounding volume in local coordinate space
    */
    void UpdateLocalBoudingVolume() override;

    /**
       @brief Update scaling variables
    */
    void UpdateScaling() override;

private:
    /** @brief Radius of sphere collider */
    float m_radius;
    /** @brief Scaling radius of sphere collider */
    float m_scalingRadius;
};

#endif // !__SPHERECOLLIDER_3D_H__
