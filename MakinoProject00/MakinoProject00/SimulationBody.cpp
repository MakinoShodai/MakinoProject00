#include "SimulationBody.h"
#include "RigidBody.h"
#include "ApplicationClock.h"
#include "PhysicsWorld.h"

// Apply tolerance vector to linear velocity
#define LINEAR_APPLY_TOLERANCE(linear) (linear * m_rb->GetMoveAllowable())
// Apply tolerance vector to angular velocity
#define ANGULAR_APPLY_TOLERANCE(angular) (angular * m_rb->GetRotateAllowable())
// Convert force to linear velocity
#define FORCE_2_VELOCITY(force) LINEAR_APPLY_TOLERANCE(force * m_rb->GetInvMass())
// Convert torque to angular velocity
#define TORQUE_2_VELOCITY(torque) ANGULAR_APPLY_TOLERANCE(m_invInertiaTensor * torque)

// Constructor
Mkpe::CSimulationBodyBase::CSimulationBodyBase(CRigidBody* rb)
    : m_rb(rb)
    , m_totalLinearVelocity(Vector3f::Zero())
    , m_totalAngularVelocity(Vector3f::Zero())
    , m_linearVelocity(Vector3f::Zero())
    , m_angularVelocity(Vector3f::Zero())
    , m_deltaLinearVelocity(Vector3f::Zero())
    , m_deltaAngularVelocity(Vector3f::Zero())
    , m_penetrationLinearVelocity(Vector3f::Zero())
    , m_penetrationAngularVelocity(Vector3f::Zero())
    , m_pseudoLinearVelocityPerFrame(Vector3f::Zero())
    , m_pseudoAngularVelocityPerFrame(Vector3f::Zero())
    , m_pseudoLinearVelocityPerSimulation(Vector3f::Zero())
    , m_pseudoAngularVelocityPerSimulation(Vector3f::Zero())
    , m_invInertiaTensor(Matrix3x3f::Identity())
    , m_invInertiaLocal(Vector3f::Zero())
    , m_gravity(0.0f)
{
    if (rb) {
        m_prevPos = rb->GetTransform().pos;
        m_prevRotation = rb->GetTransform().rotation;
    }
}

// Get allowable vector for movement
const Vector3f& Mkpe::CSimulationBodyBase::GetMoveAllowable() const {
    return (m_rb) ? m_rb->GetMoveAllowable() : Vector3f::Ones();
}

// Get allowable vector for rotation
const Vector3f& Mkpe::CSimulationBodyBase::GetRotateAllowable() const {
    return (m_rb) ? m_rb->GetRotateAllowable() : Vector3f::Ones();
}

// Apply velocity for Solving velocity at contact
void Mkpe::CSimulationBody::SolveVelocityAtContact(const Vector3f& linearVelocity, const Vector3f& angularVelocity, float impulseMagnitude) {
    m_deltaLinearVelocity += LINEAR_APPLY_TOLERANCE(linearVelocity * impulseMagnitude);
    m_deltaAngularVelocity += ANGULAR_APPLY_TOLERANCE(angularVelocity * impulseMagnitude);
}

// Apply velocity for Solving penetration at contact
void Mkpe::CSimulationBody::SolvePenetrationAtContact(const Vector3f& linearVelocity, const Vector3f& angularVelocity, float impulseMagnitude) {
    m_penetrationLinearVelocity += LINEAR_APPLY_TOLERANCE(linearVelocity * impulseMagnitude);
    m_penetrationAngularVelocity += ANGULAR_APPLY_TOLERANCE(angularVelocity * impulseMagnitude);
}

// Add force to linear velocity
void Mkpe::CSimulationBody::AddForce(const Vector3f& force, ForceMode mode) {
    switch (mode) {
        // Continuous application of force
    case ForceMode::Force:
        m_linearVelocity += FORCE_2_VELOCITY(force * CAppClock::GetMain().GetAppropriateDeltaTime());
        break;
        // Instantaneous application of force
    case ForceMode::Impulse:
        m_linearVelocity += FORCE_2_VELOCITY(force);
        break;
    }
}

// Add torque to angular velocity
void Mkpe::CSimulationBody::AddTorque(const Vector3f& torque, ForceMode mode) {
    switch (mode) {
        // Continuous application of force
    case ForceMode::Force:
        m_angularVelocity += TORQUE_2_VELOCITY(torque * CAppClock::GetMain().GetAppropriateDeltaTime());
        break;
        // Instantaneous application of force
    case ForceMode::Impulse:
        m_angularVelocity += TORQUE_2_VELOCITY(torque);
        break;
    }
}

