/**
 * @file   MathStructures.h
 * @brief  This file handles mathematical structures such as vectors, matrices and quaternions.
 *
 * @author Shodai Makino
 * @date   2023/12/4
 */

#ifndef __MATH_STRUCTURES_H__
#define __MATH_STRUCTURES_H__

#include "UtilityForType.h"
#include "Utility.h"

 // Forward declarations
template <Utl::Type::Traits::IsFloatingPoint T, uint8_t ROW, uint8_t COLUMN>
struct Matrix;
template<Utl::Type::Traits::IsFloatingPoint T>
struct Quaternion;

/** @brief Colomn vector structure */
template <Utl::Type::Traits::IsFloatingPoint T, uint8_t SIZE>
struct Vector {
    static_assert(SIZE >= 2, "Vector size is too small");

public:
    /**
       @brief Constructor
       @param args Initialization value
    */
    template<Utl::Type::Traits::IsFloatingPoint... Args>
    Vector(Args... args) : vals{ args... } { static_assert(sizeof...(args) <= SIZE, "Too many arguments provided to Vector constructor"); }

    /** @brief Copy constructor */
    Vector(const Vector& copy) { for (uint8_t i = 0; i < SIZE; ++i) { vals[i] = copy[i]; } }
    /** @brief Copy assignment operator */
    Vector& operator=(const Vector& copy) { for (uint8_t i = 0; i < SIZE; ++i) { vals[i] = copy[i]; } return *this; }

    /** @brief Move constructor */
    Vector(Vector&& other) noexcept { for (uint8_t i = 0; i < SIZE; ++i) { vals[i] = std::move(other.vals[i]); } }
    /** @brief Move assignment operator */
    Vector& operator=(Vector&& other) noexcept { if (this != &other) { for (uint8_t i = 0; i < SIZE; ++i) { vals[i] = std::move(other.vals[i]); } } return *this; }

    /** @brief Set from different size */
    template<uint8_t OTHER_SIZE, Utl::Type::Traits::IsFloatingPoint... Args>
    void SetFromDiffSize(const Vector<T, OTHER_SIZE>& copy, Args... args);

    /** @brief Dot product */
    const T Dot(const Vector& other) const {
        T ret = T(0);
        for (uint8_t i = 0; i < SIZE; ++i) { ret += vals[i] * other[i]; }
        return ret;
    }

    /** @brief Calculate vector length squared */
    const T LengthSq() const {
        return Dot(*this);
    }

    /** @brief Calculate vector length */
    const T Length() const {
        return std::sqrt(LengthSq());
    }

    /** @brief Get normalized this vector */
    const Vector GetNormalize() const {
        const T len = Length();
        if (len > T(0)) {
            const T invLen = T(1) / len;
            Vector ret;
            for (uint8_t i = 0; i < SIZE; ++i) { ret[i] = vals[i] * invLen; }
            return ret;
        }
        return Vector::Zero();
    }

    /** @brief Normalize this vector */
    void Normalize() {
        const T len = Length();
        if (len > T(0)) {
            const T invLen = T(1) / len;
            for (uint8_t i = 0; i < SIZE; ++i) { vals[i] *= invLen; }
        }
    }

    /** @brief Cross product for 2 dimensions */
    const T Cross(const Vector& other) const requires (SIZE == 2) { return vals[0] * other[1] - vals[1] * other[0]; }

    /** @brief Cross product for 3 dimensions */
    const Vector Cross(const Vector& other) const requires (SIZE == 3) {
        return Vector(
            this->vals[1] * other.vals[2] - this->vals[2] * other.vals[1],
            this->vals[2] * other.vals[0] - this->vals[0] * other.vals[2],
            this->vals[0] * other.vals[1] - this->vals[1] * other.vals[0]
        );
    }

    /** @brief Create a matrix with the elements of this vector as diagonal components */
    const Matrix<T, SIZE, SIZE> GetDiagonalMatrix() const;

    /**
       @brief Calculate the angle of rotation from this vector to another vector in 2D, ignoring one axis
       @param v Another vector
       @return Radian angle [-Pi, Pi]
       @attention
       Be sure to make both vectors unit vectors.
       No internal normalization is performed to improve computational efficiency.
       The value of the axis to be ignored must be 0.
    */
    float AngleTo(const Vector& v, uint8_t ignoreAxis) const requires (SIZE == 3);

