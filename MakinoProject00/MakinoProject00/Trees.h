/**
 * @file   Trees.h
 * @brief  This file handles prefabs for each trees.
 * 
 * @author Shodai Makino
 * @date   2024/2/3
 */

#ifndef __TREE_H__
#define __TREE_H__

#include "GameObject.h"

 /** @brief Prefab for tree 01 */
class CTree01Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CTree01Prefab);

/** @brief Prefab for tree 02 */
class CTree02Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CTree02Prefab);

/** @brief Prefab for tree 03 */
class CTree03Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CTree03Prefab);

/** @brief Prefab for tree 04 */
class CTree04Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CTree04Prefab);

/** @brief Prefab for tree 05 */
class CTree05Prefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /** @brief Prefab function */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CTree05Prefab);

#endif // !__TREE_H__
