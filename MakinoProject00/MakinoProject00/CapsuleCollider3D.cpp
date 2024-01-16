#include "CapsuleCollider3D.h"
#include "ColliderWrapper.h"

// Constructor
CCapsuleCollider3D::CCapsuleCollider3D(CGameObject* owner, float length, float radius, const Vector3f& offset)
    : m_axis(Utl::Math::_Y)
    , m_radius(radius)
    , m_scalingRadius(radius)
    , m_length(length)
    , m_scalingLength(length)
    , ACCollider3D(owner, ColliderType::Capsule, offset)
{}

// Set radius of capsule hemispheres
void CCapsuleCollider3D::SetRadius(float radius) {
    m_radius = radius;

    // Update the scaling variables
    UpdateScaling();
    // Update the bounding volume
    UpdateLocalBoudingVolume();
}

// Get height of capsule collider
float CCapsuleCollider3D::GetHeight() const {
    // Calculate height of capsule collider
    float height = m_length * GetTransform().scale[m_axis] * Utl::Inv::_2;
    // Corrected to not less than radius
    height = (height < m_scalingRadius) ? m_scalingRadius : height;
    return height;
}

// Set length of capsule collider axis
void CCapsuleCollider3D::SetLength(float length) {
    m_length = length;

    // Update the scaling variables
    UpdateScaling();
    // Update the bounding volume
    UpdateLocalBoudingVolume();
}

// Generate a reduced version of the local inertia tensor
void CCapsuleCollider3D::GenerateInvInertiaTensorLocal(Vector3f* local) {
    // Denominator of the hemispherical part and the cylindrical part
    constexpr float hemisphereDenom = 2.0f / 5.0f;
    constexpr float cylinderDenom = 1.0f / 12.0f;

    // Get the mass
    float mass = GetWrapper()->GetMass();

    // Calculate the mass of the hemispherical part and the cylindrical part
    float mass_hemisphere = mass * Utl::Inv::_3;
    float mass_cylinder = 2.0f * mass * Utl::Inv::_3;

    // Calculate the inertia tensor value of the hemispherical part and the cylindrical part
    float radius2 = m_scalingRadius * 2.0f;
    float hemisphere = hemisphereDenom * mass_hemisphere * radius2;
    float cylinder = (Utl::Inv::_2 * mass_cylinder * radius2) + (cylinderDenom * mass_cylinder * m_scalingLength * 2.0f);

    // Calculate the inverse of inertia tensor
    *local = Vector3f::Ones() * (1.0f / (hemisphere + cylinder));
}

