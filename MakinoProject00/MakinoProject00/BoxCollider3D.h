/**
 * @file   BoxCollider3D.h
 * @brief  This file handles 3D box collider.
 * 
 * @author Shodai Makino
 * @date   2023/07/27
 */

#ifndef __BOXCOLLIDER_3D_H__
#define __BOXCOLLIDER_3D_H__

#include "Collider3D.h"

/** @brief 3D Box collider component */
class CBoxCollider3D : public ACCollider3D {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this component
       @param size The size of box collider
       @param offset The offset of the center coordinates of the sphere collider
    */
    CBoxCollider3D(CGameObject* owner, const Vector3f& size = Vector3f::Ones(), const Vector3f& offset = Vector3f::Zero());
    
    /**
       @brief Destcutor
    */
    ~CBoxCollider3D() override = default;

    /** @brief Get the scaling size of box collider */
    const Vector3f& GetScalingSize() const { return m_scalingSize; }
    /** @brief Get the size of box collider */
    const Vector3f& GetSize() const { return m_size; }
    /** @brief Set the size of box collider */
    void SetSize(const Vector3f& size);

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
       @brief Update scaling variable
    */
    void UpdateScaling() override;

    /**
       @brief Update bounding volume in local coordinate space
    */
    void UpdateLocalBoudingVolume() override;

public:
    /** @brief Size of box collider */
    Vector3f m_size;
    /** @brief Scaled size of box collider to actual value */
    Vector3f m_scalingSize;
};

#endif // !__BOXCOLLIDER_3D_H__
