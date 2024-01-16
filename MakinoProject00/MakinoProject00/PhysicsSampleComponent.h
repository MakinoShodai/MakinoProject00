/**
 * @file   PhysicsSampleComponent.h
 * @brief  This file handles sample components for physics engine.
 * 
 * @author Shodai Makino
 * @date   2024/1/17
 */

#ifdef _FOR_PHYSICS

#ifndef __PHYSICS_SAMPLE_COMPONENT_H__
#define __PHYSICS_SAMPLE_COMPONENT_H__

#include "Component.h"
#include "Shape.h"

/** @brief Sample component for OnCollision */
class CSampleOnCollisionComponent : public ACComponent {
public:
    using ACComponent::ACComponent;

    /**
       @brief Starting process
    */
    void Start();

    /**
       @brief Processing when collision begins
       @param opponent Information of a contacted collider
    */
    void OnCollisionBegin(const CContactOpponent& opponent) override;

private:
    /** @brief Shape component for debugging collider */
    CWeakPtr<CDebugColliderShape> m_shapeComponent;
};

#endif // !__PHYSICS_SAMPLE_COMPONENT_H__

#endif // _FOR_PHYSICS