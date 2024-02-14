#include "Trees.h"
#include "Model.h"
#include "AssetNameDefine.h"
#include "CapsuleCollider3D.h"

// Prefab for tree 01
void CTree01Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::TREE01)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CCapsuleCollider3D>(5.0f, 0.5f, Vector3f(-0.2f, 2.0f, 0.0f));
}

// Prefab for tree 02
void CTree02Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::TREE02)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CCapsuleCollider3D>(5.0f, 0.85f, Vector3f(0.0f, 2.0f, 0.0f));
}

// Prefab for tree 03
void CTree03Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::TREE03)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CCapsuleCollider3D>(5.0f, 0.3f, Vector3f(-0.1f, 2.0f, 0.1f));
}

// Prefab for tree 04
void CTree04Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::TREE04)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CCapsuleCollider3D>(5.0f, 0.5f, Vector3f(-1.4f, 2.0f, -0.3f));
}

// Prefab for tree 05
void CTree05Prefab::Prefab() {
    AddComponent<CBasicModel>(GraphicsLayer::ReadWriteShading, ModelName::TREE05)->SetNumericMaterialToAllMesh(PerMeshMaterialNumeric(1.0f, 0.0f));
    AddComponent<CCapsuleCollider3D>(5.0f, 0.8f, Vector3f(0.0f, 2.0f, 0.0f));
}
