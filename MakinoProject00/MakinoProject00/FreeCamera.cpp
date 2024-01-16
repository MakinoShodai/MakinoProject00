#include "FreeCamera.h"
#include "InputSystem.h"
#include "GameObject.h"
#include "ApplicationClock.h"

// Move factor
const float MOVE_FACTOR = 3.0f;
// Rotate factor
const float ROTATE_FACTOR = 0.1f;

// Move acceleration
const float MOVE_ACCELERATION = 1.0f;
// Upper limit of movement speed multiplier using acceleration
const float MAX_MOVE_ACCELERATION = 10.0f;

// Start processing
void CFreeCameraControl::Start() {
    m_isAcceptMove = false;
    m_totalMoveAcceleration = 1.0f;

    m_camera = m_gameObj->GetComponent<CCameraComponent>();
}

// Updating process
void CFreeCameraControl::Update() {
    CInputSystem& inputSystem = CInputSystem::GetMain();

    // If right button down of mouse
    if (inputSystem.IsKeyDown(VK_RBUTTON)) {
        // Start to accept movings and rotations
        m_isAcceptMove = true;
        m_totalMoveAcceleration = 1.0f;

        // Set cursor loop
        inputSystem.SetIsRepeatCursorInScreen(true);
    }
    // If right button up of mouse
    else if (inputSystem.IsKeyUp(VK_RBUTTON)) {
        // Finish to accept movings and rotations
        m_isAcceptMove = false;

        // Set cursor unloop
        inputSystem.SetIsRepeatCursorInScreen(false);
    }

    // If accepting movings and rotations
    if (m_isAcceptMove && m_camera) {
        float deltaTime = CAppClock::GetMain().GetDeltaTime();
        Vector3f pos = m_gameObj->GetTransform().pos;
        Quaternionf rotation = m_gameObj->GetTransform().rotation;

        // Get look direction vector and calculate right direction vector of it
        Vector3f lookDir = m_camera->GetLookDir();
        Vector3f lookRightDir = Quaternionf(90.0f * Utl::DEG_2_RAD, Utl::Math::UNIT3_Y) * Vector3f(lookDir.x(), 0.0f, lookDir.z());
        lookRightDir.Normalize();

        // Accepting movings of this gameobject
        float moveSpeed = MOVE_FACTOR * deltaTime * m_totalMoveAcceleration;
        bool isMove = false;
        // Forward and back
        if (inputSystem.IsKey('W')) {
            pos += moveSpeed * lookDir;
            isMove = true;
        }
        if (inputSystem.IsKey('S')) {
            pos -= moveSpeed * lookDir;
            isMove = true;
        }
        // Right and left
        if (inputSystem.IsKey('D')) {
            pos += moveSpeed * lookRightDir;
            isMove = true;
        }
        if (inputSystem.IsKey('A')) {
            pos -= moveSpeed * lookRightDir;
            isMove = true;
        }
        // Up and down
        if (inputSystem.IsKey('E')) {
            pos.y() += moveSpeed;
            isMove = true;
        }
        if (inputSystem.IsKey('Q')) {
            pos.y() -= moveSpeed;
            isMove = true;
        }

        // Accepting rotations of this gameobject
        bool isRotate = false;
        POINT deltaCursor = inputSystem.GetDeltaCursorPos();
        if (std::abs(deltaCursor.x) > 0) {
            float rad = (float)deltaCursor.x * Utl::DEG_2_RAD * ROTATE_FACTOR;
            rotation *= Quaternionf(rad, Utl::Math::UNIT3_Y);
            isRotate = true;
        }
        if (std::abs(deltaCursor.y) > 0) {
            float rad = (float)deltaCursor.y * Utl::DEG_2_RAD * ROTATE_FACTOR;
            Quaternionf calculatedRotation = Quaternionf(rad, lookRightDir) * rotation;
        
            // Correct rotation
            Vector3f predictedLookDir = calculatedRotation * Utl::Math::UNIT3_Z;
            float dot = Utl::Math::UNIT3_Y.Dot(predictedLookDir);
            if (std::abs(dot) < 0.99f) {
                isRotate = true;
                rotation = calculatedRotation;
            }
        }

        // Set position and rotation
        if (isMove) {
            m_gameObj->SetPos(pos);
        }
        if (isRotate) {
            m_gameObj->SetRotation(rotation);
        }

        // If it is moving itself, it will accelerate
        if (isMove) {
            m_totalMoveAcceleration += deltaTime * MOVE_ACCELERATION;
            if (m_totalMoveAcceleration >= MAX_MOVE_ACCELERATION) {
                m_totalMoveAcceleration = MAX_MOVE_ACCELERATION;
            }
        }
        else {
            m_totalMoveAcceleration = 1.0f;
        }
    }
}

// Processing when this component is disabled
void CFreeCameraControl::OnDisable() {
    if (m_isAcceptMove) {
        // Finish to accept movings and rotations
        m_isAcceptMove = false;

        // Set cursor unloop
        CInputSystem::GetMain().SetIsRepeatCursorInScreen(false);
    }
}