    /** @brief Get pointer to array of elements */
    const T* GetArrayPtr() const { return vals; }
    /** @brief Get pointer to array of elements */
    T* GetArrayPtr() { return vals; }

    /** @brief Get the first element */
    T& x() { return vals[0]; }
    /** @brief Get the second element */
    T& y() { return vals[1]; }
    /** @brief Get the third element */
    T& z() requires (SIZE >= 3) { return vals[2]; }
    /** @brief Get the fourth element */
    T& w() requires (SIZE >= 4) { return vals[3]; }
    /** @brief Get the first element */
    const T x() const { return vals[0]; }
    /** @brief Get the second element */
    const T y() const { return vals[1]; }
    /** @brief Get the third element */
    const T z() const requires (SIZE >= 3) { return vals[2]; }
    /** @brief Get the fourth element */
    const T w() const requires (SIZE >= 4) { return vals[3]; }


    /** @brief Get the element for red */
    T& r() { return vals[0]; }
    /** @brief Get the element for green */
    T& g() { return vals[1]; }
    /** @brief Get the element for blue */
    T& b() requires (SIZE >= 3) { return vals[2]; }
    /** @brief Get the element for alpha */
    T& a() requires (SIZE >= 4) { return vals[3]; }
    /** @brief Get the element for red */
    const T r() const { return vals[0]; }
    /** @brief Get the element for green */
    const T g() const { return vals[1]; }
    /** @brief Get the element for blue */
    const T b() const requires (SIZE >= 3) { return vals[2]; }
    /** @brief Get the element for alpha */
    const T a() const requires (SIZE >= 4) { return vals[3]; }


    /** @brief Get the first element for barycentric coordinate */
    T& u() requires (SIZE == 3) { return vals[0]; }
    /** @brief Get the second element for barycentric coordinate */
    T& v() requires (SIZE == 3) { return vals[1]; }
    /** @brief Get the third element for barycentric coordinate */
    T& w() requires (SIZE == 3) { return vals[2]; }
    /** @brief Get the first element for barycentric coordinate */
    const T u() const requires (SIZE == 3) { return vals[0]; }
    /** @brief Get the second element for barycentric coordinate */
    const T v() const requires (SIZE == 3) { return vals[1]; }
    /** @brief Get the third element for barycentric coordinate */
    const T w() const requires (SIZE == 3) { return vals[2]; }


    /** @brief Index operator */
    T& operator[](uint8_t index) { return vals[index]; }
    /** @brief Index operator */
    const T operator[](uint8_t index) const { return vals[index]; }

    /** @brief Unary negation operator */
    Vector operator-() const { Vector ret; for (uint8_t i = 0; i < SIZE; ++i) { ret[i] = -vals[i]; } return ret; }

    /** @brief Addition and assignment operator for vector */
    Vector& operator+=(const Vector& other) { for (uint8_t i = 0; i < SIZE; ++i) { vals[i] += other[i]; } return *this; }
    /** @brief Subtraction and assignment operator for vector */
    Vector& operator-=(const Vector& other) { for (uint8_t i = 0; i < SIZE; ++i) { vals[i] -= other[i]; } return *this; }
    /** @brief Multiplication and assignment operator for vector */
    Vector& operator*=(const Vector& other) { for (uint8_t i = 0; i < SIZE; ++i) { vals[i] *= other[i]; } return *this; }
    /** @brief Divison and assignment operator for vector */
    Vector& operator/=(const Vector& other) { for (uint8_t i = 0; i < SIZE; ++i) { vals[i] /= other[i]; } return *this; }
    /** @brief Addition operator for vector */
    const Vector operator+(const Vector& other) const { Vector ret; for (uint8_t i = 0; i < SIZE; ++i) { ret[i] = vals[i] + other[i]; } return ret; }
    /** @brief Subtraction operator for vector */
    const Vector operator-(const Vector& other) const { Vector ret; for (uint8_t i = 0; i < SIZE; ++i) { ret[i] = vals[i] - other[i]; } return ret; }
    /** @brief Multiplication operator for vector */
    const Vector operator*(const Vector& other) const { Vector ret; for (uint8_t i = 0; i < SIZE; ++i) { ret[i] = vals[i] * other[i]; } return ret; }
    /** @brief Divison operator for vector */
    const Vector operator/(const Vector& other) const { Vector ret; for (uint8_t i = 0; i < SIZE; ++i) { ret[i] = vals[i] / other[i]; } return ret; }

