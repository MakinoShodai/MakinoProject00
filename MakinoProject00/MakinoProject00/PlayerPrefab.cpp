#include "PlayerPrefab.h"
#include "Model.h"
#include "AssetNameDefine.h"
#include "SphereCollider3D.h"
#include "RigidBody.h"
#include "PlayerControlComponent.h"
#include "PlayerAnimControlComponent.h"

// Prefab function
void CPlayerPrefab::Prefab() {
    // Set the name to this object
    SetName(OBJNAME_PLAYER);

    // Add model
    AddComponent<CSkeletalModel>(GraphicsLayer::ReadWriteShading, ModelName::CUTEBIRD);
    // Add collider
    AddComponent<CSphereCollider3D>(1.0f, Vector3f(0.0f, 1.0f, 0.0f));
    // Add rigid body
    auto rb = AddComponent<CRigidBody>();
    rb->SetGravityScale(2.0f);
    rb->SetBodyType(RigidBodyType::Dynamic);
    rb->SetRotateLock(RigidBodyAxisLock::Flag::_X | RigidBodyAxisLock::Flag::_Y | RigidBodyAxisLock::Flag::_Z);
    rb->SetMaterial(RigidBodyMaterial(0.0f, 0.0f));
    rb->SetIsPseudoVelocity(true);
    rb->SetLinearDrag(5.0f);

    // Add player specific components
    AddComponent<CPlayerControlComponent>();

    AddComponent<CPlayerAnimControlComponent>(); // This component is added at the end
}
