/**
 * @file   SampleComponent.h
 * @brief  This file handles components for samples not used in the game.
 * 
 * @author Shodai Makino
 * @date   2024/1/11
 */

#ifdef _SAMPLE

#ifndef __SAMPLE_COMPONENT_H__
#define __SAMPLE_COMPONENT_H__

#include "Component.h"
#include "Model.h"

/** @brief Component to only keep rotating */
class CSampleRotateComponent : public ACComponent {
public:
    /**
       @brief Constructor
       @param obj Game object
       @param axis Central axis of rotation
       @param speed Rotation speed
    */
    CSampleRotateComponent(CGameObject* obj, Vector3f axis, float speed)
        : ACComponent(obj), m_axis(axis), m_speed(speed) {}

    /**
       @brief Starting process
    */
    void Start() { }

    /**
       @brief Updating process
    */
    void Update();

private:
    /** @brief Central axis of rotation */
    Vector3f m_axis;
    /** @brief Rotation speed */
    float m_speed;
};

/** @brief Component for animation control in the sample scene */
class CSampleAnimComponent : public ACComponent {
public:
    // Using ACComponent constructor
    using ACComponent::ACComponent;

    /**
       @brief Starting process
    */
    void Start();

    /**
       @brief Updating process
    */
    void Update();

private:
    /** @brief Weak pointer to CSkeletalModel */
    CWeakPtr<CSkeletalModel> m_model;
    /** @brief Animation ID */
    ModelInfo::AnimID m_currentAnimID;
};

#endif // !__SAMPLE_COMPONENT_H__

#endif // !_SAMPLE