// Clear all velocities updated every frame
void Mkpe::CSimulationBody::ClearVelocitiesEveryFrame() {
    m_pseudoLinearVelocityPerFrame  = Vector3f::Zero();
    m_pseudoAngularVelocityPerFrame = Vector3f::Zero();
}

// Clear all velocities updated per physics simulation
void Mkpe::CSimulationBody::ClearVelocitiesPerSimulation() {
    m_deltaLinearVelocity                = Vector3f::Zero();
    m_deltaAngularVelocity               = Vector3f::Zero();
    m_penetrationLinearVelocity          = Vector3f::Zero();
    m_penetrationAngularVelocity         = Vector3f::Zero();
    m_pseudoLinearVelocityPerSimulation  = Vector3f::Zero();
    m_pseudoAngularVelocityPerSimulation = Vector3f::Zero();
}

// Clear all velocities
void Mkpe::CSimulationBody::ClearAllVelocities() {
    m_linearVelocity  = Vector3f::Zero();
    m_angularVelocity = Vector3f::Zero();
    ClearVelocitiesEveryFrame();
    ClearVelocitiesPerSimulation();
}

// Clear pseudo velocities
void Mkpe::CSimulationBody::ClearPseudoVelocities() {
    m_pseudoLinearVelocityPerFrame = Vector3f::Zero();
    m_pseudoAngularVelocityPerFrame = Vector3f::Zero();
    m_pseudoLinearVelocityPerSimulation = Vector3f::Zero();
    m_pseudoAngularVelocityPerSimulation = Vector3f::Zero();
}

// Merge pseudo velocity and regular velocity
void Mkpe::CSimulationBody::MergeVelocities() {
    m_totalLinearVelocity  = m_linearVelocity  + m_pseudoLinearVelocityPerFrame  + m_pseudoLinearVelocityPerSimulation;
    m_totalAngularVelocity = m_angularVelocity + m_pseudoAngularVelocityPerFrame + m_pseudoAngularVelocityPerSimulation;
}

// Decay velocity
void Mkpe::CSimulationBody::DecayVelocity() {
    float timeStep = Mkpe::CPhysicsWorld::GetTimeStep();
    m_linearVelocity *= Utl::Clamp(1.0f - timeStep, 0.0f, 1.0f);
    m_angularVelocity *= Utl::Clamp(1.0f - timeStep, 0.0f, 1.0f);
}

// Apply velocity to transform
void Mkpe::CSimulationBody::UpdateTransform() {
    // Angular velocity due to penetration solving applies a little
    constexpr float PENETRACTION_ANGULAR_ERP = 0.1f;
    // Ignore if it moves less than this value in 1 second
    constexpr float IGNORE_MOVE_1SEC = 0.1f;
    // Ignore if it rotates less than this value in 1 second
    constexpr float IGNORE_ROTATE_1SEC = Utl::DEG_2_RAD * 1.0f;

    // Apply force due to velocity solving
    m_linearVelocity += m_deltaLinearVelocity;
    m_angularVelocity += m_deltaAngularVelocity;

    // Get coordinates and rotation before change
    const Transformf& transform = m_rb->GetTransform();
    Vector3f linear = (m_linearVelocity + m_penetrationLinearVelocity);
    Vector3f angular = (m_angularVelocity + m_penetrationAngularVelocity * PENETRACTION_ANGULAR_ERP);

    // Update if coordinate is moving
    if (std::abs(linear.x()) > IGNORE_MOVE_1SEC || std::abs(linear.y()) > IGNORE_MOVE_1SEC || std::abs(linear.z()) > IGNORE_MOVE_1SEC) {
        m_rb->GetGameObject()->SetPos(transform.pos + linear * Mkpe::CPhysicsWorld::GetTimeStep());
    }

    // Update if rotation is changing
    float angularLength = angular.Length();
    if (angularLength > IGNORE_ROTATE_1SEC) {
        m_rb->GetGameObject()->SetRotation(Quaternionf(angularLength * Mkpe::CPhysicsWorld::GetTimeStep(), angular.GetNormalize()) * transform.rotation);
    }
}

// Apply gravity to linear velocity
void Mkpe::CSimulationBody::ApplyGravity() {
    // If it's not dynamic rigid body, do nothing
    if (m_rb->GetBodyType() != RigidBodyType::Dynamic) { return; }

    // Apply gravity
    m_linearVelocity.y() -= m_gravity * m_rb->GetMoveAllowable().y() * CAppClock::GetMain().GetDeltaTime();
}

// Calculate the velocity of point p in the object
Vector3f Mkpe::CSimulationBody::GetVelocityOfPointInObject(const Vector3f& p) const {
    return m_totalLinearVelocity + m_totalAngularVelocity.Cross(p);
}

