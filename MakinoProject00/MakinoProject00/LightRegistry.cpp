#include "LightRegistry.h"

//  Remove point light from the array
void CLightRegistry::RemovePointLight(CPointLightComponent* point) {
    auto it = std::find_if(m_pointLights.begin(), m_pointLights.end(),
        [point](const CWeakPtr<CPointLightComponent>& ptr) { return point == ptr.Get(); });

    if (it != m_pointLights.end()) {
        m_pointLights.erase(it);
    }
}
