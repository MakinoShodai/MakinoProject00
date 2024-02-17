#include "PistonMachine.h"
#include "ApplicationClock.h"
#include "Easing.h"
#include "Shape.h"
#include "BoxCollider3D.h"
#include "Scene.h"

// Interval of pushed out
const float PISTON_INTERVAL = 1.0f;
// Time of pushed out
const float PUSHEDOUT_TIME = 0.1f;
// Cool time of a piston machine
const float PISTON_COOLTIME = 0.5f;
// Return time of a piston machine
const float PISTON_RETURNTIME = 0.5f;

// Power of pushed out
const float POWER_PUSHEDOUT = 1.25f;
// High power of pushed out
const float POWER_PUSHEDOUT_HIGH = 2.0f;

// Constructor
CPistonMachineComponent::CPistonMachineComponent(CGameObject* owner, float power, float coolTime)
    : ACComponent(owner)
    , m_pushedOutPower(power)
    , m_coolTime(coolTime)
{ }

// Start processing
void CPistonMachineComponent::Start() {
    m_initPos = GetTransform().pos;
    m_timeCnt = 0.0f;
    m_isPushedOut = false;

    // Create black box object
    auto obj = GetScene()->CreateGameObject(Transformf(GetTransform().pos, GetTransform().scale - Vector3f::Ones() * 0.05f, GetTransform().rotation));
    obj->AddComponent<CColorOnlyShape>(GraphicsLayer::Standard, ShapeKind::Box, Colorf(0.0f, 0.0f, 0.0f, 1.0f));
}

// Update processing
void CPistonMachineComponent::Update() {
    m_timeCnt += CAppClock::GetMain().GetDeltaTime();

    if (!m_isPushedOut) {
        // Check pushed out time
        if (m_timeCnt >= PISTON_INTERVAL) {

            // Pushed out this object using easing
            float time = m_timeCnt - PISTON_INTERVAL;
            Vector3f outPos = m_initPos + GetTransform().GetUp() * POWER_PUSHEDOUT;
            if (time < PUSHEDOUT_TIME) {
                m_gameObj->SetPos(Easing::EaseOutQubic(time, PUSHEDOUT_TIME, m_initPos, outPos));
            }
            // Complete pushed out
            else {
                m_gameObj->SetPos(outPos);
                m_timeCnt = 0.0f;
                m_isPushedOut = true;
            }
        }
    }
    else {
        // Check cool time
        if (m_timeCnt >= PISTON_COOLTIME) {
            // Begin returnning to the initial position
            if (!m_beginReturnPos.has_value()) { 
                m_beginReturnPos = GetTransform().pos; 
            }

            // Return to the initial position using easing
            float time = m_timeCnt - PISTON_COOLTIME;
            if (time < PISTON_RETURNTIME) {
                m_gameObj->SetPos(Easing::EaseOutQubic(time, PISTON_RETURNTIME, m_beginReturnPos.value(), m_initPos));
            }
            // Complete return
            else {
                m_gameObj->SetPos(m_initPos);
                m_timeCnt = 0.0f;
                m_beginReturnPos = std::nullopt;
                m_isPushedOut = false;
            }
        }
    }
}

// Prefab for piston machine
void CPistonMachinePrefab::Prefab() {
    AddComponent<CBoxCollider3D>();
    AddComponent<CColorOnlyShape>(GraphicsLayer::ReadWriteShading, ShapeKind::Box, Colorf(0.5f, 0.5f, 0.5f, 1.0f));
    auto rb = AddComponent<CRigidBody>();
    rb->SetBodyType(RigidBodyType::Dynamic);
    rb->SetGravityScale(0.0f);
    rb->SetIsPseudoVelocity(true);
    rb->SetMass(100.0f);
    rb->SetMaterial(RigidBodyMaterial(0.5f, 1.0f));
    rb->SetRotateLock(RigidBodyAxisLock::Flag::ALL);
    rb->SetMoveLock(RigidBodyAxisLock::Flag::ALL);

    AddComponent<CPistonMachineComponent>(POWER_PUSHEDOUT, PISTON_COOLTIME);
}

// Prefab for piston machine that has high power
void CPistonMachineHighPowerPrefab::Prefab() {
    AddComponent<CBoxCollider3D>();
    AddComponent<CColorOnlyShape>(GraphicsLayer::ReadWriteShading, ShapeKind::Box, Colorf(1.0f, 0.5f, 0.5f, 1.0f));
    auto rb = AddComponent<CRigidBody>();
    rb->SetBodyType(RigidBodyType::Dynamic);
    rb->SetGravityScale(0.0f);
    rb->SetIsPseudoVelocity(true);
    rb->SetMass(100.0f);
    rb->SetMaterial(RigidBodyMaterial(0.5f, 1.0f));
    rb->SetRotateLock(RigidBodyAxisLock::Flag::ALL);
    rb->SetMoveLock(RigidBodyAxisLock::Flag::ALL);

    AddComponent<CPistonMachineComponent>(POWER_PUSHEDOUT_HIGH, 0.0f);
}
