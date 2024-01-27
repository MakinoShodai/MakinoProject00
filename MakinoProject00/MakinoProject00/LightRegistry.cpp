#include "LightRegistry.h"

//  Remove point light from the array
void CLightRegistry::RemovePointLight(CPointLightComponent* point) {
    auto it = std::find_if(m_pointLights.begin(), m_pointLights.end(),
        [point](const CWeakPtr<CPointLightComponent>& ptr) { return ptr == point; });

    if (it != m_pointLights.end()) {
        m_pointLights.erase(it);
    }
}

// Set directional light
bool CLightRegistry::SetDirectionalLight(CWeakPtr<CDirectionalLightComponent> dirLight) {
    if (m_dirLight != nullptr) {
        return false;
    }

    m_dirLight = dirLight;

    return true;
}
