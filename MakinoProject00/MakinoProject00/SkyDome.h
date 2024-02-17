/**
 * @file   SkyDome.h
 * @brief  This file handles prefab for sky dome.
 * 
 * @author Shodai Makino
 * @date   2024/2/2
 */

#ifndef __SKY_DOME_H__
#define __SKY_DOME_H__

#include "GameObject.h"

/** @brief Prefab name for sky dome */
const std::string PREFAB_NAME_SKYDOME = "CSkyDomePrefab";

// Sample prefab for sky dome
class CSkyDomePrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CSkyDomePrefab);

#endif // !__SKY_DOME_H__
