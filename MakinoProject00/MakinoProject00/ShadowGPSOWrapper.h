/**
 * @file   ShadowGPSOWrapper.h
 * @brief  This file handles GPSO wrapper classes for shadowing.
 * 
 * @author Shodai Makino
 * @date   2024/1/22
 */

#ifndef __SHADOW_GPSO_WRAPPER_H__
#define __SHADOW_GPSO_WRAPPER_H__

#include "LayeredGPSOWrapper.h"

/** @brief GPSO wrapper class for writing depth */
class CWriteShadowGPSOWrapper : public ACLayeredGPSOWrapperPrefab {
public:
    /**
       @brief Prefab function
       @param scene The scene where this GPSO exists
    */
    void Prefab(CScene* scene) override;
};

/** @brief GPSO wrapper class for Shading */
class CShadingGPSOWrapper : public ACLayeredGPSOWrapperPrefab {
public:
    /**
       @brief Prefab function
       @param scene The scene where this GPSO exists
    */
    void Prefab(CScene* scene) override;
};

#endif // !__SHADOW_GPSO_WRAPPER_H__
