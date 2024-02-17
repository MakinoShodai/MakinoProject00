/**
 * @file   StonePrefab.h
 * @brief  This file handles prefabs for each stones.
 * 
 * @author Shodai Makino
 * @date   2024/2/3
 */

#ifndef __STONE_PREFAB_H__
#define __STONE_PREFAB_H__

#include "GameObject.h"

/** @brief Prefab for middle stone 01 */
class CStoneMiddle01Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CStoneMiddle01Prefab);

/** @brief Prefab for middle stone 02 */
class CStoneMiddle02Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CStoneMiddle02Prefab);

/** @brief Prefab for middle stone 03 */
class CStoneMiddle03Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CStoneMiddle03Prefab);

/** @brief Prefab for small stone 01 */
class CStoneSmall01Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CStoneSmall01Prefab);

/** @brief Prefab for small stone 02 */
class CStoneSmall02Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CStoneSmall02Prefab);

/** @brief Prefab for small stone 03 */
class CStoneSmall03Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CStoneSmall03Prefab);

/** @brief Prefab for small stone 04 */
class CStoneSmall04Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CStoneSmall04Prefab);

/** @brief Prefab for small stone 05 */
class CStoneSmall05Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CStoneSmall05Prefab);

/** @brief Prefab for small stone 06 */
class CStoneSmall06Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CStoneSmall06Prefab);

#endif // !__STONE_PREFAB_H__
