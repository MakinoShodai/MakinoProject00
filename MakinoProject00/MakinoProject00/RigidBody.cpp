#include "RigidBody.h"
#include "Scene.h"
#include "PhysicsWorld.h"
#include "Collider3D.h"

// Gravitational constant
constexpr float GRAVITY = 9.8f;

// Constructor
CRigidBody::CRigidBody(CGameObject* owner)
    : ACComponent(owner)
    , m_simulationBodyHandle(nullptr)
    , m_rigidBodyHandle(nullptr)
    , m_moveAllowable(Vector3f::Ones())
    , m_rotateAllowable(Vector3f::Ones())
    , m_bodyType(RigidBodyType::Static)
    , m_invMass(1.0f)
    , m_gravityScale(GRAVITYSCALE_NULL)
    , m_isPseudoVelocity(true) {
    // When adding a collider, call a callback function to send a rigid body to the wrapper
    GetCallbackSystem()->AddFunction(CALLBACK_COLLIDER_AWAKE, [this]() {
        this->GetCallbackSystem()->InvokeFunction(CALLBACK_SEND_RIGIDBODY_TO_WRAPPER);
    });
}

// Awake processing
void CRigidBody::Awake() {
    // Call a callback function to send a rigid body to the wrapper
    GetCallbackSystem()->InvokeFunction(CALLBACK_SEND_RIGIDBODY_TO_WRAPPER);
}

// Start processing
void CRigidBody::Start() {
    // Add this rigid body to data protected inside the physics engine
    m_rigidBodyHandle = GetScene()->GetPhysicsWorld()->GetExclusiveData()->allRigidBodies.PushBack(WeakFromThis());
}

// Processing at destruction
void CRigidBody::OnDestroy() {
    // Release own simulation body from data protected inside the physics engine
    if (m_simulationBodyHandle) {
        GetScene()->GetPhysicsWorld()->GetExclusiveData()->simulationBodies.Erase(*m_simulationBodyHandle);
        m_simulationBodyHandle = nullptr;
    }

    // Release this rigid body from data protected inside the physics engine
    if (m_rigidBodyHandle) {
        GetScene()->GetPhysicsWorld()->GetExclusiveData()->allRigidBodies.Erase(*m_rigidBodyHandle);
        m_rigidBodyHandle = nullptr;
    }
}

// Processing when this component is disabled
void CRigidBody::OnDisable() {
    // Clear all velocities
    if (m_simulationBodyHandle) {
        GetSimulation()->ClearAllVelocities();
    }
}

// Add force to this rigid body
void CRigidBody::AddForce(const Vector3f& force, ForceMode mode) {
    if (m_simulationBodyHandle == nullptr || false == IsActiveOverall()) { return; }
    GetSimulation()->AddForce(force, mode);
}

// Add torque to this rigid body
void CRigidBody::AddTorque(const Vector3f& torque, ForceMode mode) {
    if (m_simulationBodyHandle == nullptr || false == IsActiveOverall()) { return; }
    GetSimulation()->AddTorque(torque, mode);
}

// Bind this collider to wrapper
void CRigidBody::BindWrapper(Mkpe::CColliderWrapper* wrapper) {
    // If no wrapper is sent, do nothing
    if (wrapper == nullptr) { return; }

    // Set this rigid body and own simulation body to the wrapper
    wrapper->SetRigidBody(WeakFromThis());
    wrapper->SetSimulationBody(GetSimulation());
}

// Set axis lock for movement
void CRigidBody::SetMoveLock(RigidBodyAxisLock::Bit bit) {
    // Axis to be locked is set to 0.
    m_moveAllowable.x() = (bit & RigidBodyAxisLock::Flag::_X) ? 0.0f : 1.0f;
    m_moveAllowable.y() = (bit & RigidBodyAxisLock::Flag::_Y) ? 0.0f : 1.0f;
    m_moveAllowable.z() = (bit & RigidBodyAxisLock::Flag::_Z) ? 0.0f : 1.0f;
}

