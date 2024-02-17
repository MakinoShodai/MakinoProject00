/**
 * @file   PointLightPrefab.h
 * @brief  This file handles prefabs for point light.
 * 
 * @author Shodai Makino
 * @date   2024/2/1
 */

#ifndef __POINT_LIGHT_PREFAB_H__
#define __POINT_LIGHT_PREFAB_H__

#include "GameObject.h"

/** @brief Prefab for regular point light */
class CPointLightRegularPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CPointLightRegularPrefab);

/** @brief Prefab for strong point light */
class CPointLightStrongPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CPointLightStrongPrefab);

/** @brief Prefab for weak point light */
class CPointLightWeakPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CPointLightWeakPrefab);

#endif // !__POINT_LIGHT_PREFAB_H__
