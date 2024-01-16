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

 /** @brief GPSO wrap abstract class for standard drawing */
class ACStandardGPSOWrapper : public ACLayeredGPSOWrapperPrefab {
protected:
    /**
       @brief Prefab helper function
       @param name Name of this GPSO
       @param scene The scene where this GPSO exists
       @param useLayers Layer to be used
       @param isDepthWrite Write to depth buffer?
    */
    void PrefabHelper(std::wstring name, ACScene* scene, std::initializer_list<GraphicsLayer> useLayers, bool isDepthWrite);
};

/** @brief GPSO wrap class for standard layer */
class CStandardGPSOWrapper : public ACStandardGPSOWrapper {
public:
    /**
       @brief Prefab function
       @param scene The scene where this GPSO exists
    */
    void Prefab(ACScene* scene) override;
};

/** @brief GPSO wrap class for transparent layer */
class CTransparentGPSOWrapper : public ACStandardGPSOWrapper {
public:
    /**
       @brief Prefab function
       @param scene The scene where this GPSO exists
    */
    void Prefab(ACScene* scene) override;
};

#endif // !__STANDARD_GPSO_WRAPPER_H__
