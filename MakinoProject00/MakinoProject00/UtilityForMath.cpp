#include "UtilityForMath.h"

// Get the forward vector of this transform
Vector3f Transformf::GetForward() const {
    return rotation * Utl::Math::UNIT3_FORWARD;
}

// Get the backward vector of this transform
Vector3f Transformf::GetBackward() const {
    return rotation * Utl::Math::UNIT3_BACKWARD;
}

// Get the right vector of this transform
Vector3f Transformf::GetRight() const {
    return rotation * Utl::Math::UNIT3_RIGHT;
}

// Get the left vector of this transform
Vector3f Transformf::GetLeft() const {
    return rotation * Utl::Math::UNIT3_LEFT;
}

// Get the up vector of this transform
Vector3f Transformf::GetUp() const {
    return rotation * Utl::Math::UNIT3_UP;
}

// Get the down vector of this transform
Vector3f Transformf::GetDown() const {
    return rotation * Utl::Math::UNIT3_DOWN;
}

// Linear interpolation
Vector3f Utl::Math::Lerp(const Vector3f& a, const Vector3f& b, float t) {
    Vector3f diff = b - a;
    return a + diff * t;
}

// Spherical linear interpolation
Quaternionf Utl::Math::Slerp(const Quaternionf& a, const Quaternionf& b, float t) {
    float dot = a.x() * b.x() + a.y() * b.y() + a.z() * b.z() + a.w() * b.w();

    // Fixed to be the shortest rotation distance
    float signB = 1.0f;
    if (dot < 0.0f) {
        signB = -1.0f;
        dot = -dot;
    }

    // Compute the angle between the quaternions
    float angle = std::acos(Utl::Clamp(dot, -1.0f, 1.0f));
    float sinAngle = std::sin(angle);

    // Check if the angle is too small
    if (std::abs(sinAngle) <= FLT_EPSILON) {
        // Linear interpolation
        Quaternionf ret(
            a.x() + t * (b.x() * signB - a.x()),
            a.y() + t * (b.y() * signB - a.y()),
            a.z() + t * (b.z() * signB - a.z()),
            a.w() + t * (b.w() * signB - a.w()));
        ret.Normalize();
        return ret;
    }

    // Calculate the cofficients for interpolation
    float invSinAngle = 1.0f / sinAngle;
    float coeff1 = std::sin((1.0f - t) * angle) * invSinAngle;
    float coeff2 = std::sin(t * angle) * invSinAngle;

    // Interpolate
    return Quaternionf(
        coeff1 * a.x() + coeff2 * b.x() * signB,
        coeff1 * a.y() + coeff2 * b.y() * signB,
        coeff1 * a.z() + coeff2 * b.z() * signB,
        coeff1 * a.w() + coeff2 * b.w() * signB
    );
}

// Calculate quaternion to rotate vector A to vector B
Quaternionf Utl::Math::VectorToVectorQuaternion(const Vector3f& a, const Vector3f& b) {
    Vector3f cross = a.Cross(b);
    cross.Normalize();
    // If not parallel vectors
    if (!Utl::Math::IsUnitVector3fZero(cross)) {
        float angle = acosf(Utl::Clamp(a.Dot(b), -1.0f, 1.0f));

        // Calculate the quaternion of this rotation
        return Quaternionf(angle, cross);
    }
    // If parallel vectors
    else {
        // Determine if it is the exact opposite vector.
        float dot = a.Dot(b);
        if (dot < 0.0f) {
            // Calculate a vector vertical with an appropriate vector that is not parallel to a
            cross = a.Cross((std::abs(a.x()) > FLT_EPSILON) ? Utl::Math::UNIT3_UP : Utl::Math::UNIT3_RIGHT);

            // Calculate the quaternion of this rotation
            return Quaternionf(Utl::PI, cross.GetNormalize());
        }
        else {
            return Quaternionf();
        }
    }
}

// Calculate and return tangent vector
Vector3f Utl::Math::GetTangentVector(const Vector3f& v) {
    Vector3f ret;
    // If the absolute value of the z component is less than 1 / √2, 
    // it can be judged to be within 45 degrees to the XY plane
    if (std::abs(v.z()) < Utl::Geo::SQRT_1_D2) {
        ret.x() = -v.y();
        ret.y() = v.x();
        ret.z() = 0.0f;
    }
    // If not, it is determined to exist in the YZ plane
    else {
        ret.x() = 0.0f;
        ret.y() = -v.z();
        ret.z() = v.y();
    }
    // Normalize and return
    return ret.GetNormalize();
}

// Calculate the cross product of 3 dimensional vectors in the XZ plane
float Utl::Math::CrossVector3In2D(const Vector3f& a, const Vector3f& b) {
    return a.x() * b.z() - a.z() * b.x();
}

// Multiply the axis vectors of a 3x3 rotation matrix to a scalar value
Vector3f Utl::Math::MultiplyMat3x3AxisToScalar(const Matrix3x3f& mat, const float& scaler, uint8_t index) {
    assert(Utl::Math::_X <= index && index <= Utl::Math::_Z);
    return Vector3f(mat(Utl::Math::_X, index) * scaler, mat(Utl::Math::_Y, index) * scaler, mat(Utl::Math::_Z, index) * scaler);
}