// Set axis lock for rotation
void CRigidBody::SetRotateLock(RigidBodyAxisLock::Bit bit) {
    // Axis to be locked is set to 0.
    m_rotateAllowable.x() = (bit & RigidBodyAxisLock::Flag::_X) ? 0.0f : 1.0f;
    m_rotateAllowable.y() = (bit & RigidBodyAxisLock::Flag::_Y) ? 0.0f : 1.0f;
    m_rotateAllowable.z() = (bit & RigidBodyAxisLock::Flag::_Z) ? 0.0f : 1.0f;
}

void CRigidBody::SetIsPseudoVelocity(bool isPseudoVelocity) {
    if (false == m_isPseudoVelocity && isPseudoVelocity) {
        m_isPseudoVelocity = true;

        // Update prev transform
        if (m_simulationBodyHandle) {
            GetSimulation()->UpdatePrevTransform();
        }
    }
    else if (m_isPseudoVelocity && false == isPseudoVelocity) {
        m_isPseudoVelocity = false;

        // Clear pseudo velocities
        if (m_simulationBodyHandle) {
            GetSimulation()->ClearPseudoVelocities();
        }
    }
}

// Set rigid body type
void CRigidBody::SetBodyType(RigidBodyType bodyType) {
    // Get if the body type was static before changing
    bool isPrevStatic = IsStatic();
    // Change body type
    m_bodyType = bodyType;
    // Get if the body type was static after changing
    bool isStatic = IsStatic();

    // Create a simulation body if changed from a static object.
    if (isPrevStatic && !isStatic && m_simulationBodyHandle == nullptr) {
        m_simulationBodyHandle = GetScene()->GetPhysicsWorld()->GetExclusiveData()->simulationBodies.PushBack(CUniquePtrWeakable<Mkpe::CSimulationBody>::Make(this));

        // Update gravity of the simulation body
        SetSimulationGravity();

        // Call a callback function to send a rigid body to the wrapper
        GetCallbackSystem()->InvokeFunction(CALLBACK_SEND_SIMULATIONBODY_TO_WRAPPER);
    }
    // If a non-static object is converted to a static object, discard the computational rigid body.
    else if (!isPrevStatic && isStatic && m_simulationBodyHandle != nullptr) {
        GetScene()->GetPhysicsWorld()->GetExclusiveData()->simulationBodies.Erase(*m_simulationBodyHandle);
        m_simulationBodyHandle = nullptr;
    }
}

// Set mass
void CRigidBody::SetMass(float mass) {
    m_invMass = 1.0f / mass;

    // Update gravity of the simulation body
    SetSimulationGravity();
}

// Set gravity scale
void CRigidBody::SetGravityScale(float gravityScale) {
    m_gravityScale = gravityScale;

    // Update gravity of the simulation body
    SetSimulationGravity();
}

// Get own simulation body
CWeakPtr<Mkpe::CSimulationBody> CRigidBody::GetSimulation() {
    return (m_simulationBodyHandle) ? GetScene()->GetPhysicsWorld()->GetExclusiveData()->simulationBodies[*m_simulationBodyHandle].GetWeakPtr() : CWeakPtr<Mkpe::CSimulationBody>();
}

// Get own simulation body
CWeakPtr<const Mkpe::CSimulationBody> CRigidBody::GetSimulation() const {
    return (m_simulationBodyHandle) ? GetScene()->GetPhysicsWorld()->GetExclusiveData()->simulationBodies[*m_simulationBodyHandle].GetWeakPtr() : CWeakPtr<Mkpe::CSimulationBody>();
}

// Set gravity to own simulation body
void CRigidBody::SetSimulationGravity() {
    // Update gravity of the simulation body
    if (m_simulationBodyHandle != nullptr) {
        float mass = GetMass();
        // #NOTE : The inverse of the mass is multiplied when applying gravity, so the mass must be multiplied by default
        // If gravity multiplier is invalid, use gravity * mass
        if (m_gravityScale < -FLT_EPSILON) {
            GetSimulation()->SetGravity(GRAVITY * mass * mass);
        }
        // Use gravity * gravity multiplier if gravity multiplier is enabled
        else {
            GetSimulation()->SetGravity(GRAVITY * m_gravityScale * mass);
        }
    }
}

// Rigid body material constructor
RigidBodyMaterial::RigidBodyMaterial(float friction, float restitution)
    : friction(friction)
    , restitution(restitution)
{}
