#include "StaticResourceRegistry.h"
#include "StaticCbVPMat.h"
#include "StaticCbOrthographicMat.h"
#include "StaticSrvShadowMap.h"
#include "StaticCbLightVPMat.h"
#include "StaticCbDirLightParam.h"
#include "StaticSbPointLights.h"

// Initialize
void CStaticResourceRegistry::Initialize() {
    EmplaceStaticAllocator<CStaticCbVP>();
    EmplaceStaticAllocator<CStaticCbRemovePosVP>();
    EmplaceStaticAllocator<CStaticCbDirLightParam>();
    EmplaceStaticAllocator<CStaticCbLightVP>();
    EmplaceStaticAllocator<CStaticCbOrthographicMat>();
    EmplaceStaticAllocator<CStaticCbOrthographicViewProjMat>();
    EmplaceStaticAllocator<CStaticSrvShadowMap1>();
    EmplaceStaticAllocator<CStaticSrvShadowMap2>();
    EmplaceStaticAllocator<CStaticSrvShadowMap3>();
    EmplaceStaticAllocator<CStaticSbPointLights>();
}

// Processing when a new scene is created
void CStaticResourceRegistry::SceneStart(CScene* scene) {
    for (auto& it : m_staticResources) {
        it->Start(scene);
    }
}

// Refresh process that must be called at the end of every frame
void CStaticResourceRegistry::FrameRefresh() {
    for (auto& it : m_staticResources) {
        it->Refresh();
    }
}
