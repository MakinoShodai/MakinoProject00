#include "Terrain.h"
#include "BoxCollider3D.h"
#include "Shape.h"
#include "AssetNameDefine.h"

// Prefab for grass terrain
void CGrassTerrainPrefab::Prefab() {
    AddComponent<CTexShape>(GraphicsLayer::ReadWriteShading, ShapeKind::Box, TexName::GRASS)->SetUVTiling(Vector2f::Ones() * 0.01f);
    AddComponent<CBoxCollider3D>();
}
