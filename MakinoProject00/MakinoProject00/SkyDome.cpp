#include "SkyDome.h"
#include "Shape.h"
#include "AssetNameDefine.h"
#include "Scene.h"

// Sample prefab for sky dome
void CSkyDomePrefab::Prefab() {
    AddComponent<CTexShape>(GraphicsLayer::SkyDome, ShapeKind::Sphere, TexName::SKYDOME);
}
