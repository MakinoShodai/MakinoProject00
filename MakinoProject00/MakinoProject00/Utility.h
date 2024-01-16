/**
 * @file   Utility.h
 * @brief  This file defines utility functions that are useful for general programs.
 * 
 * @author Shodai Makino
 * @date   2023/11/13
 */

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "UtilityForType.h"

/** @brief Connect tokens to tokens */
#define CONNECT_TOKEN(a, b) INNER_CONNECT_TOKEN(a, b)
/** @brief Connect tokens to tokens (Inner) */
#define INNER_CONNECT_TOKEN(a, b) a##b

/** @brief Convert tokens to literals */
#define CONVERT_TOKEN2LITERAL(x) INNER_CONVERT_TOKEN2LITERAL(x)
/** @brief Convert tokens to literals (Innner) */
#define INNER_CONVERT_TOKEN2LITERAL(x) #x

namespace Utl {
    // Pi
    constexpr float PI = 3.141592653589793f;
    // 2Pi
    constexpr float PI2 = 2 * PI;
    // Converting a degree method to an radian method
    constexpr float DEG_2_RAD = PI / 180.0f;
    // Converting a radian method to an degree method
    constexpr float RAD_2_DEG = 180.0f / PI;
    // Value to ignore as floating point number error
    constexpr float FLOAT_IGNORE = 0.0001f;
    // Pi
    constexpr double PId = 3.1415926535897932384626433832795028841971;
    // 2Pi
    constexpr double PI2d = 2.0 * PId;
    // Converting a degree method to an radian method
    constexpr double DEG_2_RADd = PId / 180.0;
    // Converting a radian method to an degree method
    constexpr double RAD_2_DEGd = 180.0 / PId;

    /** @brief Pi template */
    template<Type::Traits::IsFloatingPoint T>
    T GetPI() { if constexpr (std::is_same_v<T, float>) return PI; if constexpr (std::is_same_v<T, double>) return PId; }
    /** @brief 2Pi template */
    template<Type::Traits::IsFloatingPoint T>
    T GetPI2() { if constexpr (std::is_same_v<T, float>) return PI2; if constexpr (std::is_same_v<T, double>) return PI2d; }
    /** @brief Converting a degree method to an radian method template */
    template<Type::Traits::IsFloatingPoint T>
    T GetDEG_2_RAD() { if constexpr (std::is_same_v<T, float>) return DEG_2_RAD; if constexpr (std::is_same_v<T, double>) return DEG_2_RADd; }
    /** @brief Converting a radian method to an degree method template */
    template<Type::Traits::IsFloatingPoint T>
    T GetRAD_2_DEG() { if constexpr (std::is_same_v<T, float>) return RAD_2_DEG; if constexpr (std::is_same_v<T, double>) return RAD_2_DEGd; }

    /**
       @brief
       @tparam RetType the type of return variable. Integer type only
       @tparam T Type you want to align. Integer type only

       @param size Size you want to align
       @param alignment Alignment size
       @return Alignment value
    */
    template<typename RetType, typename T>
        requires std::is_integral<RetType>::value&& std::is_integral<T>::value
    RetType Align(T size, RetType alignment) {
        return ((RetType)size + (alignment - 1)) & ~(alignment - 1);
    }

    /**
       @brief Check if the value of Float is equal to 0
       @param a Value to be checked
       @return Result
    */
    const bool IsFloatZero(const float a);

    /**
       @brief Check if two values are equal
       @param a Value to be compared
       @param b Value to be compared
       @return Result
    */
    const bool IsEqual(const float a, const float b);

    /**
       @brief Clamp a value in min to max
       @param val Value to be clamped
       @param min Minimum value
       @param max Maximum value
       @return Clamped value
    */
    const float Clamp(const float val, const float min, const float max);

    /**
       @brief Convert value, which is the value of inMin ～ inMax, to outMin ～ outMax
       @param value Value to be converted
       @param inMin Minimum value of input value
       @param inMax Maximum value of input value
       @param outMin Minimum value of output value
       @param outMin Maximum value of output value
    */
    const float ConversionValue(float value, float inMin, float inMax, float outMin, float outMax);

    /**
       @brief Get value sign
       @param value Value for which the sign is to be got
    */
    int Sign(float value);

    /**
       @brief Function to call repeatedly to generate hash values from multiple variables
       @param seed Variable to store the seed value
       @param v Variable used for hash value generation
       @details
       Same process as hash_combine function in boost library
    */
    template <typename T>
    void HashCombine(std::size_t* seed, const T& v) {
        *seed ^= std::hash<T>()(v) + 0x9e3779b9 + (*seed << 6) + (*seed >> 2);
    }

    /** @brief Class to check if it is the main thread */
    class CMainThreadChecker {
    public:
        /**
           @brief Initialize the main thread ID
           @details
           Must be called from the main thread
        */
        static void Initialize() { if (ms_mainThreadID == std::thread::id()) { ms_mainThreadID = std::this_thread::get_id(); } }

        /** @brief Is this the main thread? */
        static inline bool IsMainThread() { return (std::this_thread::get_id() == ms_mainThreadID); }

    private:
        /** @brief ID of the main thread */
        static std::thread::id ms_mainThreadID;
    };

    /** @brief Inverse */
    namespace Inv {
        /** @brief Inverse of 2 */
        constexpr float _2 = 1.0f / 2.0f;
        /** @brief Inverse of 3 */
        constexpr float _3 = 1.0f / 3.0f;
        /** @brief Inverse of 4 */
        constexpr float _4 = 1.0f / 4.0f;
        /** @brief Inverse of 2 */
        constexpr double _2d = 1.0 / 2.0;
        /** @brief Inverse of 3 */
        constexpr double _3d = 1.0 / 3.0;
        /** @brief Inverse of 4 */
        constexpr double _4d = 1.0 / 4.0;

        /** @brief Inverse of 2 template */
        template<Type::Traits::IsFloatingPoint T>
        T Get2() { if constexpr (std::is_same_v<T, float>) return _2; if constexpr (std::is_same_v<T, double>) return _2d; }
        /** @brief Inverse of 3 template */
        template<Type::Traits::IsFloatingPoint T>
        T Get3() { if constexpr (std::is_same_v<T, float>) return _3; if constexpr (std::is_same_v<T, double>) return _3d; }
        /** @brief Inverse of 4 template */
        template<Type::Traits::IsFloatingPoint T>
        T Get4() { if constexpr (std::is_same_v<T, float>) return _4; if constexpr (std::is_same_v<T, double>) return _4d; }
    } // namespace Inv

    /** @brief namespace that defines the maximum or minimum value of the type */
    namespace Limit {
        /** @brief Int lowest value */
        constexpr int INT_LOWEST = (std::numeric_limits<int>::lowest)();
    }

    /** @brief Value used for geometric calculations */
    namespace Geo {
        /** @brief 1 / √2 */
        const float SQRT_1_D2 = 1.0f / std::sqrtf(2);
    } // namespace Geo

} // namespace Utl

#endif // !__UTILITY_H__
