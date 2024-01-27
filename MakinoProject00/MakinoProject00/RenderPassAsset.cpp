#include "RenderPassAsset.h"

// Create render pass asset
ACRenderPassAsset* ACRegistrarForRenderPassAsset::CreateRenderPassAsset(const std::string& className) {
    ClassMap& map = GetClassMapProtected();

    auto it = map.find(className);
    if (it != map.end()) {
        return it->second();
    }

    return nullptr;
}

// Call 'Start' function
void ACRenderPassAsset::CallStart(CWeakPtr<CScene> scene) {
    m_scene = scene;

    Start();
}