// Calculate the dot product of a 3 dimensional vector from an axis vector with 3x3 rotation matrix
float Utl::Math::DotMat3x3AxisToVector(const Matrix3x3f& mat, const Vector3f& v, uint8_t index) {
    assert(Utl::Math::_X <= index && index <= Utl::Math::_Z);
    // x * x + y * y + z * z
    return mat(Utl::Math::_X, index) * v.x() + mat(Utl::Math::_Y, index) * v.y() + mat(Utl::Math::_Z, index) * v.z();
}

// Calculate the cross product between the axis vectors of 3x3 rotation matrices
float Utl::Math::DotMat3x3AxisToMat3x3Axis(const Matrix3x3f& matA, const Matrix3x3f& matB, uint8_t indexA, uint8_t indexB) {
    assert(Utl::Math::_X <= indexA && indexA <= Utl::Math::_Z && Utl::Math::_X <= indexB && indexB <= Utl::Math::_Z);
    // x * x + y * y + z * z
    return matA(Utl::Math::_X, indexA) * matB(Utl::Math::_X, indexB) + matA(Utl::Math::_Y, indexA) * matB(Utl::Math::_Y, indexB) + matA(Utl::Math::_Z, indexA) * matB(Utl::Math::_Z, indexB);
}

// Calculate the cross product between the axis vectors of 3x3 rotation matrices
Vector3f Utl::Math::CrossMat3x3AxisToMat3x3Axis(const Matrix3x3f& matA, const Matrix3x3f& matB, uint8_t indexA, uint8_t indexB) {
    assert(Utl::Math::_X <= indexA && indexA <= Utl::Math::_Z && Utl::Math::_X <= indexB && indexB <= Utl::Math::_Z);
    // y * r.z - z * r.y
    // z * r.x - x * r.z
    // x * r.y - y * r.x
    return Vector3f(
        matA(Utl::Math::_Y, indexA) * matB(Utl::Math::_Z, indexB) - matA(Utl::Math::_Z, indexA) * matB(Utl::Math::_Y, indexB),
        matA(Utl::Math::_Z, indexA) * matB(Utl::Math::_X, indexB) - matA(Utl::Math::_X, indexA) * matB(Utl::Math::_Z, indexB),
        matA(Utl::Math::_X, indexA) * matB(Utl::Math::_Y, indexB) - matA(Utl::Math::_Y, indexA) * matB(Utl::Math::_X, indexB));
}

// Advance the current value toward the target value
float Utl::Math::MoveTowards(float currentVal, float targetVal, float absStep) {
    if (Utl::IsEqual(currentVal, targetVal)) {
        return targetVal;
    }
    else if (currentVal < targetVal) {
        float ret = currentVal + absStep;
        return (targetVal < ret) ? targetVal : ret;
    }
    else {
        float ret = currentVal - absStep;
        return (targetVal > ret) ? targetVal : ret;
    }
}

// Advance the current value toward the target value
Vector3f Utl::Math::MoveTowards(const Vector3f& currentVal, const Vector3f& targetVal, float absStep) {
    Vector3f diff = (targetVal - currentVal).GetNormalize();
    Vector3f ret = currentVal + diff * absStep;
    for (uint8_t i = 0; i < 3; ++i) {
        ret[i] = (currentVal[i] > targetVal[i]) ? (std::max)(ret[i], targetVal[i]) : (std::min)(ret[i], targetVal[i]);
    }
    return ret;
}

// Convert DirectX::XMFLOAT3 to Vector3f
Vector3f Utl::Math::ToVector3f(const DirectX::XMFLOAT3& v) { 
    return Vector3f(v.x, v.y, v.z); 
}

// Convert DirectX::XMVECTOR to Vector3f
Vector3f Utl::Math::ToVector3f(const DirectX::XMVECTOR& v) {
    DirectX::XMFLOAT3 xmFloat;
    DirectX::XMStoreFloat3(&xmFloat, v);
    return Vector3f(xmFloat.x, xmFloat.y, xmFloat.z);
}

// Convert Vector3f to DirectX::XMVECTOR
DirectX::XMVECTOR Utl::Math::ToXMVECTOR(const Vector3f& v, float w) {
    return DirectX::XMVectorSet(v.x(), v.y(), v.z(), w);
}

// Simple judgment of all elements of the unit vector are 0 or not
bool Utl::Math::IsUnitVector3fZero(const Vector3f& v) {
    return Utl::IsFloatZero(v.x() * v.x() + v.y() * v.y() + v.z() * v.z());
}

// Are all the values of the 3 dimensional vectors zero?
bool Utl::Math::IsVector3fZero(const Vector3f& v) {
    return Utl::IsFloatZero(v.x()) && Utl::IsFloatZero(v.y()) && Utl::IsFloatZero(v.z());
}

// Are the two vectors almost the same value?
bool Utl::Math::IsEqualVector3f(const Vector3f& a, const Vector3f& b) {
    return Utl::IsEqual(a.x(), b.x()) && Utl::IsEqual(a.y(), b.y()) && Utl::IsEqual(a.z(), b.z());
}

// Are the two quaternions almost the same value?
bool Utl::Math::IsEqualQuaternionf(const Quaternionf& a, const Quaternionf& b) {
    return Utl::IsEqual(a.x(), b.x()) && Utl::IsEqual(a.y(), b.y())
        && Utl::IsEqual(a.z(), b.z()) && Utl::IsEqual(a.w(), b.w());
}
