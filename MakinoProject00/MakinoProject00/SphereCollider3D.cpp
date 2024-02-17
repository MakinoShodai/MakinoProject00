#include "SphereCollider3D.h"
#include "ColliderWrapper.h"

// Constructor
CSphereCollider3D::CSphereCollider3D(CGameObject* owner, float radius, const Vector3f& offset)
    : ACCollider3D(owner, ColliderType::Sphere, offset)
    , m_radius(radius)
    , m_scalingRadius(radius) {
}

// Set radius of sphere collider
void CSphereCollider3D::SetRadius(float radius) {
    m_radius = radius;

    // Update the radius
    UpdateScaling();

    // Update the bounding volume
    UpdateLocalBoudingVolume();
}

// Generate a reduced version of the local inertia tensor
void CSphereCollider3D::GenerateInvInertiaTensorLocal(Vector3f* local) {
    constexpr float fraction = 2.0f / 5.0f;
    *local = Vector3f::Ones() * (1.0f / (fraction * GetWrapper()->GetMass() * m_scalingRadius * m_scalingRadius));
}

// Get support vector in local coordinate space
Vector3f CSphereCollider3D::GetLocalSupportVector(const Vector3f& v) const {
    return v.GetNormalize() * m_scalingRadius;
}

// Update scaling variables
void CSphereCollider3D::UpdateScaling() {
    // Get the maximum scale in each axis
    const Vector3f scale = GetTransform().scale;
    float maxScale = (std::max)(scale.x(), (std::max)(scale.y(), scale.z()));

    // Update scaling radius
    m_scalingRadius = m_radius * maxScale;
}

// Update bounding volume in local coordinate space
void CSphereCollider3D::UpdateLocalBoudingVolume() {
    m_localBV.minAABB = -Vector3f(m_scalingRadius, m_scalingRadius, m_scalingRadius);
    m_localBV.maxAABB = Vector3f(m_scalingRadius, m_scalingRadius, m_scalingRadius);
}