    /** @brief Addition and assignment operator for scalar */
    Vector& operator+=(T scalar) { for (uint8_t i = 0; i < SIZE; ++i) { vals[i] += scalar; } return *this; }
    /** @brief Subtraction and assignment operator for scalar */
    Vector& operator-=(T scalar) { for (uint8_t i = 0; i < SIZE; ++i) { vals[i] -= scalar; } return *this; }
    /** @brief Multiplication and assignment operator for scalar */
    Vector& operator*=(T scalar) { for (uint8_t i = 0; i < SIZE; ++i) { vals[i] *= scalar; } return *this; }
    /** @brief Divison and assignment operator for scalar */
    Vector& operator/=(T scalar) { for (uint8_t i = 0; i < SIZE; ++i) { vals[i] /= scalar; } return *this; }
    /** @brief Addition operator for scalar */
    const Vector operator+(T scalar) const { Vector ret; for (uint8_t i = 0; i < SIZE; ++i) { ret[i] = vals[i] + scalar; } return ret; }
    /** @brief Subtraction operator for scalar */
    const Vector operator-(T scalar) const { Vector ret; for (uint8_t i = 0; i < SIZE; ++i) { ret[i] = vals[i] - scalar; } return ret; }
    /** @brief Multiplication operator for scalar */
    const Vector operator*(T scalar) const { Vector ret; for (uint8_t i = 0; i < SIZE; ++i) { ret[i] = vals[i] * scalar; } return ret; }
    /** @brief Divison operator for scalar */
    const Vector operator/(T scalar) const { Vector ret; for (uint8_t i = 0; i < SIZE; ++i) { ret[i] = vals[i] / scalar; } return ret; }

    /** @brief Get vector with all 0 values */
    static const Vector& Zero() { return ZERO; }
    /** @brief Get vector with all 1 values */
    static const Vector& Ones() { return ONES; }

private:
    /**
       @brief Create a vector whose elements are all filled with the same value
       @param value Initialization value
       @return The created vector
    */
    static const Vector CreateFillVector(T value) {
        Vector ret;
        for (uint8_t i = 0; i < SIZE; ++i) { ret[i] = value; }
        return ret;
    }

protected:
    /** @brief Vector Elements */
    T vals[SIZE];

    /** @brief Vector with all 0 values for each sizes */
    static const Vector ZERO;
    /** @brief Vector with all 1 values for each sizes */
    static const Vector ONES;
};

// Initialize vector with all 0 values for each sizes
template <Utl::Type::Traits::IsFloatingPoint T, uint8_t SIZE>
const Vector<T, SIZE> Vector<T, SIZE>::ZERO = Vector::CreateFillVector(T(0));
// Initialize vector with all 1 values for each sizes
template <Utl::Type::Traits::IsFloatingPoint T, uint8_t SIZE>
const Vector<T, SIZE> Vector<T, SIZE>::ONES = Vector::CreateFillVector(T(1));


/** @brief Column-major matrix */
template <Utl::Type::Traits::IsFloatingPoint T, uint8_t ROW, uint8_t COLUMN>
struct Matrix {
    static_assert(ROW >= 2 && COLUMN >= 2, "Matrix size is too small");

public:
    /**
       @brief Constructor
       @param args Initialization value
    */
    template<Utl::Type::Traits::IsFloatingPoint... Args>
    Matrix(Args... args) {
        static_assert(sizeof...(args) != ROW * COLUMN || sizeof...(args) != 0, "Matrix constructors are only allowed to 'initialize all' or 'initialize none'.");
        FillOrderMatrix(0, args...);
    }

    /** @brief Copy constructor */
    Matrix(const Matrix& copy) { memcpy(&vals[0], &copy.vals[0], ROW * COLUMN * sizeof(T)); }
    /** @brief Copy operator */
    Matrix& operator=(const Matrix& copy) { memcpy(&vals[0], &copy.vals[0], ROW * COLUMN * sizeof(T)); return *this; }

