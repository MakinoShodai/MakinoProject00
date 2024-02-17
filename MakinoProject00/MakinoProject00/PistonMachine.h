/**
 * @file   PistonMachine.h
 * @brief  This file handles piston machine.
 * 
 * @author Shodai Makino
 * @date   2024/2/14
 */

#ifndef __PISTON_MACHINE_H__
#define __PISTON_MACHINE_H__

#include "Component.h"
#include "RigidBody.h"

/** @brief Component for piston machine */
class CPistonMachineComponent : public ACComponent {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this component
       @param power Power of pushed out
       @param coolTime Cool time of pushed out
    */
    CPistonMachineComponent(CGameObject* owner, float power, float coolTime);

    /**
       @brief Start processing
    */
    void Start() override;

    /**
       @brief Update processing
    */
    void Update() override;

private:
    /** @brief Cool time of pushed out */
    float m_coolTime;
    /** @brief Power of pushed out */
    float m_pushedOutPower;
    /** @brief Initial position */
    Vector3f m_initPos;
    /** @brief Variable for counting time */
    float m_timeCnt;
    /** @brief Is the piston being pushed out? */
    bool m_isPushedOut;
    /** @brief Position at the beginning of the return */
    std::optional<Vector3f> m_beginReturnPos;
};

/** @brief Prefab for piston machine */
class CPistonMachinePrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CPistonMachinePrefab);

/** @brief Prefab for piston machine that has high power */
class CPistonMachineHighPowerPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CPistonMachineHighPowerPrefab);

#endif // !__PISTON_MACHINE_H__

