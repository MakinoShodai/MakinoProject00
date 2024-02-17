#include "StaticCbVPMat.h"
#include "Scene.h"

// Scene start processing
void CStaticCbVP::Start(CScene* scene) {
    m_cameraRegistry = scene->GetCameraRegistry();
}

// view projection matrix
Utl::Dx::CPU_DESCRIPTOR_HANDLE CStaticCbVP::AllocateData() {
    // Get the camera registry and the matrix if the camera exists
    if (m_cameraRegistry != nullptr) {
        auto camera = m_cameraRegistry->GetCameraPriority();
        if (camera != nullptr) {
            // Get a view matrix and a projection matrix
            DirectX::XMFLOAT4X4 data = camera->GenerateViewProjectionMatrix();

            return DirectDataCopy(&data);
        }
    }

    throw Utl::Error::CStopDrawingSceneError(L"Warning! Couldn't get the camera when calculating a view projection matrix.\n");
}

// Scene start processing
void CStaticCbRemovePosVP::Start(CScene* scene) {
    m_cameraRegistry = scene->GetCameraRegistry();
}

// Remove a position view projection matrix
Utl::Dx::CPU_DESCRIPTOR_HANDLE CStaticCbRemovePosVP::AllocateData() {
    // Get the camera registry and the matrix if the camera exists
    if (m_cameraRegistry != nullptr) {
        auto camera = m_cameraRegistry->GetCameraPriority();
        if (camera != nullptr) {
            // Get a view matrix removed a position and a projection matrix
            DirectX::XMFLOAT4X4 mat;
            DirectX::XMStoreFloat4x4(&mat, camera->GenerateViewMatrix());
            mat._41 = mat._42 = mat._43 = 0.0f;
            DirectX::XMStoreFloat4x4(&mat, DirectX::XMLoadFloat4x4(&mat) * camera->GenerateProjectionMatrix());

            return DirectDataCopy(&mat);
        }
    }

    throw Utl::Error::CStopDrawingSceneError(L"Warning! Couldn't get the camera when calculating a view projection matrix.\n");
}
