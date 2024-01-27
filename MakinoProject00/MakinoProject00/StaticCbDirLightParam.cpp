#include "StaticCbDirLightParam.h"
#include "Scene.h"

// Scene start processing
void CStaticCbDirLightParam::Start(CScene* scene) {
    m_lightRegistry = scene->GetLightRegistry();
}

// Allocate data
Utl::Dx::CPU_DESCRIPTOR_HANDLE CStaticCbDirLightParam::AllocateData() {
    CDirectionalLightComponent* dirLight = m_lightRegistry->GetDirectionalLight().Get();

    if (dirLight != nullptr) {
        CCameraComponent* camera = dirLight->GetScene()->GetCameraRegistry()->GetCameraPriority().Get();
        if (camera != nullptr) {
            DirLightParam data;
            data.lightColor = dirLight->GetLightColor();
            data.ambientColor = dirLight->GetAmbientColor();
            data.lightDir.SetFromDiffSize(dirLight->GetTransform().GetForward());
            data.cameraPos.SetFromDiffSize(camera->GetTransform().pos);
            data.entireShadowVPMatrix = m_lightVPCaculator->GetShadowMatrix();
            for (UINT i = 0; i < CASCADE_NUM; ++i) {
                data.cascadeEndZ[i] = m_lightVPCaculator->GetCascadeEndZ(i);
                data.cascadeOffsets[i] = m_lightVPCaculator->GetCascadeOffset(i);
                data.cascadeScales[i] = m_lightVPCaculator->GetCascadeScale(i);
            }
            data.intensity = dirLight->GetIntensity();
            data.pointLightsNum = m_lightRegistry->GetPointLightsNum();

            return DirectDataCopy(&data);
        }
    }

    throw Utl::Error::CStopDrawingSceneError(L"Warning! Directional light or camera not exist in the scene");
}