// Initialize inertia tensor
void Mkpe::CSimulationBody::InitializeInertiaTensor(ACCollider3D* collider) {
    // Recalculate the inverse of the local inertia tensor
    collider->GenerateInvInertiaTensorLocal(&m_invInertiaLocal);

    // Get rotate matrix
    Matrix3x3f mat = collider->GetTransform().rotation.GetMatrix();
    // Convert local inertia tensor to 3x3 matrix
    Matrix3x3f invInertiaTensorLocal = m_invInertiaLocal.GetDiagonalMatrix();
    // Apply rotation to inertia tensor
    m_invInertiaTensor = mat * invInertiaTensorLocal * mat.Transpose();
}

// Processing of any scaling or rotation changes
void Mkpe::CSimulationBody::CheckScalingRotation(ACCollider3D* collider) {
    CGameObject* gameObject = m_rb->GetGameObject().Get();

    // If the scale is changing
    bool isScale = Mkpe::CPhysicsWorld::CheckTransformObserveForScale(gameObject);
    if (isScale) {
        // Recalculate the inverse of the local inertia tensor
        collider->GenerateInvInertiaTensorLocal(&m_invInertiaLocal);
    }

    // If the scale or the rotation is changing
    if (isScale || Mkpe::CPhysicsWorld::CheckTransformObserveForRotate(gameObject)) {
        // Get rotate matrix
        Matrix3x3f mat = collider->GetTransform().rotation.GetMatrix();

        // Convert local inertia tensor to 3x3 matrix
        Matrix3x3f invInertiaTensorLocal = m_invInertiaLocal.GetDiagonalMatrix();

        // Apply rotation to inertia tensor
        m_invInertiaTensor = mat * invInertiaTensorLocal * mat.Transpose();
    }
}

// Compute pseudo velocity in current time per frame
void Mkpe::CSimulationBody::ComputePseudoVelocityPerFrame(float invTimeStep) {
    // If you don't use pseudo-velocity, do nothing
    if (!m_rb->IsPseudoVelocity()) { return; }

    CGameObject* gameObj = m_rb->GetGameObject().Get();

    // If the position is changing
    if (Mkpe::CPhysicsWorld::CheckTransformObserveForPos(gameObj)) {
        // Calculate the amount of differential position from the previous time
        Vector3f diff = gameObj->GetTransform().pos - m_prevPos;

        // Calculate pseudo linear velocity
        m_pseudoLinearVelocityPerFrame = diff * invTimeStep;

        // Apply the difference to the previous position
        m_prevPos += diff;
    }
    // If the rotation is changing
    if (Mkpe::CPhysicsWorld::CheckTransformObserveForRotate(gameObj)) {
        // Calculate the amount of differential rotation from the previous time
        Quaternionf diff = gameObj->GetTransform().rotation * m_prevRotation.GetConjugation();

        // Calculate pseudo angular velcity
        m_pseudoAngularVelocityPerFrame = diff.GetAngularVelocity() * invTimeStep;

        // Apply the difference to the previous position
        m_prevRotation *= diff;
    }
}

// Compute pseudo velocity in current time per simulation
void Mkpe::CSimulationBody::ComputePseudoVelocityPerSimulation(float invTimeStep) {
    // If you don't use pseudo-velocity, do nothing
    if (!m_rb->IsPseudoVelocity()) { return; }

    CGameObject* gameObj = m_rb->GetGameObject().Get();

    // If the position is changing
    if (Mkpe::CPhysicsWorld::CheckTransformObserveForPos(gameObj)) {
        // Calculate the amount of differential position from the previous time
        Vector3f diff = gameObj->GetTransform().pos - m_prevPos;

        // Calculate pseudo linear velocity
        m_pseudoLinearVelocityPerSimulation = diff * invTimeStep;
    }
    // If the rotation is changing
    if (Mkpe::CPhysicsWorld::CheckTransformObserveForRotate(gameObj)) {
        // Calculate the amount of differential rotation from the previous time
        Quaternionf diff = gameObj->GetTransform().rotation * m_prevRotation.GetConjugation();

        // Calculate pseudo angular velcity
        m_pseudoAngularVelocityPerSimulation = diff.GetAngularVelocity() * invTimeStep;
    }
}

// Update transform in previous time of physics simulation
void Mkpe::CSimulationBody::UpdatePrevTransform() {
    // If you don't use pseudo-velocity, do nothing
    if (!m_rb->IsPseudoVelocity()) { return; }

    CGameObject* gameObj = m_rb->GetGameObject().Get();
    m_prevPos = gameObj->GetTransform().pos;
    m_prevRotation = gameObj->GetTransform().rotation;
}

// Is this simulation body active?
bool Mkpe::CSimulationBody::IsActive() const {
    return m_rb->IsActiveOverall();
}
