#include "ForPhysicsObjects.h"
#include "Scene.h"
#include "AssetNameDefine.h"
#include "ApplicationClock.h"
#include "Random.h"

// Interval of generating a physics sample prefab
const float GENERATE_INTERVAL = 5.0f;
// Maximum number of generating physics sample prefabs
const UINT GENERATE_MAX = 100;

// Initialize static member
UINT CPhysicsSamplePrefabGenerateComponent::ms_currentGeneratedNum = 0;

// Start processing
void CPhysicsSamplePrefabGenerateComponent::Start() {
    m_timeCnt = GENERATE_INTERVAL + 0.01f;
}

// Update processing
void CPhysicsSamplePrefabGenerateComponent::Update() {
    m_timeCnt += CAppClock::GetMain().GetAppropriateDeltaTime();
    if (m_timeCnt >= GENERATE_INTERVAL) {
        m_timeCnt = 0.0f;
        if (ms_currentGeneratedNum < GENERATE_MAX) {
            ms_currentGeneratedNum++;

            // Generate prefab
            CRandom& random = CRandom::GetMain();
            auto prefab = GetScene()->CreateGameObject(
                PREFAB_NAMES_PHYSICS_SAMPLE[random.GenerateRange(0, 2)],
                Transformf(
                    GetTransform().pos,
                    GetTransform().scale,
                    Vector3f((float)random.GenerateRange(0, 359), (float)random.GenerateRange(0, 359), (float)random.GenerateRange(0, 359)) * Utl::DEG_2_RAD
                ));
            prefab->AddComponent<CPhysicsSampleCountComponent>();
        }
    }
}

// Process to be called at instance destruction
void CPhysicsSampleCountComponent::OnDestroy() {
    CPhysicsSamplePrefabGenerateComponent::DecrementCurrentGeneratedNum();
}

// Update processing
void CAutoDestroyComponent::Update() {
    if (GetTransform().pos.y() <= -100.0f) {
        GetScene()->DestroyGameObject(m_gameObj);
    }
}

// Prefab function
void CPhysicsSampleGeneraterPrefab::Prefab() {
    AddComponent<CPhysicsSamplePrefabGenerateComponent>();
}

// Sphere prefab for physics sample
void CPhysicsSampleSpherePrefab::Prefab() {
    AddComponent<CSphereCollider3D>();
    AddComponent<CTexShape>(GraphicsLayer::ReadWriteShading, ShapeKind::Sphere, TexName::CHECKERBOARD_TEX);
    auto rb = AddComponent<CRigidBody>();
    rb->SetBodyType(RigidBodyType::Dynamic);
    rb->SetGravityScale(1.0f);
    rb->SetMass(1.0f);
    rb->SetMaterial(RigidBodyMaterial(0.5f, 0.0f));

    AddComponent<CAutoDestroyComponent>();
}

// Capsule prefab for physics sample
void CPhysicsSampleCapsulePrefab::Prefab() {
    AddComponent<CCapsuleCollider3D>();
    AddComponent<CTexShape>(GraphicsLayer::ReadWriteShading, ShapeKind::Capsule, TexName::CHECKERBOARD_TEX);
    auto rb = AddComponent<CRigidBody>();
    rb->SetBodyType(RigidBodyType::Dynamic);
    rb->SetGravityScale(1.0f);
    rb->SetMass(1.0f);
    rb->SetMaterial(RigidBodyMaterial(0.5f, 0.0f));

    AddComponent<CAutoDestroyComponent>();
}

// Box prefab for physics sample
void CPhysicsSampleBoxPrefab::Prefab() {
    AddComponent<CBoxCollider3D>();
    AddComponent<CTexShape>(GraphicsLayer::ReadWriteShading, ShapeKind::Box, TexName::CHECKERBOARD_TEX);
    auto rb = AddComponent<CRigidBody>();
    rb->SetBodyType(RigidBodyType::Dynamic);
    rb->SetGravityScale(1.0f);
    rb->SetMass(1.0f);
    rb->SetMaterial(RigidBodyMaterial(0.5f, 0.0f));

    AddComponent<CAutoDestroyComponent>();
}