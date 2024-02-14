#include "Ruins.h"
#include "AssetNameDefine.h"
#include "Model.h"
#include "CapsuleCollider3D.h"
#include "BoxCollider3D.h"

// Prefab for big brick 01
void CBigBrick01Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::BRICK_BIG01)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CBoxCollider3D>(Vector3f(0.6f, 1.15f, 0.6f));
}

// Prefab for big brick 02
void CBigBrick02Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::BRICK_BIG02)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CBoxCollider3D>(Vector3f(0.9f, 1.5f, 0.9f));
}

// Prefab for big brick 04
void CBigBrick04Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::BRICK_BIG04)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CBoxCollider3D>(Vector3f(0.6f, 1.4f, 0.6f));
}

// Prefab for ruin column 03
void CColumn03Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::COLUMN03)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CCapsuleCollider3D>(8.0f, 0.35f, Vector3f(0.0f, -0.65f, 0.0f));
}

// Prefab for ruin wall element 02
void CWallElement02Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::WALL_ELEM_02)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CBoxCollider3D>(Vector3f(0.5f, 1.0f, 2.3f), Vector3f(0.0f, 0.5f, 0.0f));
}