// Get support vector in local coordinate space
Vector3f CCapsuleCollider3D::GetLocalSupportVector(const Vector3f& v) const {
    // #NOTE : Divide the capsule into two hemispherical parts and a cylindrical part.
    // Calculate own up vector
    Vector3f upVector = Vector3f::Zero();
    upVector[m_axis] = 1.0f;

    // Check the direction with its own up vector and the given vector
    int sign = Utl::Sign(upVector.Dot(v));
    // Given a vertical vector, returns coordinates of center + radius
    if (sign == 0) {
        return v * m_scalingRadius;
    }
    // If the given vector is opposite to its own up-vector, invert the up-vector.
    else if (sign < 0) {
        upVector[m_axis] = -1.0f;
    }

    // Calculate the intersection of this capsule as an infinite cylinder with the given vector directions
    // #NOTE : Omitted calculation of Utl::Math::CalculateIntersectLineInfiniteCylinder.
    //         Point on the center axis of the infinite cylinder is made the origin and UpVector.
    //         Also, the starting point of the line is the origin
    Vector3f intersectPoint;
    {
        // Calculate vector S
        Vector3f S = upVector; // upVector - 0

        // Calculate dot product needed
        float V_V = v.Dot(v);
        float S_V = S.Dot(v);
     /* float P1_V = 0.dot(v);  */
        float S_S = S.Dot(S);
     /* float P1_S = 0.dot(S);  */
     /* float P1_P1 = 0.dot(0); */

        // Calculate each coefficient (omit the part where the calculation result is 0)
        float A = V_V - S_V * S_V / S_S;
     /* float B = P1_V - S_V * P1_S / S_S;                     */
     /*         = 0 - S_V * 0 / S_S;                           */
     /* float C = P1_P1 - P1_S * P1_S / S_S - radius * radius; */
     /*         = 0 - 0 * 0 / S_S - radius * radius;           */
        float C = -m_scalingRadius * m_scalingRadius;

        // If A is 0, then the axis vector of the capsule is parallel to the given vector
        if (std::abs(A) <= FLT_EPSILON) {
            return upVector * (m_scalingLength * Utl::Inv::_2 + m_scalingRadius);
        }

        // Calculate scalar values in √
        /* float sqrtScalar = 0 * 0 - A * C; */
        float sqrtScalar = -A * C;
        // Calculate square root
        sqrtScalar = sqrtf(sqrtScalar);

        // Calculate the end point of penetration
        /* float a = (0 + sqrtScalar) * invA; */
        /* intersectPoint = 0 + v * a; */
        float a = sqrtScalar / A;
        intersectPoint = v * a;
    }

    // Calculate end of the line segment of this capsule
    Vector3f segmentTop = upVector * (m_scalingLength * Utl::Inv::_2);
    
    // Calculate the vector from the end of the line segment to the intersection
    Vector3f toIntersect = intersectPoint - segmentTop;
    // If the axis element of this vector is negative, then there is an intersection in the cylinder part
    if ((sign > 0 && toIntersect[m_axis] < 0.0f) || (sign < 0 && toIntersect[m_axis] > 0.0f)) {
        // In this case, just return the intersection as is.
        return intersectPoint;
    }

    // If not, the intersection exists in the hemispherical part and is calculated
    // Normalize the vector from the end of the line segment to the intersection
    toIntersect.Normalize();
    // Calculate intersection of the hemispherical part
    return segmentTop + toIntersect * m_scalingRadius;
}

// Update scaling variables
void CCapsuleCollider3D::UpdateScaling() {
    const Vector3f& scale = GetTransform().scale;

    // Get the larger scale of the directions not used for the axis
    float radiusScale = (std::max)(scale[(m_axis + 1) % 3], scale[(m_axis + 2) % 3]);
    // Calculate radius taking into account scale
    m_scalingRadius = m_radius * radiusScale;

    // Calculate length taking into account scale
    float length = m_length * scale[m_axis];
    // Length should not be less than twice the radius
    float radius2 = m_scalingRadius * 2.0f;
    m_scalingLength = (radius2 > length) ? radius2 : length;
    // Subtract twice the radius from the length
    m_scalingLength -= radius2;
    if (m_scalingLength < 0.0f) {
        m_scalingLength = 0.0f;
    }

}

// Update bounding volume in local coordinate space
void CCapsuleCollider3D::UpdateLocalBoudingVolume() {
    const Quaternionf& rotation = GetTransform().rotation;

    // Apply length to axis vector of this capsule
    Matrix3x3f mat = rotation.GetMatrix();
    Vector3f rotateSizeAxis = Utl::Math::MultiplyMat3x3AxisToScalar(mat, m_scalingLength, m_axis);

    // Project half size on each axis
    float radius2 = m_scalingRadius * 2.0f;
    Vector3f dotHalfSize = Vector3f(
        std::abs(Utl::Math::UNIT3_X.Dot(rotateSizeAxis)) + radius2,
        std::abs(Utl::Math::UNIT3_Y.Dot(rotateSizeAxis)) + radius2,
        std::abs(Utl::Math::UNIT3_Z.Dot(rotateSizeAxis)) + radius2
    );

    m_localBV.minAABB = -dotHalfSize;
    m_localBV.maxAABB = dotHalfSize;
}
