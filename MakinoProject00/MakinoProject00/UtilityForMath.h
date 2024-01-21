/**
 * @file   UtilityForMath.h
 * @brief  This file provides utility in handling mathematics.
 * 
 * @author Shodai Makino
 * @date   2023/12/6
 */

#ifndef __UTILITY_FOR_MATH_H__
#define __UTILITY_FOR_MATH_H__

#include "MathStructures.h"
#include "UtilityForBit.h"

 /** @brief 2 dimensional vector */
using Vector2f = Vector<float, 2>;
/** @brief 3 dimensional vector */
using Vector3f = Vector<float, 3>;
/** @brief 4 dimensional vector */
using Vector4f = Vector<float, 4>;
/** @brief Vector for color */
using Colorf = Vector<float, 4>;
/** @brief Vector for barycentric coordinate */
using Barycentric3f = Vector<float, 3>;

/** @brief 3x3 matrix */
using Matrix3x3f = Matrix<float, 3, 3>;
/** @brief 4x4 matrix */
using Matrix4x4f = Matrix<float, 4, 4>;

/** @brief Quaternion */
using Quaternionf = Quaternion<float>;

/** @brief Transform that has position and scale, rotation */
struct Transformf {
    /** @brief Position */
    Vector3f pos;
    /** @brief Scale */
    Vector3f scale;
    /** @brief Rotation */
    Quaternionf rotation;

    /**
       @brief Constructor
       @param pos Position
       @param scale Scale
       @param angle Eular angle
    */
    Transformf(const Vector3f& pos = Vector3f::Zero(),
        const Vector3f& scale = Vector3f::Ones(),
        const Vector3f& angle = Vector3f::Zero())
        : pos(pos)
        , scale(scale)
        , rotation(angle)
    {}

    /**
       @brief Constructor
       @param pos Position
       @param scale Scale
       @param rotate Quaternion
    */
    Transformf(const Vector3f& pos,
        const Vector3f& scale,
        const Quaternionf& rotate)
        : pos(pos)
        , scale(scale)
        , rotation(rotate)
    {}

    /**
       @brief Convert position existing in world coordinate space to its own local coordinate space
       @param worldPos position to be converted
       @return Converted position
    */
    Vector3f ConvertWorld2Local(const Vector3f& worldPos) const {
        return rotation.GetConjugation() * (worldPos - pos);
    }

    /**
       @brief Convert position existing in its own local coordinate space to world coordinate space
       @param worldPos position to be converted
       @return Converted position
    */
    Vector3f ConvertLocal2World(const Vector3f& localPos) const {
        return (rotation * localPos) + pos;
    }

    /** @brief Get the forward vector of this transform */
    Vector3f GetForward() const;
    /** @brief Get the backward vector of this transform */
    Vector3f GetBackward() const;
    /** @brief Get the right vector of this transform */
    Vector3f GetRight() const;
    /** @brief Get the left vector of this transform */
    Vector3f GetLeft() const;
    /** @brief Get the up vector of this transform */
    Vector3f GetUp() const;
    /** @brief Get the down vector of this transform */
    Vector3f GetDown() const;

    /** @brief Copy constructor */
    Transformf(const Transformf& other) = default;
    /** @brief Copy assignment operator */
    Transformf& operator=(const Transformf& other) = default;
    /** @brief Move constructor */
    Transformf(Transformf&& other) = default;
    /** @brief Move assignment operator */
    Transformf& operator=(Transformf&& other) = default;
};

namespace Utl {
    /** @brief For mathematics */
    namespace Math {
        /** @brief first element index */
        const uint8_t _X = 0;
        /** @brief second element index */
        const uint8_t _Y = 1;
        /** @brief third element index */
        const uint8_t _Z = 2;
        /** @brief fourth element index */
        const uint8_t _W = 3;

        /** @brief Red element index */
        const uint8_t _R = 0;
        /** @brief Green element index */
        const uint8_t _G = 1;
        /** @brief Blue element index */
        const uint8_t _B = 2;
        /** @brief Alpha element index */
        const uint8_t _A = 3;

        /** @brief 3 dimensional X unit vector */
        const Vector3f UNIT3_RIGHT = Vector3f(1.0f, 0.0f, 0.0f);
        /** @brief 3 dimensional X unit vector */
        const Vector3f UNIT3_LEFT = Vector3f(-1.0f, 0.0f, 0.0f);
        /** @brief 3 dimensional Y unit vector */
        const Vector3f UNIT3_UP = Vector3f(0.0f, 1.0f, 0.0f);
        /** @brief 3 dimensional Y unit vector */
        const Vector3f UNIT3_DOWN = Vector3f(0.0f, -1.0f, 0.0f);
        /** @brief 3 dimensional Z unit vector */
        const Vector3f UNIT3_FORWARD = Vector3f(0.0f, 0.0f, 1.0f);
        /** @brief 3 dimensional Z unit vector */
        const Vector3f UNIT3_BACKWARD = Vector3f(0.0f, 0.0f, -1.0f);

