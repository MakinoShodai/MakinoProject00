#ifndef __EASING_H__
#define __EASING_H__

/** @link https://easings.net/ja @endlink */

#include "UtilityForMath.h"

namespace Easing {
    /**
       @brief EaseIn Quadratic
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInQuad(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseOut Quadratic
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseOutQuad(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseInOut Quadratic
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInOutQuad(const T1& time, const T1& totalTime, const T2& start, const T2& end);



    /**
       @brief EaseIn Qubic
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInQubic(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseOut Qubic
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseOutQubic(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseInOut Qubic
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInOutQubic(const T1& time, const T1& totalTime, const T2& start, const T2& end);



    /**
       @brief EaseIn Quartic
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInQuart(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseOut Quartic
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseOutQuart(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseInOut Quartic
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInOutQuart(const T1& time, const T1& totalTime, const T2& start, const T2& end);



    /**
       @brief EaseIn Quintic
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInQuint(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseOut Quintic
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseOutQuint(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseInOut Quintic
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInOutQuint(const T1& time, const T1& totalTime, const T2& start, const T2& end);



    /**
       @brief EaseIn Sinusoidal
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInSine(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseOut Sinusoidal
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseOutSine(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseInOut Sinusoidal
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInOutSine(const T1& time, const T1& totalTime, const T2& start, const T2& end);



    /**
       @brief EaseIn Exponential
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInExpo(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseOut Exponential
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseOutExpo(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseInOut Exponential
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInOutExpo(const T1& time, const T1& totalTime, const T2& start, const T2& end);



    /**
       @brief EaseIn Circular
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInCirc(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseOut Circular
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseOutCirc(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseInOut Circular
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInOutCirc(const T1& time, const T1& totalTime, const T2& start, const T2& end);



    /**
       @brief EaseIn Back
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @param outPower Power to jump out from the start point of the value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInBack(const T1& time, const T1& totalTime, const T2& start, const T2& end, const float& outPower = 1.0f);
    /**
       @brief EaseOut Back
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @param outPower Power to jump out from the end point of the value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseOutBack(const T1& time, const T1& totalTime, const T2& start, const T2& end, const float& outPower = 1.0f);
    /**
       @brief EaseInOut Back
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @param outPower Power to jump out from the start point and the end point of the value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInOutBack(const T1& time, const T1& totalTime, const T2& start, const T2& end, const float& outPower = 1.0f);



    /**
       @brief EaseIn Bounce
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInBounce(const T1& time, const T1& totalTime, const T2& start, const T2& end);
    /**
       @brief EaseOut Bounce
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseOutBounce(const T1& time, const T1& totalTime, const T2& start, const T2& end);
#if 0
    /**
       @brief EaseInOut Bounce
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 EaseInOutBounce(const T1& time, const T1& totalTime, const T2& start, const T2& end);
#endif



    /**
       @brief Linear
       @param time Mediating variable (current)
       @param totalTime Mediating variable (end point)
       @param start Start point of value
       @param end End point of value
       @return Calculated value
    */
    template <typename T1 = int, typename T2 = float>
    [[nodiscard]] inline T2 Linear(const T1& time, const T1& totalTime, const T2& start, const T2& end);
} // namespace Easing



// EaseIn Quad
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInQuad(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime;

    // Calculating easing
    return e * t*t + start;
}
// EaseOut Quad
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseOutQuad(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime;

    // Calculating easing
    return -e * t * (t - 2.0f) + start;
}
// EaseInOut Quad
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInOutQuad(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / (totalTime / 2.0f);

    // Calculating in easing
    if (t < 1.0f)
        return e / 2.0f * t * t + start;

    // Decay the time value
    t -= 1.0f;

    // Calculating out easing
    return -e / 2.0f * (t * (t - 2.0f) - 1.0f) + start;
}



// EaseIn Qubic
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInQubic(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime;

    // Calculating easing
    return e * t*t*t + start;
}
// EaseOut Qubic
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseOutQubic(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime - 1.0f;

    // Calculating easing
    return e * (t*t*t + 1.0f) + start;
}
// EaseInOut Qubic
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInOutQubic(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / (totalTime / 2.0f);

    // Calculating in easing
    if (t < 1.0f)
        return e / 2.0f * t*t*t + start;

    // Decay the time value
    t -= 2.0f;

    // Calculating out easing
    return e / 2.0f * (t*t*t + 2.0f) + start;
}



// EaseIn Quart
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInQuart(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime;

    // Calculating easing
    return e * t*t*t*t + start;
}
// EaseOut Quart
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseOutQuart(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime - 1.0f;

    // Calculating easing
    return -e * (t*t*t*t - 1.0f) + start;
}
// EaseInOut Quart
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInOutQuart(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / (totalTime / 2.0f);

    // Calculating in easing
    if (t < 1.0f)
        return e / 2.0f * t*t*t*t + start;

    // Decay the time value
    t -= 2.0f;

    // Calculating out easing
    return -e / 2.0f * (t*t*t*t - 2.0f) + start;
}



// EaseIn Quintic
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInQuint(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime;

    // Calculating easing
    return e * t*t*t*t*t + start;
}
// EaseOut Quintic
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseOutQuint(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime - 1.0f;

    // Calculating easing
    return e * (t*t*t*t*t + 1.0f) + start;
}
// EaseInOut Quintic
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInOutQuint(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / (totalTime / 2.0f);

    // Calculating in easing
    if (t < 1.0f)
        return e / 2.0f * t*t*t*t*t + start;

    // Decay the time value
    t -= 2.0f;

    // Calculating out easing
    return e / 2.0f * (t*t*t*t*t + 2.0f) + start;
}



