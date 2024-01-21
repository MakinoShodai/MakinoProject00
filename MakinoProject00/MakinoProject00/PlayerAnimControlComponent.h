/**
 * @file   PlayerAnimControlComponent.h
 * @brief  This file handles component for controlling animation of player.
 * 
 * @author Shodai Makino
 * @date   2024/1/18
 */

#ifndef __PLAYER_ANIM_CONTROL_COMPONENT_H__
#define __PLAYER_ANIM_CONTROL_COMPONENT_H__

#include "Component.h"
#include "PlayerControlComponent.h"
#include "Model.h"

/** @brief component for controlling animation of player */
class CPlayerAnimControlComponent : public ACComponent {
public:
    using ACComponent::ACComponent;

    /**
       @brief Starting process
    */
    void Start() override;

    /**
       @brief Updating process
    */
    void Update() override;

private:
    /** @brief Model component of this game object */
    CWeakPtr<CSkeletalModel> m_model;
    /** @brief Component to control player */
    CWeakPtr<CPlayerControlComponent> m_playerControl;
    /** @brief Animation id and loop flag for idle animation */
    AnimIDWithLoop m_idleAnim;
    /** @brief Animation id and loop flag for run animation */
    AnimIDWithLoop m_runAnim;
    /** @brief Animation id and loop flag for jump animation */
    AnimIDWithLoop m_jumpAnim;
};

#endif // !__PLAYER_ANIM_CONTROL_COMPONENT_H__
