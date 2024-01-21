/**
 * @file   UpdateMode.h
 * @brief  This file handles update mode of scene.
 * 
 * @author Shodai Makino
 * @date   2024/1/18
 */

#ifndef __UPDATE_MODE_H__
#define __UPDATE_MODE_H__

#include "UtilityForBit.h"

/** @brief Type of scene update mode */
using UpdateModeType = UINT32;

/**
   @brief Update mode of scene
   While update mode is in effect, 
   any updating process are not performed except 
   for updating and predraw processing objects that are allowed to be updated.
*/
enum class UpdateMode : UpdateModeType {
    /** @brief No mode */
    Null = 0,
    /** @brief Debug mode */
    Debug = 1 << 1,
    /** @brief Event mode */
    Event = 1 << 2,
};
ENUM_TO_BITFLAG(UpdateMode); // Support for bit operation

#endif // !__UPDATE_MODE_H__
