#include "Utility.h"

// Initialize ID of the main thread
std::thread::id Utl::CMainThreadChecker::ms_mainThreadID = {};

// Check if the value of Float is equal to 0
const bool Utl::IsFloatZero(const float a) {
    return std::abs(a) <= Utl::FLOAT_IGNORE ? true : false;
}

// Check if two values are equal
const bool Utl::IsEqual(const float a, const float b) {
    return std::abs(a - b) <= Utl::FLOAT_IGNORE ? true : false;
}

// Clamp a value in min to max
const float Utl::Clamp(const float val, const float min, const float max) {
    return
        // Clamp a value in max
        (val > max) ? max :
        // Clamp a value in min
        (val < min) ? min : val;
}

// Convert value, which is the value of inMin ～ inMax, to outMin ～ outMax
const float Utl::ConversionValue(float value, float inMin, float inMax, float outMin, float outMax) {
    return Utl::Clamp((value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin, outMin, outMax);
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
