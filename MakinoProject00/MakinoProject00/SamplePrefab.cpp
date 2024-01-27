#include "SamplePrefab.h"
#include "BoxCollider3D.h"
#include "SphereCollider3D.h"
#include "CapsuleCollider3D.h"
#include "RigidBody.h"
#include "Shape.h"

// Sample prefab for static box
void CSampleStaticBoxPrefab::Prefab() {
    AddComponent<CBoxCollider3D>();
    AddComponent<CColorOnlyShape>(GraphicsLayer::ReadWriteShading, ShapeKind::Box, Colorf(0.2f, 0.2f, 0.2f, 1.0f));
    auto rb = AddComponent<CRigidBody>();
    rb->SetBodyType(RigidBodyType::Static);
    rb->SetMaterial(RigidBodyMaterial(0.5f, 0.5f));
}

// Sample prefab for bouncing ball
void CSampleSuperBallPrefab::Prefab() {
    AddComponent<CSphereCollider3D>();
    AddComponent<CColorOnlyShape>(GraphicsLayer::ReadWriteShading, ShapeKind::Sphere, Colorf(1.0f, 0.0f, 0.0f, 1.0f))->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 1.0f));
    auto rb = AddComponent<CRigidBody>();
    rb->SetBodyType(RigidBodyType::Dynamic);
    rb->SetMaterial(RigidBodyMaterial(0.0f, 1.0f));
    rb->SetMass(0.1f);
    rb->SetGravityScale(15.0f);
}

// Sample prefab for clay ball
void CSampleClayBallPrefab::Prefab() {
    AddComponent<CSphereCollider3D>();
    AddComponent<CColorOnlyShape>(GraphicsLayer::ReadWriteShading, ShapeKind::Sphere, Colorf(0.58f, 0.42f, 0.1f, 1.0f));
    auto rb = AddComponent<CRigidBody>();
    rb->SetBodyType(RigidBodyType::Dynamic);
    rb->SetMaterial(RigidBodyMaterial(1.0f, 0.0f));
    rb->SetMass(3.0f);
    rb->SetGravityScale(0.5f);
}