        /**
           @brief Linear interpolation
           @param a Interpolation start vector
           @param b Interpolation end vector
           @param t Parameter (0 to 1)
        */
        Vector3f Lerp(const Vector3f& a, const Vector3f& b, float t);

        /**
           @brief Spherical linear interpolation
           @param a Interpolation start quaternion
           @param b Interpolation end quaternion
           @param t Parameter (0 to 1)
        */
        Quaternionf Slerp(const Quaternionf& a, const Quaternionf& b, float t);

        /**
           @brief Calculate quaternion to rotate vector A to vector B
           @param aNorm Vector A (unit vector)
           @param bNorm Vector B (unit vector)
           @return Calculated quaternion
           @attention
           Be sure to pass a unit vector. No internal normalization is performed to improve computational efficiency
        */
        Quaternionf VectorToVectorQuaternion(const Vector3f& a, const Vector3f& b);

        /**
           @brief Calculate and return tangent vector
           @param v Unit vector
           @return Tangent vector
           @attention
           Be sure to pass a unit vector. No internal normalization is performed to improve computational efficiency
        */
        Vector3f GetTangentVector(const Vector3f& v);

        /**
           @brief Calculate the cross product of 3 dimensional vectors in the XZ plane
           @param a Vector A
           @param b Vector B
           @param Result
        */
        float CrossVector3In2D(const Vector3f& a, const Vector3f& b);

        /**
           @brief Multiply the axis vectors of a 3x3 rotation matrix to a scalar value
           @param mat Matrix to be multiplied
           @param scalar Scalar to be multiplied
           @param index Index of axis vector
           @return Result vector
        */
        Vector3f MultiplyMat3x3AxisToScalar(const Matrix3x3f& mat, const float& scalar, uint8_t index);

        /**
           @brief Calculate the dot product of a 3 dimensional vector from an axis vector with 3x3 rotation matrix
           @param mat Matrix to be calculated
           @param v Vector to be calculated
           @param index Index of axis vector
           @return Result of dot product
        */
        float DotMat3x3AxisToVector(const Matrix3x3f& mat, const Vector3f& v, uint8_t index);

        /**
           @brief Calculate the dot product between the axis vectors of 3x3 rotation matrices
           @param matA Matrix A
           @param matB Matrix B
           @param indexA Index of axis vectorA
           @param indexB Index of axis vectorB
           @return Result of dot product
        */
        float DotMat3x3AxisToMat3x3Axis(const Matrix3x3f& matA, const Matrix3x3f& matB, uint8_t indexA, uint8_t indexB);

        /**
           @brief Calculate the cross product between the axis vectors of 3x3 rotation matrices
           @param matA Matrix A
           @param matB Matrix B
           @param indexA Index of axis vectorA
           @param indexB Index of axis vectorB
           @return Result of cross product
        */
        Vector3f CrossMat3x3AxisToMat3x3Axis(const Matrix3x3f& matA, const Matrix3x3f& matB, uint8_t indexA, uint8_t indexB);

        /**
           @brief Advance the current value toward the target value
           @param currentVal The current value
           @param maxVal The target value
           @param step Step one's advance (absolute value)
           @details
           If the current value is greater than the target value, subtract, if less, add
        */
        float MoveTowards(float currentVal, float targetVal, float absStep);

        /**
           @brief Advance the current value toward the target value
           @param currentVal The current value
           @param maxVal The target value
           @param step Step one's advance (absolute value)
           @details
           If the current value is greater than the target value, subtract, if less, add
        */
        Vector3f MoveTowards(const Vector3f& currentVal, const Vector3f& targetVal, float absStep);

        /** @brief Simple judgment of all elements of the unit vector are 0 or not */
        bool IsUnitVector3fZero(const Vector3f& v);

        /** @brief Are all the values of the 3 dimensional vectors zero? */
        bool IsVector3fZero(const Vector3f& v);
        /** @brief Are the two 3 dimensional vectors almost the same value? */
        bool IsEqualVector3f(const Vector3f& a, const Vector3f& b);

        /** @brief Are the two quaternions almost the same value? */
        bool IsEqualQuaternionf(const Quaternionf& a, const Quaternionf& b);
    } // namespace Math
} // namespace Utl

#endif // !__UTILITY_FOR_MATH_H__
