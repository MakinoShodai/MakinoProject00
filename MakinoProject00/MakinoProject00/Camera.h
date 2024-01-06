/**
 * @file   Camera.h
 * @brief  This file handles camera component.
 * 
 * @author Shodai Makino
 * @date   2023/12/6
 */

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Component.h"

/** @brief Camera component */
class CCameraComponent : public ACComponent {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this component
       @param cameraName Name of this camera
    */
    CCameraComponent(CGameObject* owner, std::wstring cameraName);

    /**
       @brief Processing for the first update frame
    */
    void Start() override;

    /**
       @brief Update processing
    */
    void Update() override {}

    /**
       @brief Generate a view matrix
       @return View matrix
    */
    DirectX::XMFLOAT4X4 GenerateViewMatrix();

    /**
       @brief Generate a projection matrix
       @return Projection matrix
    */
    DirectX::XMFLOAT4X4 GenerateProjectionMatrix();

    /**
       @brief Generate a view projection matrix
       @return View projection matrix
    */
    DirectX::XMFLOAT4X4 GenerateViewProjectionMatrix();

    /** @brief Get the name of this camera */
    const std::wstring& GetName() const { return m_name; }
    
    /** @brief Get the priority of this camera */
    int GetPriority() const { return m_priority; }
    /** @brief Set a priority to this camera */
    void SetPriority(int priority) { m_priority = priority; }

    /** @brief Get the focus position */
    const Vector3f& GetFocus() const { return m_focus; }
    /** @brief Set a focus position */
    void SetFocus(const Vector3f& focus) { m_focus = focus; }

    /** @brief Get the up vector */
    const Vector3f& GetUp() const { return m_up; }
    /** @brief Set a up vector */
    void SetUp(const Vector3f& up) { m_up = up; }

    /** @brief Get the fov angle Y */
    float GetFovAngleY() const { return m_fovAngleY; }
    /** @brief Set a fov angle Y */
    void SetFovAngleY(float fovAngleY) { m_fovAngleY = fovAngleY; }

    /** @brief Get the aspect ratio */
    float GetAspect() const { return m_aspect; }
    /** @brief Set an aspect ratio */
    void SetAspect(float aspect) { m_aspect = aspect; }

    /** @brief Get the near clipping value */
    float GetNear() const { return m_near; }
    /** @brief Set a near clipping value */
    void SetNear(float nearValue) { m_near = nearValue; }

    /** @brief Get the far clipping value */
    float GetFar() const { return m_far; }
    /** @brief Set a far clipping value */
    void SetFar(float farValue) { m_far = farValue; }

private:
    /** @brief Name of this camera */
    const std::wstring m_name;
    /** @brief Camera priority */
    int m_priority;

    /** @brief Focus position */
    Vector3f m_focus;
    /** @brief Up vector */
    Vector3f m_up;
    /** @brief Angle of view from top to bottom */
    float m_fovAngleY;
    /** @brief Aspect ratio */
    float m_aspect;
    /** @brief Near clipping value */
    float m_near;
    /** @brief Far clipping value */
    float m_far;
};

#endif // !__CAMERA_H__
