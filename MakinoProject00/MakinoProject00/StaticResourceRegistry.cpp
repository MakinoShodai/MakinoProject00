#include "StaticResourceRegistry.h"
#include "StaticCbVPMat.h"
#include "StaticCbOrthographicMat.h"

// Initialize
void CStaticResourceRegistry::Initialize() {
    EmplaceCbAllocator<CStaticCbVP>();
    EmplaceCbAllocator<CStaticCbOrthographicMat>();
}

// Processing when a new scene is created
void CStaticResourceRegistry::SceneStart(ACScene* scene) {
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
