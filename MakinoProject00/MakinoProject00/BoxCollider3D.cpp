#include "BoxCollider3D.h"
#include "ColliderWrapper.h"

// Constructor
CBoxCollider3D::CBoxCollider3D(CGameObject* owner, const Vector3f& size, const Vector3f& offset)
    : ACCollider3D(owner, ColliderType::Box, offset)
    , m_size(size)
    , m_scalingSize(size)
{}

// Set the size of box collider
void CBoxCollider3D::SetSize(const Vector3f & size) {
    // Update the size
    m_size = size;
    // Update the scaling size
    UpdateScaling();

    // Update the bounding volume
    UpdateLocalBoudingVolume();
}

// Generate a reduced version of the local inertia tensor
void CBoxCollider3D::GenerateInvInertiaTensorLocal(Vector3f* local) {
    constexpr float denom = 1.0f / 12.0f;
    float mass = GetWrapper()->GetMass();

    // Calculate the diagonal elements of the inverse of the inertia tensor
    local->x() = 1.0f / (denom * mass * (m_scalingSize.y() * m_scalingSize.y() + m_scalingSize.z() * m_scalingSize.z()));
    local->y() = 1.0f / (denom * mass * (m_scalingSize.x() * m_scalingSize.x() + m_scalingSize.z() * m_scalingSize.z()));
    local->z() = 1.0f / (denom * mass * (m_scalingSize.x() * m_scalingSize.x() + m_scalingSize.y() * m_scalingSize.y()));
}

// Get support vector in local coordinate space
Vector3f CBoxCollider3D::GetLocalSupportVector(const Vector3f& v) const {
    Vector3f half = GetScalingSize() * Utl::Inv::_2;
    return Vector3f(
        (v.x() >= 0) ? half.x() : -half.x(),
        (v.y() >= 0) ? half.y() : -half.y(),
        (v.z() >= 0) ? half.z() : -half.z()
    );
}

// Update scaling variable
void CBoxCollider3D::UpdateScaling() {
    m_scalingSize = m_size * GetTransform().scale;
}

// Update bounding volume in local coordinate space
void CBoxCollider3D::UpdateLocalBoudingVolume() {
    const Quaternionf& rotation = GetTransform().rotation;

    // Apply rotation to each axis vector with half size applied
    Matrix3x3f mat = rotation.GetMatrix();
    Vector3f rotateSizeAxisX = Utl::Math::MultiplyMat3x3AxisToScalar(mat, m_scalingSize.x() * Utl::Inv::_2, Utl::Math::_X);
    Vector3f rotateSizeAxisY = Utl::Math::MultiplyMat3x3AxisToScalar(mat, m_scalingSize.y() * Utl::Inv::_2, Utl::Math::_Y);
    Vector3f rotateSizeAxisZ = Utl::Math::MultiplyMat3x3AxisToScalar(mat, m_scalingSize.z() * Utl::Inv::_2, Utl::Math::_Z);

    // Project half size on each axis
    Vector3f dotHalfSize = Vector3f(
        std::abs(Utl::Math::UNIT3_RIGHT.Dot(rotateSizeAxisX)) + std::abs(Utl::Math::UNIT3_RIGHT.Dot(rotateSizeAxisY)) + std::abs(Utl::Math::UNIT3_RIGHT.Dot(rotateSizeAxisZ)),
        std::abs(Utl::Math::UNIT3_UP.Dot(rotateSizeAxisX)) + std::abs(Utl::Math::UNIT3_UP.Dot(rotateSizeAxisY)) + std::abs(Utl::Math::UNIT3_UP.Dot(rotateSizeAxisZ)),
        std::abs(Utl::Math::UNIT3_FORWARD.Dot(rotateSizeAxisX)) + std::abs(Utl::Math::UNIT3_FORWARD.Dot(rotateSizeAxisY)) + std::abs(Utl::Math::UNIT3_FORWARD.Dot(rotateSizeAxisZ))
    );

    m_localBV.minAABB = -dotHalfSize;
    m_localBV.maxAABB = dotHalfSize;
}