// EaseIn Sinusoidal
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInSine(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculating easing
    return -e * std::cosf(time * Utl::DEG_2_RAD * 90.0f / totalTime) + e + start;
}
// EaseOut Sinusoidal
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseOutSine(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculating easing
    return e * std::sinf(time * Utl::DEG_2_RAD * 90.0f / totalTime) + start;
}
// EaseInOut Sinusoidal
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInOutSine(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculating easing
    return  -e / 2.0f * (std::cosf(time * Utl::PI / totalTime) - 1.0f) + start;
}



// EaseIn Exponential
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInExpo(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime;

    // Calculating easing
    return e * std::powf(2.0f, 10.0f * (t - 1.0f)) + start;
}
// EaseOut Exponential
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseOutExpo(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime;

    // Calculating easing
    return e * (-std::powf(2.0f, -10.0f * t) + 1.0f) + start;
}
// EaseInOut Exponential
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInOutExpo(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / (totalTime / 2.0f);

    // Calculating in easing
    if (t < 1.0f)
        return e / 2.0f * std::powf(2.0f, 10.0f * (t - 1.0f)) + start;

    // Decay the time value
    t -= 1.0f;

    // Calculating out easing
    return e / 2.0f * (-std::powf(2.0f, -10.0f * t) + 2.0f) + start;
}



// EaseIn Circular
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInCirc(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime;

    // Calculating easing
    return -e * (std::sqrtf(1.0f - t * t) - 1.0f) + start;
}
// EaseOut Circular
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseOutCirc(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime - 1.0f;

    // Calculating easing
    return e * std::sqrtf(1.0f - t * t) + start;
}
// EaseInOut Circular
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInOutCirc(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / (totalTime / 2.0f);

    // Calculating in easing
    if (t < 1.0f)
        return -e / 2.0f * (std::sqrtf(1.0f - t * t) - 1.0f);

    // Decay the time value
    t -= 2.0f;

    // Calculating out easing
    return e / 2.0f * (std::sqrtf(1.0f - t * t) + 1.0f) + start;
}



// EaseIn Back
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInBack(const T1& time, const T1& totalTime, const T2& start, const T2& end, const float& outPower) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime;

    // Calculating easing
    return e * t*t * ((outPower + 1.0f) * t - outPower) + start;
}
// EaseOut Back
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseOutBack(const T1& time, const T1& totalTime, const T2& start, const T2& end, const float& outPower) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime - 1.0f;

    // Calculating easing
    return e * (t*t * ((outPower + 1.0f) * t + outPower) + 1.0f) + start;
}
// EaseInOut Back
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInOutBack(const T1& time, const T1& totalTime, const T2& start, const T2& end, const float& outPower) {
    // Calculate end point of value
    T2 e = end - start;
    float outP = outPower * 1.525f;

    // Calculate time value
    float t = (float)time / (totalTime / 2.0f);

    // Calculating in easing
    if (t < 1.0f)
        return e * (t*t * ((outP + 1.0f) * t - outP)) + start;

    // Decay the time value
    t -= 2.0f;

    // Calculating out easing
    return e / 2.0f * (t*t * ((outP + 1.0f) * t + outP) + 2.0f) + start;
}



// EaseIn Bounce
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseInBounce(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Prepare a value of type T2 with all 0s
    T2 param_0;
    memset(&param_0, 0, sizeof(T2));

    // Calculating easing
    return EaseOutBounce<T1, T2>(totalTime - time, totalTime, end - start, param_0);
}
// EaseOut Bounce
template <typename T1, typename T2>
[[nodiscard]] inline T2 Easing::EaseOutBounce(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    // Calculate end point of value
    T2 e = end - start;

    // Calculate time value
    float t = (float)time / totalTime;

    if (t < 1.0f / 2.75f)
        return e * (7.5625f * t*t) + start;

    else if (t < 2.0f / 2.75f)
    {
        t -= 1.5f / 2.75f;

        return e * (7.5625f * t*t + 0.75f) + start;
    }
    else if (t < 2.5f / 2.75f)
    {
        t -= 2.25f / 2.75f;

        return e * (7.5625f * t*t + 0.9375f) + start;
    }
    else
    {
        t -= 2.625f / 2.75f;

        return e * (7.5625f * t*t + 0.984375f) + start;
    }
}
#if 0
// EaseInOut Bounce
template <typename T1, typename T2>
[[nodiscard]] inline T2 CEasing::EaseInOutBounce(const T1& time, const T1& totalTime, const T2& start, const T2& end) {
    if (time < totalTime / 2.0f)
    {
        return EaseInBounce<T1, T2>(time * 2.0f, totalTime, end - start, end) * 0.5f + start;
    }
    else
    {
        // Prepare a value of type T2 with all 0s
        T2 param_0;
        memset(&param_0, 0, sizeof(T2));

        return EaseInOutBounce<T1, T2>(time * 2.0f - totalTime, totalTime, end - start, param_0) * 0.5f + start + (end - start) * 0.5f;
    }
}
#endif


// Linear
template<typename T1, typename T2>
inline T2 Easing::Linear(const T1 & time, const T1 & totalTime, const T2 & start, const T2 & end) {
    float t = (float)time / totalTime;

    return (end - start) * t + start;
}

#endif // !__EASING_H__
