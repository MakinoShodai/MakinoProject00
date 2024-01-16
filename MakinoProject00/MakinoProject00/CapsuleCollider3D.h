/**
 * @file   CapsuleCollider3D.h
 * @brief  This file handles 3D capsule collider.
 *
 * @author Shodai Makino
 * @date   2023/07/29
 */

#ifndef __CAPSULECOLLIDER_3D_H__
#define __CAPSULECOLLIDER_3D_H__

#include "Collider3D.h"

/** @brief 3D Capsule collider component */
class CCapsuleCollider3D : public ACCollider3D {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this component
       @param length The length of capsule collider axis
       @param radius The radius of capsule hemispheres
       @param offset The offset of the center coordinates of the sphere collider
    */
    CCapsuleCollider3D(CGameObject* owner, float length = 2.0f, float radius = 0.5f, const Vector3f& offset = Vector3f::Zero());
    /**
       @brief Destructor
    */
    ~CCapsuleCollider3D() override = default;

    /** @brief Get scaling radius of capsule hemispheres */
    float GetScalingRadius() const { return m_scalingRadius; }
    /** @brief Get radius of capsule hemispheres */
    float GetRadius() const { return m_radius; }
    /** @brief Set radius of capsule hemispheres */
    void SetRadius(float radius);


    /** @brief Get height of capsule collider */
    float GetHeight() const;
    /** @brief Get scaling height of capsule collider */
    float GetScalingLength() const { return m_scalingLength; }
    /** @brief Get length of capsule collider axis */
    float GetLength() const { return m_length; }
    /** @brief Set length of capsule collider axis */
    void SetLength(float length);

    /** @brief Get axis of capsule collider */
    UINT8 GetAxisIndex() const { return m_axis; }
    /** @brief Set axis of capsule collider */
    void SetAxisIndex(UINT8 axisIndex) { if (axisIndex < 3) { m_axis = axisIndex; } }

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
       @brief Update scaling variables
    */
    void UpdateScaling() override;

    /**
       @brief Update bounding volume in local coordinate space
    */
    void UpdateLocalBoudingVolume() override;

private:
    /** @brief Axis of capsule collider */
    UINT8 m_axis;
    /** @brief Radius of capsule hemispheres */
    float m_radius;
    /** @brief Scaled radius of capsule hemispheres to actual value */
    float m_scalingRadius;
    /** @brief Length of capsule collider axis */
    float m_length;
    /** @brief Scaled length of capsule collider axis to actual value */
    float m_scalingLength;
};

#endif // !__CAPSULECOLLIDER_3D_H__
