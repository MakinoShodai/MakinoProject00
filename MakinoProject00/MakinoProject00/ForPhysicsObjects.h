/**
 * @file   ForPhysicsObjects.h
 * @brief  This file handles objects for physics sample.
 * 
 * @author Shodai Makino
 * @date   2024/2/14
 */

#ifndef __FOR_PHYSICS_OBJECTS_H__
#define __FOR_PHYSICS_OBJECTS_H__

#include "GameObject.h"
#include "Component.h"

/** @brief Names of the physics sample prefab */
const std::string PREFAB_NAMES_PHYSICS_SAMPLE[3] = { "CPhysicsSampleSpherePrefab", "CPhysicsSampleCapsulePrefab", "CPhysicsSampleBoxPrefab" };

/** @brief This component regularly generates a physics sample prefab */
class CPhysicsSamplePrefabGenerateComponent : public ACComponent {
public:
    using ACComponent::ACComponent;

    /**
       @brief Start processing
    */
    void Start() override;

    /**
       @brief Update processing
    */
    void Update() override;

    /** @brief Decrement current number of the generated physics sample prefabs */
    static void DecrementCurrentGeneratedNum() { if (ms_currentGeneratedNum > 0) { ms_currentGeneratedNum--; } }

private:
    /** @brief Current number of the generated physics sample prefabs */
    static UINT ms_currentGeneratedNum;

    /** @brief Variable for counting time */
    float m_timeCnt;
};

/** @brief This component count for generated physics sample prefab */
class CPhysicsSampleCountComponent : public ACComponent {
public:
    using ACComponent::ACComponent;

    /**
       @brief Process to be called at instance destruction
    */
    void OnDestroy() override;
};

/** @brief Component for automatically destroying objects when the Y-coordinate falls under a certain coordinate */
class CAutoDestroyComponent : public ACComponent {
public:
    using ACComponent::ACComponent;

    /**
       @brief Update processing
    */
    void Update() override;
};

/** @brief This prefab regularly generates physics sample prefabs */
class CPhysicsSampleGeneraterPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CPhysicsSampleGeneraterPrefab);

 /** @brief Sphere prefab for physics sample */
class CPhysicsSampleSpherePrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CPhysicsSampleSpherePrefab);

/** @brief Capsule prefab for physics sample */
class CPhysicsSampleCapsulePrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CPhysicsSampleCapsulePrefab);

/** @brief Box prefab for physics sample */
class CPhysicsSampleBoxPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CPhysicsSampleBoxPrefab);

#endif // !__FOR_PHYSICS_OBJECTS_H__
