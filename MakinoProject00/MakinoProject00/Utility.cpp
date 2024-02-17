#include "Utility.h"

// Initialize ID of the main thread
std::thread::id Utl::CMainThreadChecker::ms_mainThreadID = {};

// Converts radian values in the range [-Pi, Pi] to [0, Pi]
const float Utl::ConvertTo0To2Pi(float rad) {
    return (rad < 0) ? rad + PI2 : rad;
}

// Converts radian values in the range [0, 2Pi] to [-Pi, Pi]
const float Utl::ConvertToPlusMinusPi(float rad) {
    return (rad > Utl::PI) ? rad - Utl::PI2 : rad;
}

// Check if the value of Float is equal to 0
const bool Utl::IsFloatZero(const float a) {
    return (std::abs(a) <= Utl::FLOAT_IGNORE);
}

// Check if two values are equal
const bool Utl::IsEqual(const float a, const float b) {
    return (std::abs(a - b) <= Utl::FLOAT_IGNORE);
}

// Convert value, which is the value of inMin ～ inMax, to outMin ～ outMax
const float Utl::ConversionValueInRange(float value, float inMin, float inMax, float outMin, float outMax) {
    return Utl::Clamp((value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin, outMin, outMax);
}

// Convert value, which is the value of inMin ～ inMax, to outMin ～ outMax
const float Utl::ConversionValueInRange(float value, const ConversionRangeParameter& range) {
    return Utl::Clamp((value - range.inMin) * (range.outMax - range.outMin) / (range.inMax - range.inMin) + range.outMin, range.outMin, range.outMax);
}

// Linear interpolation from A to B
float Utl::Lerp(float a, float b, float t) {
    return (b - a) * t + a;
}

// Get value sign
int Utl::Sign(float value) {
    if (Utl::IsFloatZero(value)) {
        return 0;
    }
    else if (value > 0) {
        return 1;
    }
    return -1;
}

// Constructor
Utl::ConversionRangeParameter::ConversionRangeParameter(float inMin, float inMax, float outMin, float outMax)
    : inMin(inMin)
    , inMax(inMax)
    , outMin(outMin)
    , outMax(outMax)
{ }
