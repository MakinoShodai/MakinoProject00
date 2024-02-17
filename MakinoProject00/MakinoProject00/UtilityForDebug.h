/**
 * @file   UtilityForDebug.h
 * @brief  This file defines utility functions that are useful for debugging..
 * 
 * @author Shodai Makino
 * @date   2024/1/16
 */

#ifndef __UTILITY_FOR_DEBUG_H__
#define __UTILITY_FOR_DEBUG_H__

#include "Utility.h"

// Macro for outputting 3D vector as OutputDebugString
#define OutputDebugStringVector3f(vec) OutputDebugString((CONVERT_TOKEN2LITERAL(vec) L" : " + \
    std::to_wstring(vec.x()) + L", " + \
    std::to_wstring(vec.y()) + L", " + \
    std::to_wstring(vec.z()) + L"\n").c_str());

// Macro for outputting float as OutputDebugString
#define OutputDebugStringNumeric(val) OutputDebugString((CONVERT_TOKEN2LITERAL(val) L" : " + \
    std::to_wstring(val) + L"\n").c_str());

#endif // !__UTILITY_FOR_DEBUG_H__
