/**
 * @file   CameraRegistry.h
 * @brief  This file handles a camera registry.
 * 
 * @author Shodai Makino
 * @date   2023/12/6
 */

#ifndef __CAMERA_REGISTRY_H__
#define __CAMERA_REGISTRY_H__

#include "Camera.h"

/** @brief Registry class that manages all cameras in a scene */
class CCameraRegistry {
public:
    /**
       @brief Constructor
    */
    CCameraRegistry() = default;
    /**
       @brief Destructor
    */
    ~CCameraRegistry() = default;

    /**
       @brief Processing at the end of the update process
    */
    void EndUpdate();

    /**
       @brief Get a camera with highest priority
       @return Weak pointer to camera component
    */
    CWeakPtr<CCameraComponent> GetCameraPriority() const;

    /**
       @brief Get a camera with the same name as the specified name
       @param name Name to be searched
       @return Weak pointer to camera component
    */
    CWeakPtr<CCameraComponent> GetCameraName(const std::wstring& name) const;

    /**
       @brief Add a camera to the registry
       @param camera Camera to be added
    */
    void AddCamera(CCameraComponent* camera);

    /**
       @brief Remove a camera from the registry
       @param camera Camera to be removed
    */
    void RemoveCamera(CCameraComponent* camera);

private:
    /** @brief All cameras in a scene */
    std::vector<CWeakPtr<CCameraComponent>> m_cameras;
};

#endif // !__CAMERA_REGISTRY_H__
