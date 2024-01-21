/**
 * @file   PlayerPrefab.h
 * @brief  This file handles prefab of player.
 * 
 * @author Shodai Makino
 * @date   2024/1/18
 */

#ifndef __PLAYER_PREFAB_H__
#define __PLAYER_PREFAB_H__

#include "GameObject.h"

/** @brief Name of player object */
const std::wstring OBJNAME_PLAYER = L"Player";

/** @brief Prefab of player */
class CPlayerPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;

};

#endif // !__PLAYER_PREFAB_H__

