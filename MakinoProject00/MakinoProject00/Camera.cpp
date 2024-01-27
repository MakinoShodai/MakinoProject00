#include "Camera.h"
#include "GameObject.h"
#include "Scene.h"

// Constructor
CCameraComponent::CCameraComponent(CGameObject* owner, std::wstring cameraName, bool isFocusMode)
    : ACComponent(owner)
    , m_name(std::move(cameraName))
    , m_priority(0)
    , m_isFocusMode(isFocusMode)
    , m_up(Utl::Math::UNIT3_UP)
    , m_fovAngleY(Utl::DEG_2_RAD * 60.0f)
    , m_aspect(16.0f / 9.0f)
    , m_near(0.2f)
    , m_far(1000.0f)
    , m_rotationMatrix(DirectX::XMMatrixIdentity())
{ }

// Processing for the first update frame
void CCameraComponent::Start() {
    m_gameObj->GetScene()->GetCameraRegistry()->AddCamera(this);

    if (m_isFocusMode) {
        m_focus = GetTransform().pos + Utl::Math::UNIT3_FORWARD;
    }
    else {
        m_focus = GetTransform().rotation * Utl::Math::UNIT3_FORWARD;
    }
}

// Destroy processing
void CCameraComponent::OnDestroy() {
    m_gameObj->GetScene()->GetCameraRegistry()->RemoveCamera(this);
}

// Apply the rotation of this gameobject to this look direction vector
void CCameraComponent::ApplyRotation() {
    // If not in focus mode, update look direction vector
    if (false == m_isFocusMode) {
        m_focus = GetTransform().rotation * Utl::Math::UNIT3_FORWARD;

        // Calculate rotation matrix
        m_rotationMatrix = GenerateViewMatrix();
        m_rotationMatrix = DirectX::XMMatrixInverse(nullptr, m_rotationMatrix);
        m_rotationMatrix.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else {
        // Calculate rotation matrix
        m_rotationMatrix = GenerateViewMatrix();
        m_rotationMatrix = DirectX::XMMatrixInverse(nullptr, m_rotationMatrix);
        m_rotationMatrix.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

        Vector3f vFocus = m_focus - GetTransform().pos;
        vFocus.Normalize();
        m_gameObj->SetRotation(Utl::Math::VectorToVectorQuaternion(Utl::Math::UNIT3_FORWARD, vFocus));
    }
}

// Generate a view matrix
DirectX::XMMATRIX CCameraComponent::GenerateViewMatrix() {
    DirectX::XMMATRIX mat;

    // Get the position of this game object
    const Vector3f& pos = GetTransform().pos;

    Vector3f focus;
    // If in focus mode, use focus position
    if (m_isFocusMode) {
        // If this position and the fucus position as the same, shift the coordinate a little
        if (Utl::Math::IsEqualVector3f(pos, m_focus)) {
            focus = m_focus + Vector3f(0.0f, 0.0f, 0.01f);
        }
        else {
            focus = m_focus;
        }
    }
    // If not in focus mode, use look direction vector
    else {
        focus = pos + m_focus;
    }

    // Create a view matrix
    mat = DirectX::XMMatrixLookAtLH(
        DirectX::XMVectorSet(pos.x(), pos.y(), pos.z(), 0.0f),
        DirectX::XMVectorSet(focus.x(), focus.y(), focus.z(), 0.0f),
        DirectX::XMVectorSet(m_up.x(), m_up.y(), m_up.z(), 0.0f)
    );
    return mat;
}

// Generate a projection matrix
DirectX::XMMATRIX CCameraComponent::GenerateProjectionMatrix() {
    DirectX::XMMATRIX mat;

    // Create a projection matrix
    mat = DirectX::XMMatrixPerspectiveFovLH(
        m_fovAngleY, // Vertical angle of view of the camera
        m_aspect,    // Ratio of width to height when height is 1
        m_near,      // Distance to start projecting on screen
        m_far        // Distance to end projecting on screen
    );
    return mat;
}

// Generate a view projection matrix
DirectX::XMFLOAT4X4 CCameraComponent::GenerateViewProjectionMatrix() {
    DirectX::XMMATRIX mat;

    // Create a view projection matrix
    mat = GenerateViewMatrix() * GenerateProjectionMatrix();

    // Convert XMMATRIX to XMFLOAT4X4
    DirectX::XMFLOAT4X4 ret;
    DirectX::XMStoreFloat4x4(&ret, mat);

    return ret;
}
