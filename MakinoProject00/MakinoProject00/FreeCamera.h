/**
 * @file   FreeCamera.h
 * @brief  This file handles free camera.
 * 
 * @author Shodai Makino
 * @date   2024/1/16
 */

#ifndef __FREE_CAMERA_H__
#define __FREE_CAMERA_H__

#include "Component.h"
#include "Camera.h"

/** @brief Component for free camera movement */
class CFreeCameraControl : public ACComponent {
public:
    using ACComponent::ACComponent;

    /**
       @brief Start processing
    */
    void Start() override;

    /**
       @brief Updating process
    */
    void Update() override;

    /**
       @brief Processing when this component is disabled
    */
    void OnDisable() override;

private:
    /** @brief Is this component accepting movings and rotations? */
    bool m_isAcceptMove;
    /** @brief Weak pointer to camera */
    CWeakPtr<CCameraComponent> m_camera;
    /** @brief Total acceleration of movement */
    float m_totalMoveAcceleration;
};

#endif // !__FREE_CAMERA_H__