    /** @brief Get a transpose matrix */
    Matrix Transpose() {
        Matrix ret;
        for (uint8_t i = 0; i < ROW; ++i) {
            for (uint8_t j = 0; j < COLUMN; ++j) {
                ret(j, i) = (*this)(i, j);
            }
        }
        return ret;
    }

    /** @brief Get a inverse matrix */
    Matrix Inverse();

    /** @brief Element access operator */
    T& operator()(uint8_t row, uint8_t column) { return vals[column * ROW + row]; }
    /** @brief Element access operator */
    const T operator()(uint8_t row, uint8_t column) const { return vals[column * ROW + row]; }

    /** @brief Product with matrix */
    template<uint8_t OTHER_COLUMN>
    const Matrix<T, ROW, OTHER_COLUMN> operator*(const Matrix<T, COLUMN, OTHER_COLUMN>& other) const {
        Matrix<T, ROW, OTHER_COLUMN> ret;
        for (uint8_t i = 0; i < ROW; ++i) {
            for (uint8_t j = 0; j < OTHER_COLUMN; ++j) {
                ret(i, j) = 0;
                for (uint8_t k = 0; k < COLUMN; ++k) {
                    ret(i, j) += (*this)(i, k) * other(k, j);
                }
            }
        }
        return ret;
    }

    /** @brief Product with column vector */
    const Vector<T, COLUMN> operator*(const Vector<T, COLUMN>& vec) const {
        Vector<T, ROW> ret;
        for (uint8_t i = 0; i < ROW; ++i) {
            for (uint8_t j = 0; j < COLUMN; ++j) {
                ret[i] += (*this)(i, j) * vec[j];
            }
        }
        return ret;
    }

    /** @brief Get a identity matrix */
    static const Matrix& Identity() { return IDENTITY; }

private:
    /** @brief Create a matrix whose elements are filled in order (empty function) */
    void FillOrderMatrix(std::uint8_t index) {}

    /**
       @brief Create a matrix whose elements are filled in order
       @param index Element number
       @param firstArg First assignment value
       @param remainingArgs Remaining values
    */
    template <typename FirstArg, typename... RemainingArgs>
    void FillOrderMatrix(std::uint8_t index, FirstArg firstArg, RemainingArgs... remainingArgs) {
        // Set values to be contiguous per column
        uint8_t row = index % COLUMN;
        uint8_t col = index / COLUMN;
        vals[row * ROW + col] = firstArg;
        FillOrderMatrix(index + 1, remainingArgs...);
    }

    /** @brief Create a matrix for identity matrix */
    static Matrix CreateFillIdentityMatrix() {
        Matrix ret;
        for (uint8_t i = 0; i < ROW; ++i) {
            for (uint8_t j = 0; j < COLUMN; ++j) {
                ret(i, j) = (i == j) ? T(1) : T(0);
            }
        }
        return ret;
    }

protected:
    /** @brief Matrix Elements */
    T vals[ROW * COLUMN];

    /** @brief Identity matrices for each sizes */
    static const Matrix IDENTITY;
};
// Initialize identity matrices for each sizes
template <Utl::Type::Traits::IsFloatingPoint T, uint8_t ROW, uint8_t COLUMN>
const Matrix<T, ROW, COLUMN> Matrix<T, ROW, COLUMN>::IDENTITY = Matrix::CreateFillIdentityMatrix();

/** @brief Quaternion */
template<Utl::Type::Traits::IsFloatingPoint T>
struct Quaternion {
public:
    /**
       @brief Constructor for identity matrix
    */
    Quaternion() : vals{ T(0), T(0), T(0), T(1) } {}

    /**
       @brief Constructor
       @param x Imaginary part 1
       @param y Imaginary part 2
       @param z Imaginary part 3
       @param w Real part
    */
    Quaternion(T x, T y, T z, T w) : vals{ x, y, z, w } {}

    /** @brief Copy constructor */
    Quaternion(const Quaternion& copy) { for (uint8_t i = 0; i < 4; ++i) { vals[i] = copy.vals[i]; } }
    /** @brief Copy assignment operator */
    Quaternion& operator=(const Quaternion& copy) { for (uint8_t i = 0; i < 4; ++i) { vals[i] = copy.vals[i]; } return *this; }

