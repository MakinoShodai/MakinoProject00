/**
 * @file   StandardGPSOWrapper.h
 * @brief  This file handles GPSO wrap class for standard layer.
 * 
 * @author Shodai Makino
 * @date   2024/1/11
 */

#ifndef __STANDARD_GPSO_WRAPPER_H__
#define __STANDARD_GPSO_WRAPPER_H__

#include "LayeredGPSOWrapper.h"

/** @brief GPSO wrap class for standard layer */
class CStandardGPSOWrapper : public CLayeredGPSOWrapperPrefab {
public:
    /**
       @brief Prefab function
       @param scene The scene where this GPSO exists
    */
    void Prefab(ACScene* scene) override;
};

#endif // !__STANDARD_GPSO_WRAPPER_H__
