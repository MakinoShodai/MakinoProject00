#include "PointLight.h"
#include "Scene.h"

// Constructor
CPointLightComponent::CPointLightComponent(CGameObject* owner, Colorf color)
    : ACComponent(owner)
    , m_color(color)
    , m_constantAttenuation(0.0f)
    , m_linearAttenuation(0.05f)
    , m_quadraticAttenuation(0.01f)
    , m_intensity(10.0f)
    , m_isStart(false) {
}

// Starting process
void CPointLightComponent::Start() {
    m_isStart = true;
    
    GetScene()->GetLightRegistry()->AddPointLight(WeakFromThis());
}

// Process at enable
void CPointLightComponent::OnEnable() {
    if (m_isStart) {
        GetScene()->GetLightRegistry()->AddPointLight(WeakFromThis());
    }
}

// Process at disable
void CPointLightComponent::OnDisable() {
    GetScene()->GetLightRegistry()->RemovePointLight(this);
}
