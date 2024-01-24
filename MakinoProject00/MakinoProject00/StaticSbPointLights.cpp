#include "StaticSbPointLights.h"
#include "Scene.h"

// Scene start processing
void CStaticSbPointLights::Start(ACScene* scene) {
    m_lightRegistry = scene->GetLightRegistry();
}

// Allocate data
Utl::Dx::CPU_DESCRIPTOR_HANDLE CStaticSbPointLights::AllocateData() {
    UINT pointLightsNum = m_lightRegistry->GetPointLightsNum();
    if (pointLightsNum == 0) {
        return DirectDataCopy(0, nullptr);
    }

    const std::vector<CWeakPtr<CPointLightComponent>>& pointLights = m_lightRegistry->GetPointLights();
    CUniquePtr<PointLightInHlsl[]> data = CUniquePtr<PointLightInHlsl[]>::Make(pointLightsNum);

    for (UINT i = 0; i < pointLightsNum; ++i) {
        data[i].color = pointLights[i]->GetColor();
        data[i].posWS.SetFromDiffSize(pointLights[i]->GetTransform().pos, 1.0f);
        data[i].attenuation = pointLights[i]->GetAllAttenuations();
        data[i].intensity = pointLights[i]->GetIntensity();
    }

    return DirectDataCopy(pointLightsNum, data.Get());
}
