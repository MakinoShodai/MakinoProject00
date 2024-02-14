#include "PlayerCamera.h"
#include "Scene.h"
#include "PlayerPrefab.h"
#include "InputSystem.h"
#include "ApplicationClock.h"

// Minimum radius of vertical rotation
const float MIN_VERTICAL_RAD = -70.0f * Utl::DEG_2_RAD;
// Maximum radius of vertical rotation
const float MAX_VERTICAL_RAD = 85.0f * Utl::DEG_2_RAD;
// Offset Y of focus point
const float FOCUS_OFFSET_Y = 5.0f;
// Lerp speed of focus point
const float FOCUS_LERP_SPEED = 0.2f;
// Minimum value of distance from the position of this camera to the focus point
const float MIN_FOCUS_DISTANCE = 15.0f;
// Maximum value of distance from the position of this camera to the focus point
const float MAX_FOCUS_DISTANCE = 30.0f;
// Maximum speed of linear interpolation for horizontal rotation
const float MIN_SPEED_HORIZONTAL_RAD_LERP = 0.01f;
// Minimum speed of linear interpolation for horizontal rotation
const float MAX_SPEED_HORIZONTAL_RAD_LERP = 0.5f;
// Helper structure for calculating linear interpolation rate for horizontal rotation
const Utl::ConversionRangeParameter CONVERT_RANGE_HORIZONTAL_RAD_LERP = 
    Utl::ConversionRangeParameter(3.0f, MIN_FOCUS_DISTANCE, MIN_SPEED_HORIZONTAL_RAD_LERP, MAX_SPEED_HORIZONTAL_RAD_LERP);

// Starting process
void CPlayerCameraControl::Start() {
    // Initialize
    m_verticalRad = 0.0f;
    m_horizontalRad = 0.0f;
    m_horizontalRadLerpSpeed = 1.0f;

    // Get component
    m_cameraComponent = m_gameObj->GetComponent<CCameraComponent>();

    // Get player control component that player object has
    CWeakPtr<CGameObject> player = GetScene()->FindGameObject(OBJNAME_PLAYER);
    m_playerControl = (player != nullptr) ? player->GetComponent<CPlayerControlComponent>() : nullptr;

    // Set focus point of this camera
    if (m_playerControl) {
        Vector3f playerPos = m_playerControl->GetTransform().pos;
        m_gameObj->SetPos(playerPos - m_playerControl->GetTransform().GetForward());
        m_cameraComponent->SetFocus(playerPos + Vector3f(0.0f, FOCUS_OFFSET_Y, 0.0f));
    }

#ifdef _DEBUG
    //m_focusPointShapeObj = GetScene()->CreateGameObject<CGameObject>(Transformf(m_cameraComponent->GetFocus(), Vector3f::Ones() * 0.1f, Vector3f::Zero()));
    //m_focusPointShapeObj->AddComponent<CColorOnlyShape>(GraphicsLayer::Transparent, ShapeKind::Sphere, Colorf(1.0f, 0.0f, 0.0f, 0.3f));
#endif
}

// Updating process
void CPlayerCameraControl::Update() {
    CInputSystem& inputSystem = CInputSystem::GetMain();
    float deltaTime = CAppClock::GetMain().GetDeltaTime();

    Vector3f currentFocusPoint = m_cameraComponent->GetFocus();
    // If the player exists
    if (m_playerControl != nullptr) {
        // Calculate the focus point including offset
        Vector3f offset = Quaternionf(m_horizontalRad, Utl::Math::UNIT3_UP) * Vector3f(m_playerControl->GetPlayerCameraOffsetX(), FOCUS_OFFSET_Y, 0.0f);
        Vector3f focusPoint = m_playerControl->GetTransform().pos + offset;
        // Transition the current focus point toward a new focus point
        currentFocusPoint = Utl::Math::Lerp(currentFocusPoint, focusPoint, FOCUS_LERP_SPEED);
        m_cameraComponent->SetFocus(currentFocusPoint);
    }

    // Calculate the vector in XZ plane from the camera position to the focus point
    Vector3f vFocusXZ = currentFocusPoint - GetTransform().pos;
    vFocusXZ.y() = 0.0f;
    vFocusXZ.Normalize();

    // Calculate the difference from the horizontal radian value in the previous frame
    Vector3f vBaseXZ = (Quaternionf(m_horizontalRad, Utl::Math::UNIT3_UP) * Utl::Math::UNIT3_FORWARD).GetNormalize();
    float deltaRad = vBaseXZ.AngleTo(vFocusXZ, Utl::Math::_Y);

    // Transition horizontal radian values
    m_horizontalRad = Utl::Lerp(m_horizontalRad, m_horizontalRad + deltaRad, m_horizontalRadLerpSpeed);

    // Change vertical and horizontal radian values in response to keystrokes
    POINT deltaCursor = inputSystem.GetDeltaCursorPos();
    m_horizontalRad += (float)deltaCursor.x * deltaTime;
    m_verticalRad += (float)deltaCursor.y * deltaTime;
    // Vertical rotation is limited
    m_verticalRad = Utl::Clamp(m_verticalRad, MIN_VERTICAL_RAD, MAX_VERTICAL_RAD);

    // Calculate distance from the position of this camera to the focus point according to vertical rotation
    float distance = Utl::ConversionValueInRange(m_verticalRad, 0.0f, MAX_VERTICAL_RAD, MIN_FOCUS_DISTANCE, MAX_FOCUS_DISTANCE);
    // Rotate the vector of calculated distances and set it as the camera position
    Vector3f vNewFocus = Quaternionf(Vector3f(m_verticalRad, m_horizontalRad, 0.0f)) * Vector3f(0.0f, 0.0f, distance);
    vNewFocus.y() = (std::min)(vNewFocus.y(), FOCUS_OFFSET_Y - 0.1f);
    m_gameObj->SetPos(currentFocusPoint - vNewFocus);

    // Calculate the lerp speed of the horizontal rotation 
    // for the next frame according to the length in the XZ plane of the vector to the new focus point
    vNewFocus.y() = 0.0f;
    m_horizontalRadLerpSpeed = Utl::ConversionValueInRange(vNewFocus.Length(), CONVERT_RANGE_HORIZONTAL_RAD_LERP);

    // Draw focus points for debugging
#ifdef _DEBUG
    //m_focusPointShapeObj->SetPos(currentFocusPoint);
#endif // _DEBUG
}

// Prefab function
void CPlayerCameraPrefab::Prefab() {
    AddComponent<CCameraComponent>(L"Player camera", true);
    AddComponent<CPlayerCameraControl>();
}
