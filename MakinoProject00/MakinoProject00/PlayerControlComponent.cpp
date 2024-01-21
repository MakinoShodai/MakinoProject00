#include "PlayerControlComponent.h"
#include "InputSystem.h"
#include "ApplicationClock.h"
#include "Scene.h"

// Maximum movement speed
const float MOVE_MAX_SPEED = 15.0f;
// Movement acceleration (per second)
const float MOVE_ACCELERATION = 10.0f;
// Deceleration acceleration when stopped
const float STOP_DECELERATION = 40.0f;
// Decreasing speed when reversed
const float DECREASE_SPEED_REVERSE = 10.0f;

// Maximum value of camera offset X
const float MAX_CAMERA_OFFSET_X = 3.0f;
// Transition speed of camera offset X (per second)
const float SPEED_CAMERA_OFFSET_X = MAX_CAMERA_OFFSET_X * 0.5f;

// Starting process
void CPlayerControlComponent::Start() {
    // Initialize
    m_prevInputMove = Vector2f::Zero();
    m_isMoved = false;
    m_currentSpeed = 0.0f;
    m_localPlayerCameraOffsetX = 0.0f;

    // Get component
    m_rb = m_gameObj->GetComponent<CRigidBody>();
}

// Updating process
void CPlayerControlComponent::Update() {
    CInputSystem& inputSystem = CInputSystem::GetMain();
    float deltaTime = CAppClock::GetMain().GetDeltaTime();

    // Initialize movement flag
    m_isMoved = false;
    
    // Moves by keystroke
    Vector2f input;
    Vector3f vMove;
    if (inputSystem.IsKeyGenerateDir(&input)) {
        // Set movement flag
        m_isMoved = true;
        // Acceleration
        m_currentSpeed = Utl::Math::MoveTowards(m_currentSpeed, MOVE_MAX_SPEED, MOVE_ACCELERATION * deltaTime);
        // Convert input vector
        vMove = Vector3f(input.x(), 0.0f, input.y());

        // Transition the camera's horizontal offset to the direction of movement if it is moving sideways
        if (false == Utl::IsFloatZero(input.x())) {
            m_localPlayerCameraOffsetX = Utl::Math::MoveTowards(m_localPlayerCameraOffsetX, input.x() * MAX_CAMERA_OFFSET_X, deltaTime * SPEED_CAMERA_OFFSET_X);
        }
        // If not, offset back
        else {
            m_localPlayerCameraOffsetX = Utl::Math::MoveTowards(m_localPlayerCameraOffsetX, 0.0f, deltaTime * SPEED_CAMERA_OFFSET_X);
        }

        // Get current camera
        CCameraComponent* camera = GetScene()->GetCameraRegistry()->GetCameraPriority().Get();
        // Apply rotation of camera to the move vector
        Vector3f lookDirXZ = camera->GetLookDir();
        lookDirXZ.y() = 0.0f;
        lookDirXZ.Normalize();
        float radXZ = Utl::Math::UNIT3_FORWARD.AngleTo(lookDirXZ, Utl::Math::_Y);
        vMove = Quaternionf(radXZ, Utl::Math::UNIT3_UP) * vMove;
        // Normalize move vector
        vMove.Normalize();

        // Decrease speed when input is reversed
        float dot = m_prevInputMove.Dot(input);
        if (std::abs(dot + 1.0f) < FLT_EPSILON) {
            m_currentSpeed = (std::max)(m_currentSpeed - DECREASE_SPEED_REVERSE, 0.0f);
        }
        m_prevInputMove = input;
    }
    else {
        // Decceleration
        m_currentSpeed = Utl::Math::MoveTowards(m_currentSpeed, 0.0f, STOP_DECELERATION * deltaTime);
        // Set the direction of movement to forward
        vMove = GetTransform().GetForward();
    }

    if (m_currentSpeed > 0.0f) {
        Quaternionf rotation = Utl::Math::Slerp(m_gameObj->GetTransform().rotation, Utl::Math::VectorToVectorQuaternion(Utl::Math::UNIT3_FORWARD, vMove), 0.2f);
        // look towards the direction of movement
        m_gameObj->SetRotation(rotation);
        vMove = rotation * Utl::Math::UNIT3_FORWARD;

        // Correct the size of the movement to be a circle
        float angle = Utl::Math::UNIT3_FORWARD.AngleTo(vMove, Utl::Math::_Y);
        vMove.x() = sinf(angle) * deltaTime * m_currentSpeed;
        vMove.z() = cosf(angle) * deltaTime * m_currentSpeed;

        // Apply movement
        m_gameObj->SetPos(GetTransform().pos + vMove);
    }
}
