/**
 * @file   DebugCamera.h
 * @brief  This file handles debug camera component and prefab of debug camera.
 * 
 * @author Shodai Makino
 * @date   2024/1/18
 */

#ifndef __DEBUG_CAMERA_H__
#define __DEBUG_CAMERA_H__

#include "Component.h"
#include "GameObject.h"
#include "FreeCamera.h"

/** @brief Prefab name for debug camera */
const std::string PREFAB_NAME_DEBUGCAMERA = "CDebugCameraPrefab";

/** @brief Component for debug camera */
class CDebugCameraComponent : public ACComponent {
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

private:
    /** @brief Component for free camera movement */
    CWeakPtr<CFreeCameraControl> m_freeControl;
    /** @brief Camera component */
    CWeakPtr<CCameraComponent> m_cameraComponent;
};

/** @brief Prefab of debug camera object */
class CDebugCameraPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CDebugCameraPrefab);

#endif // !__DEBUG_CAMERA_H__
