#include "DebugCamera.h"
#include "InputSystem.h"
#include "Scene.h"

// Priority when debug camera is enabled
const int DEBUG_CAMERA_ENABLE_PRIORITY = 9999;
// Priority when debug camera is disabled
const int DEBUG_CAMERA_DISABLE_PRIORITY = -9999;

// Start processing
void CDebugCameraComponent::Start() {
    // Get components
    m_freeControl = m_gameObj->GetComponent<CFreeCameraControl>();
    m_cameraComponent = m_gameObj->GetComponent<CCameraComponent>();

    // Disable free control component
    m_freeControl->SetIsActive(false);
}

// Update processing
void CDebugCameraComponent::Update() {
    // If 0 key is pressed down
    if (CInputSystem::GetMain().IsKeyDown('0')) {
        // Disable debug mode and not allow the camera to move
        if (GetScene()->CheckUpdateMode(UpdateMode::Debug)) {
            GetScene()->SetUpdateMode(UpdateMode::Null);
            m_freeControl->SetIsActive(false);

            // Set priority for disabled
            m_cameraComponent->SetPriority(DEBUG_CAMERA_DISABLE_PRIORITY);
        }
        // Enable debug mode and allow the camera to move
        else {
            ACScene* scene = GetScene().Get();
            scene->SetUpdateMode(UpdateMode::Debug);
            m_freeControl->SetIsActive(true);

            // Align position and rotation with current camera
            CCameraComponent* currentCamera = scene->GetCameraRegistry()->GetCameraPriority().Get();
            m_gameObj->SetPos(currentCamera->GetTransform().pos);
            m_gameObj->SetRotation(currentCamera->GetTransform().rotation);

            // Set priority for enabled
            m_cameraComponent->SetPriority(DEBUG_CAMERA_ENABLE_PRIORITY);
        }
    }
}

// Prefab function
void CDebugCameraPrefab::Prefab() {
    // Add components
    AddComponent<CCameraComponent>(L"Debug camera")->SetPriority(DEBUG_CAMERA_DISABLE_PRIORITY);
    AddComponent<CFreeCameraControl>();
    AddComponent<CDebugCameraComponent>();

    // Set debug update mode
    OverwriteUpdateMode(UpdateMode::Debug);
}