    /** @brief Move constructor */
    Quaternion(Quaternion&& other) { for (uint8_t i = 0; i < 4; ++i) { vals[i] = std::move(other.vals[i]); } }
    /** @brief Move assignment operator */
    Quaternion& operator=(Quaternion&& other) { if (this != &other) { for (uint8_t i = 0; i < 4; ++i) { vals[i] = std::move(other.vals[i]); } } return *this; }

    /**
       @brief Constructor
       @param rad Rotation angle for radian
       @param axis Axis of rotation (unit vector)
    */
    Quaternion(T rad, const Vector<T, 3>& axis);

    /**
       @brief Constructor
       @param eulerAngle Euler angles
    */
    Quaternion(const Vector<T, 3>& eulerAngle);

    /**
       @brief Normalize this quaternion
    */
    Quaternion GetNormalize() {
        Quaternion ret = *this;
        T invNorm = 1 / std::sqrt(x() * x() + y() * y() + z() * z() + w() * w());
        ret.x() *= invNorm;
        ret.y() *= invNorm;
        ret.z() *= invNorm;
        ret.w() *= invNorm;
        return ret;
    }

    /**
       @brief Normalize this quaternion
    */
    void Normalize() {
        T invNorm = 1 / std::sqrt(x() * x() + y() * y() + z() * z() + w() * w());
        x() *= invNorm;
        y() *= invNorm;
        z() *= invNorm;
        w() *= invNorm;
    }

    /**
       @brief Calculate and apply the angle of rotation around the axis from the current posture
       @param rad Rotation angle for radian
       @param axis Axis of rotation
    */
    void AngleAxis(T rad, const Vector<T, 3>& axis) {
        Quaternion self(rad, axis);
        *this *= self;
    }

    /**
       @brief Get Eular angle from this quaternion
       @return Eular angle
    */
    const Vector<T, 3> GetEularAngle() const;

    /**
       @brief Get rotation matrix from this quaternion
       @return Rotation matrix
    */
    const Matrix<T, 3, 3> GetMatrix() const;

    /**
       @brief Get angular velocity from this quaternion
       @return Angular velocity
    */
    Vector<T, 3> GetAngularVelocity() const;

    /**
       @brief Get a conjugate quaternion from this quaternion
       @return Conjugate quaternion
       @note Conjugate quaternions represent reverse rotations
    */
    Quaternion GetConjugation() const {
        return Quaternion(-x(), -y(), -z(), w());
    }

    /** @brief Get the first element */
    T& x() { return vals[0]; }
    /** @brief Get the second element */
    T& y() { return vals[1]; }
    /** @brief Get the third element */
    T& z() { return vals[2]; }
    /** @brief Get the fourth element */
    T& w() { return vals[3]; }
    /** @brief Get the first element */
    const T x() const { return vals[0]; }
    /** @brief Get the second element */
    const T y() const { return vals[1]; }
    /** @brief Get the third element */
    const T z() const { return vals[2]; }
    /** @brief Get the fourth element */
    const T w() const { return vals[3]; }

    /**
       @brief Quaternion product operator
       @param rhs The value that is on the right-hand side in the quaternion product
       @return Result of quaternion product
    */
    const Quaternion operator*(const Quaternion& rhs) const;

    /**
       @brief Apply another quaternion rotation to this quaternion
       @param lhs The value that is on the left-hand side in the quaternion product
       @return Result of quaternion product
       @attention The order is reversed from the usual quaternion product
    */
    Quaternion& operator*=(const Quaternion& lhs);

    /**
       @brief Apply this quaternion rotation to a 3 dimensional vector
       @param v Vector to be applied rotation
       @return Rotated vector
    */
    const Vector<T, 3> operator*(const Vector<T, 3>& v) const;

    /** @brief Index operator */
    T& operator[](uint8_t index) { return vals[index]; }
    /** @brief Index operator */
    const T operator[](uint8_t index) const { return vals[index]; }

private:
    /**
       @brief Quaternion product
       @param lhs The value that is on the left-hand side in the quaternion product
       @param rhs The value that is on the right-hand side in the quaternion product
       @return Quaternion of result
    */
    inline static Quaternion Product(const Quaternion& lhs, const Quaternion& rhs);

private:
    /** @brief [3] is a real part */
    T vals[4];
};

