/**
 * @file   SamplePrefab.h
 * @brief  This file handles prefabs for sample.
 * 
 * @author Shodai Makino
 * @date   2024/1/27
 */

#ifndef __SAMPLE_PREFAB_H__
#define __SAMPLE_PREFAB_H__

#include "GameObject.h"

// Sample prefab for static box
class CSampleStaticBoxPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CSampleStaticBoxPrefab);

// Sample prefab for bouncing ball
class CSampleSuperBallPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CSampleSuperBallPrefab);

// Sample prefab for clay ball
class CSampleClayBallPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CSampleClayBallPrefab);

#endif // !__SAMPLE_PREFAB_H__
