/**
 * @file   BoudingVolume.h
 * @brief  This file handles Bounding volume using AABB.
 * 
 * @author Shodai Makino
 * @date   2023/07/18
 */

#ifndef __BOUNDING_VOLUME_H__
#define __BOUNDING_VOLUME_H__

#include "UtilityForMath.h"

namespace Mkpe {
    /** @brief Bounding volume using AABB */
    struct BoundingVolume {
        /** @brief Minimum coordinates of AABB */
        Vector3f minAABB;
        /** @brief Maximum coordinates of AABB */
        Vector3f maxAABB;

        /**
           @brief Constructor
           @param minAABB Minimum coordinates of AABB
           @param maxAABB Maximum coordinates of AABB
        */
        BoundingVolume(const Vector3f& minAABB = Vector3f(0.0f, 0.0f, 0.0f), const Vector3f& maxAABB = Vector3f(0.0f, 0.0f, 0.0f))
            : minAABB(minAABB), maxAABB(maxAABB) { }

        /**
           @brief Constructor that creates a bounding volume that contains two bounding volumes
           @param a First bounding volume
           @param b Second bounding volume
        */
        BoundingVolume(const BoundingVolume& a, const BoundingVolume& b);

        /**
           @brief Constructor that creates a bounding volume that contains three bounding volumes
           @param a First bounding volume
           @param b Second bounding volume
           @param c Third bounding volume
        */
        BoundingVolume(const BoundingVolume& a, const BoundingVolume& b, const BoundingVolume& c);

        /**
           @brief Constructor that moves a bounding volume defined in local coordinate space to world coordinate space
           @param pos Coordinates in world coordinate space
           @param localBV Bounding volume defined in local coordinate space
        */
        BoundingVolume(const Vector3f& pos, const BoundingVolume& localBV);

        /**
           @brief Expand the bounding volume by a specified amount in each axis
           @param expand How much to spread on each axis
        */
        void Expand(const Vector3f& expand);

        /** @brief Get const of bounding volume */
        const float GetCost() const;

        /** @brief Get proximity to target bounding volume */
        const float GetProximity(const BoundingVolume& target) const;

        /**
           @brief Is this BV contains the subject BV?
           @param target Target bounding volume
           @return Return true if contained
        */
        inline const bool IsContain(const BoundingVolume& target) const;

        /**
           @brief Is this BV in contact with the subject BV?
           @param target Target bounding volume
           @return Return true if contacted
        */
        inline const bool IsIntersect(const BoundingVolume& target) const;
    };
} // namespace Mkpe

// Is this BV contains the subject BV?
inline const bool Mkpe::BoundingVolume::IsContain(const BoundingVolume& target) const {
    return (
        (minAABB.x() <= target.minAABB.x()) &&
        (minAABB.z() <= target.minAABB.z()) &&
        (minAABB.y() <= target.minAABB.y()) &&
        (maxAABB.x() >= target.maxAABB.x()) &&
        (maxAABB.z() >= target.maxAABB.z()) &&
        (maxAABB.y() >= target.maxAABB.y()));
}

// Is this BV in contact with the subject BV?
inline const bool Mkpe::BoundingVolume::IsIntersect(const BoundingVolume& target) const {
    return (
        (minAABB.x() <= target.maxAABB.x()) &&
        (maxAABB.x() >= target.minAABB.x()) &&
        (minAABB.z() <= target.maxAABB.z()) &&
        (maxAABB.z() >= target.minAABB.z()) &&
        (minAABB.y() <= target.maxAABB.y()) &&
        (maxAABB.y() >= target.minAABB.y()));
}

#endif // !__BOUNDING_VOLUME_H__
