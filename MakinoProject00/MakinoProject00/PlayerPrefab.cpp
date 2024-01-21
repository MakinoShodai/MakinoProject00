#include "PlayerPrefab.h"
#include "Model.h"
#include "AssetNameDefine.h"
#include "CapsuleCollider3D.h"
#include "RigidBody.h"
#include "PlayerControlComponent.h"
#include "PlayerAnimControlComponent.h"

// Prefab function
void CPlayerPrefab::Prefab() {
    // Set the name to this object
    SetName(OBJNAME_PLAYER);

    // Add movel
    AddComponent<CSkeletalModel>(GraphicsLayer::Standard, ModelName::CUTEBIRD)->GetController()->Play(1, true);
    // Add collider
    AddComponent<CCapsuleCollider3D>(2.0f, 0.9f, Vector3f(0.0f, 1.0f, 0.0f));
    // Add rigid body
    auto rb = AddComponent<CRigidBody>();
    rb->SetGravityScale(2.0f);
    rb->SetBodyType(RigidBodyType::Dynamic);
    rb->SetRotateLock(RigidBodyAxisLock::Flag::_X | RigidBodyAxisLock::Flag::_Z);
    rb->SetMaterial(RigidBodyMaterial(0.0f, 0.0f));
    rb->SetIsPseudoVelocity(true);

    // Add player specific components
    AddComponent<CPlayerControlComponent>();

    AddComponent<CPlayerAnimControlComponent>(); // This component is added at the end
}
