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

    throw Utl::Error::CStopDrawingSceneError(L"Warning! Couldn't get the render target when calculating a view projection matrix.\n");
}