// Set from different size
template<Utl::Type::Traits::IsFloatingPoint T, uint8_t SIZE>
template<uint8_t OTHER_SIZE, Utl::Type::Traits::IsFloatingPoint... Args>
void Vector<T, SIZE>::SetFromDiffSize(const Vector<T, OTHER_SIZE>& copy, Args... args) {
    static_assert(sizeof...(args) <= (SIZE - OTHER_SIZE), "Too many arguments provided to Vector constructor");

    if constexpr (SIZE <= OTHER_SIZE) {
        for (uint8_t i = 0; i < SIZE; ++i) {
            vals[i] = copy[i];
        }
    }
    else {
        uint8_t i = 0;
        for (; i < OTHER_SIZE; ++i) {
            vals[i] = copy[i];
        }

        (..., (vals[i++] = args));
    }
}

/** @brief float * Vector operator */
template<Utl::Type::Traits::IsFloatingPoint T, uint8_t SIZE>
Vector<T, SIZE> operator*(const T scalar, const Vector<T, SIZE>& vec) {
    Vector<T, SIZE> ret;
    for (uint8_t i = 0; i < SIZE; ++i) {
        ret[i] = scalar * vec[i];
    }
    return ret;
}

// Create a matrix with the elements of this vector as diagonal components
template<Utl::Type::Traits::IsFloatingPoint T, uint8_t SIZE>
const Matrix<T, SIZE, SIZE> Vector<T, SIZE>::GetDiagonalMatrix() const {
    Matrix<T, SIZE, SIZE> mat;
    for (uint8_t i = 0; i < SIZE; ++i) {
        for (uint8_t j = 0; j < SIZE; ++j) {
            mat(i, j) = (i == j) ? vals[i] : T(0);
        }
    }
    return mat;
}

// Calculate the angle of rotation from this vector to another vector in 2D, ignoring one axis
template<Utl::Type::Traits::IsFloatingPoint T, uint8_t SIZE>
float Vector<T, SIZE>::AngleTo(const Vector& v, uint8_t ignoreAxis) const requires(SIZE == 3) {
    // Calculate dot product and cross product
    T d = Utl::Clamp(Dot(v), T(-1), T(1));

    T epsilon = std::numeric_limits<T>::epsilon();
    // Vectors are parallel
    if (std::abs(d - T(1)) < epsilon) {
        return T(0);
    }
    // Vectors are antiparallel
    else if (std::abs(d + T(1)) < epsilon) {
        return Utl::GetPI<T>();
    }
    // Calculate angle normally
    else {
        // Convert [0, Pi] to [-Pi, Pi]
        Vector c = Cross(v);
        if (c[ignoreAxis] < T(0))
            return -std::acos(d);
        else
            return std::acos(d);
    }
}

// Get a inverse matrix
template<Utl::Type::Traits::IsFloatingPoint T, uint8_t ROW, uint8_t COLUMN>
Matrix<T, ROW, COLUMN> Matrix<T, ROW, COLUMN>::Inverse() {
    static_assert(ROW == COLUMN, "Inverse only defined for square matrix.");

    // Create own copy matrix and unit matrix
    Matrix copy = *this;
    Matrix inv = Identity();

    T tmp;
    for (uint8_t i = 0; i < ROW; ++i) {
        // Partial Pivot Selection
        // Bring the element in i column, with the highest absolute value to the diagonal component
        // #NOTE : Since the operation is finished up to the i - 1 row, 
        //         the identity matrix is collapsed if the rows are swapped.
        T maxAbs = std::abs(copy(i, i));
        for (uint8_t row = i; row < ROW; ++row) {
            // If the absolute value of it is highest than maxAbs
            tmp = std::abs(copy(row, i));
            if (maxAbs < tmp) {
                maxAbs = tmp;
                // Replace all elements in one row
                for (uint8_t col = 0; col < COLUMN; ++col) {
                    tmp = copy(i, col);
                    copy(i, col) = copy(row, col);
                    copy(row, col) = tmp;

                    // Perform the same operation
                    tmp = inv(i, col);
                    inv(i, col) = inv(row, col);
                    inv(row, col) = tmp;
                }
            }
        }

        // Perform normalization operation on all columns of i row
        T invDiagonal = T(1) / copy(i, i);
        // #NOTE : Operation is not needed because the elements are guaranteed to be 0 up to the i - 1 column of i row.
        for (uint8_t col = i; col < COLUMN; ++col) {
            copy(i, col) *= invDiagonal;
        }
        // Perform the same operation
        // #NOTE : This matrix does not aim to be a identity matrix, so it is not guaranteed to be 0 up to the i - 1 column in i row.
        for (uint8_t col = 0; col < COLUMN; ++col) {
            inv(i, col) *= invDiagonal;
        }

        // Calculate to 0 for all elements in a column except for the diagonal component
        for (uint8_t row = 0; row < ROW; ++row) {
            if (row != i) {
                T r = -copy(row, i);

                // Calculate this element to zero
                // #NOTE : Operation is not needed because the elements are guaranteed to be zero up to the 'i - 1' column of i row.
                for (uint8_t col = i; col < COLUMN; ++col) {
                    copy(row, col) += copy(i, col) * r;
                }
                // Perform the same operation
                // #NOTE : This matrix does not aim to be a identity matrix, so it is not guaranteed to be 0 up to the i - 1 column in i row.
                for (uint8_t col = 0; col < COLUMN; ++col) {
                    inv(row, col) += inv(i, col) * r;
                }
            }
        }
    }

    return inv;
}

