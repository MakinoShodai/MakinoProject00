#include "CollisionAlgorithm.h"
#include "CollisionAlgorithmCapsuleCapsule.h"
#include "CollisionAlgorithmSphereSphere.h"
#include "CollisionAlgorithmBoxSphere.h"
#include "CollisionAlgorithmBoxBox.h"
#include "GJKAlgorithm.h"
#include "BVOverlapPair.h"
#include "Scene.h"

// Broad phase collision algorithm
void Mkpe::CCollisionAlgorithm::BroadContact(Dbvt::BVOverlapPair* pair) {
    // Get wrappers and colliders that pair contains
    CColliderWrapper* wrapperA = pair->GetWrapperA();
    CColliderWrapper* wrapperB = pair->GetWrapperB();
    CWeakPtr<ACCollider3D> colliderA = wrapperA->GetCollider();
    CWeakPtr<ACCollider3D> colliderB = wrapperB->GetCollider();

    pair->contactData.ClearContactPoint();

    // If no collision determination is needed, exit
    if (false == colliderA->CheckTagEither(colliderB.Get())              // Tag-to-tag with no contact?
        || (!wrapperA->IsActive() || !wrapperB->IsActive())              // Are either of the two inactive?
        || (wrapperA->IsStatic() && wrapperB->IsStatic())                // Are both static?
        || (colliderA->GetGameObject() == colliderB->GetGameObject())) { // Are the owned objects the same?
        return;
    }

    // Get type of colliders
    ColliderType typeA = colliderA->GetColliderType();
    ColliderType typeB = colliderB->GetColliderType();

    // Call the appropriate function
    (*m_algorithmFunc[(UINT)typeA][(UINT)typeB])(pair);
}

// Constructor
Mkpe::CCollisionAlgorithm::CCollisionAlgorithm()
    : ACMainThreadSingleton(0)
    , m_algorithmFunc{} {
    // Sphere - Sphere
    m_algorithmFunc[(UINT)ColliderType::Sphere][(UINT)ColliderType::Sphere] = CollisionAlgorithm::SphereSphere;
    // Box - Sphere
    m_algorithmFunc[(UINT)ColliderType::Box][(UINT)ColliderType::Sphere] = CollisionAlgorithm::BoxSphere;
    m_algorithmFunc[(UINT)ColliderType::Sphere][(UINT)ColliderType::Box] = CollisionAlgorithm::BoxSphere;
    // Box - Box
    m_algorithmFunc[(UINT)ColliderType::Box][(UINT)ColliderType::Box] = CollisionAlgorithm::BoxBox;
    // Box - Capsule
    m_algorithmFunc[(UINT)ColliderType::Box][(UINT)ColliderType::Capsule] = CollisionAlgorithm::GJKAlgorithm;
    m_algorithmFunc[(UINT)ColliderType::Capsule][(UINT)ColliderType::Box] = CollisionAlgorithm::GJKAlgorithm;
    // Capsule - Capsule
    m_algorithmFunc[(UINT)ColliderType::Capsule][(UINT)ColliderType::Capsule] = CollisionAlgorithm::CapsuleCapsule;
    // Capsule - Sphere
    m_algorithmFunc[(UINT)ColliderType::Capsule][(UINT)ColliderType::Sphere] = CollisionAlgorithm::CapsuleSphere;
    m_algorithmFunc[(UINT)ColliderType::Sphere][(UINT)ColliderType::Capsule] = CollisionAlgorithm::CapsuleSphere;
}
