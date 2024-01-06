#include "Camera.h"
#include "GameObject.h"
#include "Scene.h"

// Constructor
CCameraComponent::CCameraComponent(CGameObject* owner, std::wstring cameraName) 
    : ACComponent(owner)
    , m_name(std::move(cameraName))
    , m_priority(0)
    , m_focus(Vector3f::Zero())
    , m_up(Utl::Math::UNIT3_Y)
    , m_fovAngleY(Utl::DEG_2_RAD * 60.0f)
    , m_aspect(16.0f / 9.0f)
    , m_near(0.2f)
    , m_far(1000.0f)
{ }

// Processing for the first update frame
void CCameraComponent::Start() {
    m_gameObj->GetScene()->GetCameraRegistry()->AddCamera(this);
}

// Generate a view matrix
DirectX::XMFLOAT4X4 CCameraComponent::GenerateViewMatrix() {
    DirectX::XMMATRIX mat;

    // Get the position of this game object
    Vector3f pos = m_gameObj->GetTransform().pos;

    // If this position and the fucus position as the same, shift the coordinate a little
    if (Utl::Math::IsEqualVector3f(pos, m_focus)) {
        pos.y() += 0.1f;
    }

    // Create a view matrix
    mat = DirectX::XMMatrixLookAtLH(
        DirectX::XMVectorSet(pos.x(), pos.y(), pos.z(), 0.0f),
        DirectX::XMVectorSet(m_focus.x(), m_focus.y(), m_focus.z(), 0.0f),
        DirectX::XMVectorSet(m_up.x(), m_up.y(), m_up.z(), 0.0f)
    );
    
    // Convert XMMATRIX to XMFLOAT4X4
    DirectX::XMFLOAT4X4 ret;
    DirectX::XMStoreFloat4x4(&ret, mat);

    return ret;
}

// Generate a projection matrix
DirectX::XMFLOAT4X4 CCameraComponent::GenerateProjectionMatrix() {
    DirectX::XMMATRIX mat;

    // Create a projection matrix
    mat = DirectX::XMMatrixPerspectiveFovLH(
        m_fovAngleY, // Vertical angle of view of the camera
        m_aspect,    // Ratio of width to height when height is 1
        m_near,      // Distance to start projecting on screen
        m_far        // Distance to end projecting on screen
    );

    // Convert XMMATRIX to XMFLOAT4X4
    DirectX::XMFLOAT4X4 ret;
    DirectX::XMStoreFloat4x4(&ret, mat);

    return ret;
}

// Generate a view projection matrix
DirectX::XMFLOAT4X4 CCameraComponent::GenerateViewProjectionMatrix() {
    DirectX::XMMATRIX mat;

    // Get the position of this game object
    Vector3f pos = m_gameObj->GetTransform().pos;

    // If this position and the fucus position as the same, shift the coordinate a little
    if (Utl::Math::IsEqualVector3f(pos, m_focus)) {
        pos.y() += 0.1f;
    }

    // Create a view projection matrix
    mat = 
        DirectX::XMMatrixLookAtLH(
            DirectX::XMVectorSet(pos.x(), pos.y(), pos.z(), 0.0f),
            DirectX::XMVectorSet(m_focus.x(), m_focus.y(), m_focus.z(), 0.0f),
            DirectX::XMVectorSet(m_up.x(), m_up.y(), m_up.z(), 0.0f)) * 
        DirectX::XMMatrixPerspectiveFovLH(m_fovAngleY, m_aspect, m_near, m_far);

    // Convert XMMATRIX to XMFLOAT4X4
    DirectX::XMFLOAT4X4 ret;
    DirectX::XMStoreFloat4x4(&ret, mat);

    return ret;
}