// Constructor
template<Utl::Type::Traits::IsFloatingPoint T>
Quaternion<T>::Quaternion(T rad, const Vector<T, 3>& axis) {
    // Calculate sin with half angle
    T halfRad = rad * Utl::Inv::Get2<T>();
    T s = std::sin(halfRad);
    // Calculate each components
    x() = axis.x() * s;
    y() = axis.y() * s;
    z() = axis.z() * s;
    w() = std::cos(halfRad);
}

// Constructor
template<Utl::Type::Traits::IsFloatingPoint T>
Quaternion<T>::Quaternion(const Vector<T, 3>& eulerAngle) {
    // Prepare variables needed for calculations
    T cosX = std::cos(eulerAngle.x() * Utl::Inv::Get2<T>());
    T sinX = std::sin(eulerAngle.x() * Utl::Inv::Get2<T>());
    T cosY = std::cos(eulerAngle.y() * Utl::Inv::Get2<T>());
    T sinY = std::sin(eulerAngle.y() * Utl::Inv::Get2<T>());
    T cosZ = std::cos(eulerAngle.z() * Utl::Inv::Get2<T>());
    T sinZ = std::sin(eulerAngle.z() * Utl::Inv::Get2<T>());
    // Calculate each components
    x() = sinX * cosY * cosZ - cosX * sinY * sinZ;
    y() = cosX * sinY * cosZ + sinX * cosY * sinZ;
    z() = cosX * cosY * sinZ - sinX * sinY * cosZ;
    w() = cosX * cosY * cosZ + sinX * sinY * sinZ;
}

// Get Eular angle from this quaternion
template<Utl::Type::Traits::IsFloatingPoint T>
const Vector<T, 3> Quaternion<T>::GetEularAngle() const {
    constexpr T MINUSONE_EPSILON = T(-1) + FLT_EPSILON;
    constexpr T PLUSONE_EPSILON = T(1) - FLT_EPSILON;

    // Prepare variables needed for calculations
    T wy = w() * y();
    T xz = x() * z();

    // Calculate the value to be put into asin
    T forAsin = T(-2) * (xz - wy);

    // If the absolute value of this value is greater than 1, 
    // asin returns an error value and performs a special calculation
    // #NOTE : In this case, gimbal lock has occurred, so rotation can only be expressed as the total angle of roll and yaw.
    //         Set x to 0 and assign the total angle to z

    // If it is negative
    if (forAsin <= MINUSONE_EPSILON) {
        return Vector<T, 3>(
            T(0),
            -Utl::GetPI<T>() * T(0.5),
            T(2) * std::atan2(x(), -y())
        );
    }
    // If it is positive
    else if (forAsin >= PLUSONE_EPSILON) {
        return Vector<T, 3>(
            T(0),
            Utl::GetPI<T>() * T(0.5),
            T(2) * std::atan2(-x(), y())
        );
    }
    else {
        // Prepare the variables needed for the remaining calculations
        T ww = w() * w();
        T wx = w() * x();
        T wz = w() * z();
        T xx = x() * x();
        T xy = x() * y();
        T yy = y() * y();
        T yz = y() * z();
        T zz = z() * z();
        // Convert and return
        return Vector<T, 3>(
            std::atan2(T(2) * (yz + wx), ww - xx - yy + zz), // Roll
            std::asin(forAsin),                              // Pitch
            std::atan2(T(2) * (xy + wz), ww + xx - yy - zz)  // Yaw
        );
    }
}

