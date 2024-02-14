#include "StonePrefab.h"
#include "Model.h"
#include "AssetNameDefine.h"
#include "BoxCollider3D.h"
#include "SphereCollider3D.h"

// Prefab for middle stone 01
void CStoneMiddle01Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::STONE_MIDDLE01)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CBoxCollider3D>(Vector3f(2.3f, 3.7f, 2.575f), Vector3f(0.15f, 0.0f, -0.05f));
}

// Prefab for middle stone 02
void CStoneMiddle02Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::STONE_MIDDLE02)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CBoxCollider3D>(Vector3f(2.5f, 3.7f, 3.1f), Vector3f(0.0f, 0.0f, -0.05f));
}

// Prefab for middle stone 03
void CStoneMiddle03Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::STONE_MIDDLE03)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CBoxCollider3D>(Vector3f(2.0f, 3.7f, 2.65f), Vector3f(0.0f, 0.0f, -0.1f));
}

// Prefab for small stone 01
void CStoneSmall01Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::STONE_SMALL01)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CBoxCollider3D>(Vector3f(1.8f, 1.525f, 2.0f), Vector3f(0.0f, -0.10f, 0.0f));
}

// Prefab for small stone 02
void CStoneSmall02Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::STONE_SMALL02)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CBoxCollider3D>(Vector3f(2.75f, 1.75f, 1.85f), Vector3f(0.0f, 0.0f, -0.025f));
}

// Prefab for small stone 03
void CStoneSmall03Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::STONE_SMALL03)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CBoxCollider3D>(Vector3f(2.2f, 3.0f, 2.1f), Vector3f(-0.05f, 0.0f, -0.3f));
}

// Prefab for small stone 04
void CStoneSmall04Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::STONE_SMALL04)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CSphereCollider3D>(1.3f, Vector3f(0.0f, 0.0f, 0.0f));
}

// Prefab for small stone 05
void CStoneSmall05Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::STONE_SMALL05)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CBoxCollider3D>(Vector3f(1.35f, 2.6f, 1.6f), Vector3f(0.0f, 0.0f, -0.05f));
}

// Prefab for small stone 06
void CStoneSmall06Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::STONE_SMALL06)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CBoxCollider3D>(Vector3f(1.2f, 2.5f, 1.55f), Vector3f(-0.1f, 0.0f, -0.02f));
}
