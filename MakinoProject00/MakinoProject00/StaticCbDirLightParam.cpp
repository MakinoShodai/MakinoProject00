#include "StaticCbDirLightParam.h"
#include "Scene.h"

// Scene start processing
void CStaticCbDirLightParam::Start(ACScene* scene) {
    CWeakPtr<CGameObject> obj = scene->FindGameObject(OBJNAME_DIRECTIONAL_LIGHT);
    m_directionalLight = (obj != nullptr) ? obj->GetComponent<CDirectionalLightComponent>() : nullptr;
    m_lightRegistry = scene->GetLightRegistry();
}

// Allocate data
Utl::Dx::CPU_DESCRIPTOR_HANDLE CStaticCbDirLightParam::AllocateData() {
    CCameraComponent* camera = m_directionalLight->GetScene()->GetCameraRegistry()->GetCameraPriority().Get();
    
    DirLightParam data;
    data.lightColor = m_directionalLight->GetLightColor();
    data.ambientColor = m_directionalLight->GetAmbientColor();
    data.lightDir.SetFromDiffSize(m_directionalLight->GetLightDir());
    data.cameraPos.SetFromDiffSize(camera->GetTransform().pos);
    data.entireShadowVPMatrix = m_lightVPCaculator->GetShadowMatrix();
    for (UINT i = 0; i < CASCADE_NUM; ++i) {
        data.cascadeEndZ[i] = m_lightVPCaculator->GetCascadeEndZ(i);
        data.cascadeOffsets[i] = m_lightVPCaculator->GetCascadeOffset(i);
        data.cascadeScales[i] = m_lightVPCaculator->GetCascadeScale(i);
    }
    data.intensity = m_directionalLight->GetIntensity();
    data.pointLightsNum = m_lightRegistry->GetPointLightsNum();

    return DirectDataCopy(&data);
}
