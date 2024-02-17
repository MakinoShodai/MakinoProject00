/**
 * @file   Terrain.h
 * @brief  This file handles prefab for terrain.
 * 
 * @author Shodai Makino
 * @date   2024/2/3
 */

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "GameObject.h"

/** @brief Prefab for grass terrain */
class CGrassTerrainPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CGrassTerrainPrefab);

#endif // !__TERRAIN_H__
