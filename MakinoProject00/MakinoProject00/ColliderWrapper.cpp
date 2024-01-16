#include "ColliderWrapper.h"
#include "Scene.h"

// Initialize static member
const RigidBodyMaterial Mkpe::CColliderWrapper::ms_defaultRbMaterial = RigidBodyMaterial();
Mkpe::CSimulationBodyBase Mkpe::CColliderWrapper::ms_staticSimulationBody  = CSimulationBodyBase(nullptr);

// Constructor
Mkpe::CColliderWrapper::CColliderWrapper(CWeakPtr<ACCollider3D> collider)
    : m_simulationBody(nullptr)
    , m_collider(collider)
    , m_rigidBody(nullptr)
    , m_thisTreeType(Mkpe::Dbvt::TREETYPE_MAX)
    , m_thisNode(nullptr)
{}

// Destructor
Mkpe::CColliderWrapper::~CColliderWrapper() {
}
