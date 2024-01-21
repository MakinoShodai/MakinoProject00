/**
 * @file   PlayerControlComponent.h
 * @brief  This file handles component for moving the player by key input.
 * 
 * @author Shodai Makino
 * @date   2024/1/18
 */

#ifndef __PLAYER_CONTROL_COMPONENT_H__
#define __PLAYER_CONTROL_COMPONENT_H__

#include "Component.h"
#include "RigidBody.h"

/** @brief Component for moving the player by key input */
class CPlayerControlComponent : public ACComponent {
public:
    using ACComponent::ACComponent;

    /**
       @brief Starting process
    */
    void Start();

    /**
       @brief Updating process
    */
    void Update();

    /** @brief Get offset X of the focus point of the player camera in XZ local coordinate space of the camera */
    float GetPlayerCameraOffsetX() { return m_localPlayerCameraOffsetX; }
    /** @brief Did this object run on the last update frame?  */
    bool IsMoved() { return m_isMoved; }

private:
    /** @brief Rigid body */
    CWeakPtr<CRigidBody> m_rb;
    /** @brief Input vector of movement at previous frame */
    Vector2f m_prevInputMove;
    /** @brief Current speed of player */
    float m_currentSpeed;
    /** @brief offset X of the focus point of the player camera in XZ local coordinate space of the camera */
    float m_localPlayerCameraOffsetX;
    /** @brief Did this object run on the last update frame? */
    bool m_isMoved;
};

#endif // !__PLAYER_CONTROL_COMPONENT_H__
