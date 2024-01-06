#include "CameraRegistry.h"
#include "Application.h"

// Get a camera with highest priority
CWeakPtr<CCameraComponent> CCameraRegistry::GetCameraPriority() const {
    int highestPriority = Utl::Limit::INT_LOWEST;
    CWeakPtr<CCameraComponent> ret = nullptr;

    // Find the camera with highest priority
    for (auto& it : m_cameras) {
        if (highestPriority < it->GetPriority()) {
            ret = it;
            highestPriority = it->GetPriority();
        }
    }

    // Check camera size
    if (ret == nullptr) { OutputDebugString(L"Warning! Camera is not found in the scene!\n"); }
    return ret;
}

// Get a camera with the same name as the specified name
CWeakPtr<CCameraComponent> CCameraRegistry::GetCameraName(const std::wstring& name) const {
    // Find the camera with highest priority
    for (auto& it : m_cameras) {
        if (name == it->GetName()) {
            return it;
        }
    }

    // Check camera size
    OutputDebugString(L"Warning! Camera with specified name not found in the scene!\n");
    return nullptr;
}

// Add a camera to the registry
void CCameraRegistry::AddCamera(CCameraComponent* camera) {
    if (camera != nullptr) {
        m_cameras.push_back(camera->WeakFromThis());
    }
}
