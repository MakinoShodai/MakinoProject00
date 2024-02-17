/**
 * @file   Ruins.h
 * @brief  This file handles prefab for ruin environment object.
 * 
 * @author Shodai Makino
 * @date   2024/2/3
 */

#ifndef __RUINS_H__
#define __RUINS_H__

#include "GameObject.h"

/** @brief Prefab for big brick 01 */
class CBigBrick01Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CBigBrick01Prefab);

/** @brief Prefab for big brick 02 */
class CBigBrick02Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CBigBrick02Prefab);

/** @brief Prefab for big brick 04 */
class CBigBrick04Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CBigBrick04Prefab);

/** @brief Prefab for ruin column 03 */
class CColumn03Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CColumn03Prefab);

/** @brief Prefab for ruin wall element 02 */
class CWallElement02Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CWallElement02Prefab);

#endif // !__RUINS_H__
