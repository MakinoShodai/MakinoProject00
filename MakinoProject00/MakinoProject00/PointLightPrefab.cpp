#include "PointLightPrefab.h"
#include "PointLight.h"

// Prefab for regular point light
void CPointLightRegularPrefab::Prefab() {
    AddComponent<CPointLightComponent>();
}

// Prefab for strong point light
void CPointLightStrongPrefab::Prefab() {
    auto light = AddComponent<CPointLightComponent>();
    light->SetLinearAttenuation(0.025f);
    light->SetQuadraticAttenuation(0.005f);
}

// Prefab for weak point light
void CPointLightWeakPrefab::Prefab() {
    auto light = AddComponent<CPointLightComponent>();
    light->SetLinearAttenuation(0.1f);
    light->SetQuadraticAttenuation(0.02f);
}
