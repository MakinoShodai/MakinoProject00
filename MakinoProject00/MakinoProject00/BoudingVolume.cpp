#include "BoudingVolume.h"

// Constructor that creates a bounding volume that contains two bounding volumes
Mkpe::BoundingVolume::BoundingVolume(const BoundingVolume& a, const BoundingVolume& b) {
    minAABB.x() = (std::min)(a.minAABB.x(), b.minAABB.x());
    minAABB.y() = (std::min)(a.minAABB.y(), b.minAABB.y());
    minAABB.z() = (std::min)(a.minAABB.z(), b.minAABB.z());
    maxAABB.x() = (std::max)(a.maxAABB.x(), b.maxAABB.x());
    maxAABB.y() = (std::max)(a.maxAABB.y(), b.maxAABB.y());
    maxAABB.z() = (std::max)(a.maxAABB.z(), b.maxAABB.z());
}

// Constructor that creates a bounding volume that contains three bounding volumes
Mkpe::BoundingVolume::BoundingVolume(const BoundingVolume& a, const BoundingVolume& b, const BoundingVolume& c)
    : BoundingVolume(BoundingVolume(a, b), c) 
{ }

// Constructor that moves a bounding volume defined in local coordinate space to world coordinate space
Mkpe::BoundingVolume::BoundingVolume(const Vector3f& pos, const BoundingVolume& localBV) {
    minAABB = localBV.minAABB + pos;
    maxAABB = localBV.maxAABB + pos;
}

// Expand the bounding volume by a specified amount in each axis
void Mkpe::BoundingVolume::Expand(const Vector3f& expand) {
    minAABB -= expand;
    maxAABB += expand;
}

// Get const of bounding volume
const float Mkpe::BoundingVolume::GetCost() const {
    float lengthX = maxAABB.x() - minAABB.x();
    float lengthY = maxAABB.y() - minAABB.y();
    float lengthZ = maxAABB.z() - minAABB.z();
    return 2.0f * (lengthX * lengthY + lengthY * lengthZ + lengthZ * lengthX);
}

// Get proximity to target bounding volume
const float Mkpe::BoundingVolume::GetProximity(const BoundingVolume& target) const {
    const Vector3f diff = (minAABB + maxAABB) - (target.minAABB + target.maxAABB);
    return (fabsf(diff.x()) + fabsf(diff.y()) + fabsf(diff.z()));
}
