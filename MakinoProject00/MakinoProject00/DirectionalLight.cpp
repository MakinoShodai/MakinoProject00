#include "DirectionalLight.h"

// Initial light direction
const Vector3f INIT_LIGHT_DIR = Vector3f(0.2f, -1.0f, 0.2f);
// Initial light color
const Colorf INIT_LIGHT_COLOR = Colorf(0.1f, 0.1f, 0.2f, 1.0f);
// Initial ambient color
const Colorf INIT_AMBIENT_COLOR = Colorf(0.01f, 0.01f, 0.02f, 1.0f);
// Initial intensity
const float INIT_INTENSITY = 1.0f;

// Starting process
void CDirectionalLightComponent::Start() {
    SetLightDir(INIT_LIGHT_DIR);
    SetLightColor(INIT_LIGHT_COLOR);
    SetAmbientColor(INIT_AMBIENT_COLOR);
    SetIntensity(INIT_INTENSITY);
}

// Set direction vector of this light
void CDirectionalLightComponent::SetLightDir(const Vector3f& dir) {
    m_lightDir = dir;
    m_lightDir.Normalize();
    m_gameObj->SetRotation(Utl::Math::VectorToVectorQuaternion(Utl::Math::UNIT3_FORWARD, m_lightDir));
}

// Prefab function
void CDirectionalLightPrefab::Prefab() {
    AddComponent<CDirectionalLightComponent>();

    SetName(OBJNAME_DIRECTIONAL_LIGHT);
}
