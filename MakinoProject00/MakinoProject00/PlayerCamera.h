/**
 * @file   PlayerCamera.h
 * @brief  This file handles player camera component and prefab of player camera.
 * 
 * @author Shodai Makino
 * @date   2024/1/19
 */

#ifndef __PLAYER_CAMERA_H__
#define __PLAYER_CAMERA_H__

#include "Component.h"
#include "GameObject.h"
#include "Camera.h"
#include "PlayerControlComponent.h"

/** @brief Component for controlling player camera */
class CPlayerCameraControl : public ACComponent {
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
    /** @brief Player control component that player object has */
    CWeakPtr<CPlayerControlComponent> m_playerControl;
    /** @brief Camera component */
    CWeakPtr<CCameraComponent> m_cameraComponent;
    /** @brief Amount of vertical rotation */
    float m_verticalRad;
    /** @brief Amount of horizontal rotation */
    float m_horizontalRad;
    /** @brief Speed of Linear interpolation for radius of horizontal rotation */
    float m_horizontalRadLerpSpeed;

#ifdef _DEBUG
    /** @brief Shape for debug drawing focus point */
    CWeakPtr<CGameObject> m_focusPointShapeObj;
#endif // _DEBUG
};

/** @brief Prefab of player camera */
class CPlayerCameraPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CPlayerCameraPrefab);

#endif // !__PLAYER_CAMERA_H__
