#include "StaticCbOrthographicMat.h"
#include "Scene.h"

// Allocate data
Utl::Dx::CPU_DESCRIPTOR_HANDLE CStaticCbOrthographicMat::AllocateData() {
    // Get current rtv
    ACRenderTarget* rtv = ACRenderTarget::GetCurrentAppliedRenderTarget();

    DirectX::XMFLOAT4X4 proj;
    if (rtv != nullptr) {
        DirectX::XMStoreFloat4x4(&proj,
            DirectX::XMMatrixOrthographicLH(
                (float)rtv->GetWidth(),  // Width of resolution
                (float)rtv->GetHeight(), // Height of resolution
                -1.0f,                   // Minimum value of the range that can be projected in the Z coordinate
                1.0f                     // Maximum value of the range that can be projected in the Z coordinate
            )
        );
    }
    else {
        OutputDebugString(L"Warning! Couldn't get the render target when calculating the orthographic projection matrix.\n");
        DirectX::XMStoreFloat4x4(&proj, DirectX::XMMatrixIdentity());
    }

    return DirectDataCopy(&proj);
}

// Advance to the next data assignment?
bool CStaticCbOrthographicMat::IsAdvanceNextAllocate() {
    // Get current rtv
    ACRenderTarget* rtv = ACRenderTarget::GetCurrentAppliedRenderTarget();

    // If rtv is nullptr or the same as the previous render target, no need to advance
    if (rtv == nullptr || m_prevRenderTarget == rtv) {
        return false;
    }

    // Update the previous render target
    m_prevRenderTarget = rtv;
    return true;
}

// Scene start processing
void CStaticCbOrthographicViewProjMat::Start(CScene* scene) {
    m_cameraRegistry = scene->GetCameraRegistry();
}

// Allocate data
Utl::Dx::CPU_DESCRIPTOR_HANDLE CStaticCbOrthographicViewProjMat::AllocateData() {
    // Get current rtv
    ACRenderTarget* rtv = ACRenderTarget::GetCurrentAppliedRenderTarget();
    DirectX::XMFLOAT4X4 data;
    if (rtv != nullptr) {
        // Get the camera registry and the matrix if the camera exists
        if (m_cameraRegistry != nullptr) {
            auto camera = m_cameraRegistry->GetCameraPriority();
            if (camera != nullptr) {
                // Get a view matrix and generate orthographic projection matrix
                DirectX::XMStoreFloat4x4(&data,
                    camera->GenerateViewMatrix() * 
                    DirectX::XMMatrixOrthographicLH(
                        (float)rtv->GetWidth(),  // Width of resolution
                        (float)rtv->GetHeight(), // Height of resolution
                        camera->GetNear(),       // Camera near clipping value
                        camera->GetFar()         // Camera far clipping value
                    )
                );
            }
            else {
                throw Utl::Error::CStopDrawingSceneError(L"Warning! Couldn't get the camera when calculating a view projection matrix.\n");
            }
        }
        else {
            throw Utl::Error::CStopDrawingSceneError(L"Warning! Couldn't get the camera when calculating a view projection matrix.\n");
        }
    }
    else {
        OutputDebugString(L"Warning! Couldn't get the render target when calculating the orthographic projection matrix.\n");
        DirectX::XMStoreFloat4x4(&data, DirectX::XMMatrixIdentity());
    }

    return DirectDataCopy(&data);
}

// Advance to the next data assignment?
bool CStaticCbOrthographicViewProjMat::IsAdvanceNextAllocate() {
    // Get current rtv
    ACRenderTarget* rtv = ACRenderTarget::GetCurrentAppliedRenderTarget();

    // If rtv is nullptr or the same as the previous render target, no need to advance
    if (rtv == nullptr || m_prevRenderTarget == rtv) {
        return false;
    }

    // Update the previous render target
    m_prevRenderTarget = rtv;
    return true;
}
