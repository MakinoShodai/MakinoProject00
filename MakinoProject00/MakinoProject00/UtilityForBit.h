/**
 * @file   UtilityForBit.h
 * @brief  This file defines utility functions that are useful for bit operations.
 * 
 * @author Shodai Makino
 * @date   2023/12/2
 */

#ifndef __UTILITY_FOR_BIT_H__
#define __UTILITY_FOR_BIT_H__

namespace Utl {
    /**
       @brief How many bits are standing?
       @param b Bits to be checked
       @return Number of standing bits
    */
    UINT CountBits(uint64_t b);

    /**
       @brief How many bits are standing?
       @param b Bits to be checked
       @return Number of standing bits
    */
    UINT CountBits(uint32_t b);

    /**
       @brief Bool check of enum class for bit operation
       @param flag Variable to be checked
       @return Result
    */
    template<typename T>
        requires std::is_enum_v<T>
    inline bool CheckEnumBit(T flag) { return static_cast<typename std::underlying_type<T>::type>(flag) != 0; }
} // namespace Utl

/** @brief Change Enum to bit flag */
#define ENUM_TO_BITFLAG(T)                                                      \
    static_assert(std::is_enum<T>::value, "T must be enum class type");         \
                                                                                \
    constexpr T operator|(const T lhs, const T rhs) {                           \
        using U = typename std::underlying_type<T>::type;                       \
        return static_cast<T>(static_cast<U>(lhs) | static_cast<U>(rhs));       \
    }                                                                           \
                                                                                \
    constexpr T operator&(const T lhs, const T rhs) {                           \
        using U = typename std::underlying_type<T>::type;                       \
        return static_cast<T>(static_cast<U>(lhs) & static_cast<U>(rhs));       \
    }                                                                           \
                                                                                \
    constexpr T operator^(const T lhs, const T rhs) {                           \
        using U = typename std::underlying_type<T>::type;                       \
        return static_cast<T>(static_cast<U>(lhs) ^ static_cast<U>(rhs));       \
    }                                                                           \
                                                                                \
    constexpr T operator~(const T val) {                                        \
        using U = typename std::underlying_type<T>::type;                       \
        return static_cast<T>(~static_cast<U>(val));                            \
    }                                                                           \
                                                                                \
    constexpr T operator<<(const T val, const unsigned int shift) {             \
        using U = typename std::underlying_type<T>::type;                       \
        return static_cast<T>(static_cast<U>(val) << shift);                    \
    }                                                                           \
                                                                                \
    constexpr T operator>>(const T val, const unsigned int shift) {             \
        using U = typename std::underlying_type<T>::type;                       \
        return static_cast<T>(static_cast<U>(val) >> shift);                    \
    }                                                                           \
                                                                                \
    inline T& operator|=(T& lhs, const T& rhs) {                                \
        using U = typename std::underlying_type<T>::type;                       \
        return lhs = static_cast<T>(static_cast<U>(lhs) | static_cast<U>(rhs)); \
    }                                                                           \
                                                                                \
    inline T& operator&=(T& lhs, const T& rhs) {                                \
        using U = typename std::underlying_type<T>::type;                       \
        return lhs = static_cast<T>(static_cast<U>(lhs) & static_cast<U>(rhs)); \
    }                                                                           \
                                                                                \
    inline T& operator^=(T& lhs, const T& rhs) {                                \
        using U = typename std::underlying_type<T>::type;                       \
        return lhs = static_cast<T>(static_cast<U>(lhs) ^ static_cast<U>(rhs)); \
    }                                                                           \
                                                                                \
    inline T& operator<<=(T& lhs, const unsigned int shift) {                   \
        using U = typename std::underlying_type<T>::type;                       \
        return lhs = static_cast<T>(static_cast<U>(lhs) << shift);              \
    }                                                                           \
                                                                                \
    inline T& operator>>=(T& lhs, const unsigned int shift) {                   \
        using U = typename std::underlying_type<T>::type;                       \
        return lhs = static_cast<T>(static_cast<U>(lhs) >> shift);              \
    }

#endif // !__UTILITY_FOR_BIT_H__
