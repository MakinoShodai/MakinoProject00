#include "Collider3D.h"
#include "Scene.h"
#include "RigidBody.h"
#include "ColliderWrapper.h"
#include "PhysicsWorld.h"

// Constructor
ACCollider3D::ACCollider3D(CGameObject* owner, ColliderType type, const Vector3f& offset)
    : ACComponent(owner)
    , m_type(type)
    , m_offset(offset)
    , m_scalingOffset(offset)
    , m_tag(ColliderTag::Empty)
    , m_isOverlap(false) {
    // Ensure the correct type of collider
    assert(type != ColliderType::Max);

    // Create and add callback function when rigid body is added
    CCallbackSystem* callbackSystem = m_gameObj->GetCallbackSystem();
    callbackSystem->AddFunction(CALLBACK_SEND_RIGIDBODY_TO_WRAPPER, [this]() {
        // Get own rigid body
        CRigidBody* rb = this->m_gameObj->GetComponent<CRigidBody>().Get();

        // If own wrapper has not been created yet, create own wrapper and set its handle to it
        if (!m_wrapperHandle) {
            m_wrapperHandle = GetScene()->GetPhysicsWorld()->GetExclusiveData()->allWrappers.PushBack(CUniquePtrWeakable<Mkpe::CColliderWrapper>::Make(WeakFromThis()));
        }

        // Bind the rigid body to this collider wrapper
        rb->BindWrapper(GetWrapper());

        // Erase this callback function from callback system
        this->m_gameObj->GetCallbackSystem()->RemoveCurrentInvokeFunction(CALLBACK_SEND_RIGIDBODY_TO_WRAPPER);
    });

    // Create and add callback function for sending simulation body
    callbackSystem->AddFunction(CALLBACK_SEND_SIMULATIONBODY_TO_WRAPPER, [this]() {
        // Get own rigid body and wrapper of this collider
        CRigidBody* rb = this->m_gameObj->GetComponent<CRigidBody>().Get();
        Mkpe::CColliderWrapper* wrapper = GetWrapper();

        // Bind the simulation body to this collider wrapper
        rb->BindWrapper(wrapper);

        // Update each variables
        UpdateScalingOffset();
        UpdateLocalBoudingVolume();
        UpdateScaling();
        wrapper->InitializeInertiaTensor();
    });

}

// Awake processing
void ACCollider3D::Awake() {
    // If own wrapper has not been created yet, create own wrapper and set its handle to it
    if (!m_wrapperHandle) {
        m_wrapperHandle = GetScene()->GetPhysicsWorld()->GetExclusiveData()->allWrappers.PushBack(CUniquePtrWeakable<Mkpe::CColliderWrapper>::Make(WeakFromThis()));
    }

    // Call callback function at collider initial processing
    m_gameObj->GetCallbackSystem()->InvokeFunction(CALLBACK_COLLIDER_AWAKE);
}

// Start processing
void ACCollider3D::Start() {
    // Update scaling-related values
    UpdateScaling();
    UpdateLocalBoudingVolume();

    // Add own leaf node to the bounding volume tree
    GetScene()->GetPhysicsWorld()->GetBVTree()->AddLeafNode(GetWrapper());
}

// Processing at destruction
void ACCollider3D::OnDestroy() {
    // Get physics simulation environment class
    Mkpe::CPhysicsWorld* physicsWorld = GetScene()->GetPhysicsWorld();

    // Remove own leaf node from the bounding volume tree
    physicsWorld->GetBVTree()->RemoveLeafNode(GetWrapper());
    // Release wrapper for this collider
    physicsWorld->GetExclusiveData()->allWrappers.Erase(*m_wrapperHandle);
}

// Generate bounding volume
Mkpe::BoundingVolume ACCollider3D::GenerateBoudingVolume() const {
    return Mkpe::BoundingVolume(GetCenter(), m_localBV);
}

// Check if scaling and rotation updates are needed, update if necessary
void ACCollider3D::CheckScalingRotation() {
    // If scale has changed
    if (Mkpe::CPhysicsWorld::CheckTransformObserveForScale(m_gameObj.Get())) {
        // If Scale or Rotation has changed, offset and BV is updated
        if (Mkpe::CPhysicsWorld::CheckTransformObserveForRotate(m_gameObj.Get())) {
            UpdateScalingOffset();
            UpdateLocalBoudingVolume();
        }

        // Update scaling variables in derived classes
        UpdateScaling();
    }
}

// Is the ID sent the same as my leaf node ID?
bool ACCollider3D::CheckMyLeafNodeID(Mkpe::Dbvt::NodeID id) const {
    return GetWrapper()->CheckMyLeafNodeID(id);
}

// Get the maximum extent to which a vertex is shifted by rotation
float ACCollider3D::GetMaxRotateScalar() const {
    return (m_localBV.maxAABB - m_localBV.minAABB).Length() * Utl::Inv::_2;
}

// Set the offset of the center coordinates of the collider
void ACCollider3D::SetOffset(const Vector3f& offset) {
    m_offset = offset;
    
    // Update offset and BV
    UpdateScalingOffset();
    UpdateLocalBoudingVolume();
}

// Get list class of the contact colliders for each categories
const Mkpe::CContactColliderList& ACCollider3D::GetContactColliderdLists() const {
    return GetWrapper()->GetContactColliderList();
}

// Get this collider wrapper
Mkpe::CColliderWrapper* ACCollider3D::GetWrapper() {
    return GetScene()->GetPhysicsWorld()->GetExclusiveData()->allWrappers[*m_wrapperHandle].Get();
}
// Get this collider wrapper
const Mkpe::CColliderWrapper* ACCollider3D::GetWrapper() const {
    return GetScene()->GetPhysicsWorld()->GetExclusiveData()->allWrappers[*m_wrapperHandle].Get();
}

// Update the scaling offset of the center coordinates of the collider
void ACCollider3D::UpdateScalingOffset() {
    // Get transform
    const Transformf& transform = GetTransform();

    // Apply rotation and scaling to my offset
    m_scalingOffset = transform.rotation * (m_offset * transform.scale);
}