// Get rotation matrix from this quaternion
template<Utl::Type::Traits::IsFloatingPoint T>
const Matrix<T, 3, 3> Quaternion<T>::GetMatrix() const {
    Matrix<T, 3, 3> mat;
    // Prepare variables needed for calculations
    T xx = x() * x();
    T yy = y() * y();
    T zz = z() * z();
    T xy = x() * y();
    T xz = x() * z();
    T yz = y() * z();
    T wx = w() * x();
    T wy = w() * y();
    T wz = w() * z();
    // Calculate each components
    mat(0, 0) = T(1) - T(2) * (yy + zz);
    mat(0, 1) = T(2) * (xy + wz);
    mat(0, 2) = T(2) * (xz - wy);
    mat(1, 0) = T(2) * (xy - wz);
    mat(1, 1) = T(1) - T(2) * (xx + zz);
    mat(1, 2) = T(2) * (yz + wx);
    mat(2, 0) = T(2) * (xz + wy);
    mat(2, 1) = T(2) * (yz - wx);
    mat(2, 2) = T(1) - T(2) * (xx + yy);
    return mat;
}

// Get angular velocity from this quaternion
template<Utl::Type::Traits::IsFloatingPoint T>
Vector<T, 3> Quaternion<T>::GetAngularVelocity() const {
    // If the denominator is zero, the angular velocity is zero
    T denom = std::sqrt(T(1) - w() * w());
    if (std::abs(denom) <= FLT_EPSILON) { return Vector<T, 3>(0.0f, 0.0f, 0.0f); }

    // Prepare variables needed for calculations
    T angle = T(2) * std::acos(w());
    T invSinHalfAngle = T(1) / denom;
    // Calculate each components
    return Vector<T, 3>(
        angle * x() * invSinHalfAngle,
        angle * y() * invSinHalfAngle,
        angle * z() * invSinHalfAngle
    );
}

// Quaternion product operator
template<Utl::Type::Traits::IsFloatingPoint T>
const Quaternion<T> Quaternion<T>::operator*(const Quaternion& rhs) const {
    Quaternion ret = Product(*this, rhs);
    ret.Normalize();
    return ret;
}

// Apply another quaternion rotation to this quaternion
template<Utl::Type::Traits::IsFloatingPoint T>
Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& lhs) {
    *this = Product(lhs, *this);
    Normalize();
    return *this;
}

// Apply this quaternion rotation to a 3 dimensional vector
template<Utl::Type::Traits::IsFloatingPoint T>
const Vector<T, 3> Quaternion<T>::operator*(const Vector<T, 3>& v) const {
    // Replace 3D vector with quaternion
    Quaternion v2(v.x(), v.y(), v.z(), T(0));
    // Apply this quaternion rotation to replaced 3D vector
    // #NOTE : When the conjugate quaternion is p, 'q * v * p'
    Quaternion vRotate = Product(Product(*this, v2), GetConjugation());
    // Extract x,y,z components
    return Vector<T, 3>(vRotate.x(), vRotate.y(), vRotate.z());
}

// Quaternion product
template<Utl::Type::Traits::IsFloatingPoint T>
inline Quaternion<T> Quaternion<T>::Product(const Quaternion& lhs, const Quaternion& rhs) {
    return Quaternion(
        lhs.w() * rhs.x() - lhs.z() * rhs.y() + lhs.y() * rhs.z() + lhs.x() * rhs.w(),
        lhs.z() * rhs.x() + lhs.w() * rhs.y() - lhs.x() * rhs.z() + lhs.y() * rhs.w(),
        -lhs.y() * rhs.x() + lhs.x() * rhs.y() + lhs.w() * rhs.z() + lhs.z() * rhs.w(),
        -lhs.x() * rhs.x() - lhs.y() * rhs.y() - lhs.z() * rhs.z() + lhs.w() * rhs.w()
    );
}

#endif // !__MATH_STRUCTURES_H__
