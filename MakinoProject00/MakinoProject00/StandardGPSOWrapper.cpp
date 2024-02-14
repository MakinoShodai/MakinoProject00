#include "StandardGPSOWrapper.h"
#include "ShaderRegistry.h"

// Prefab helper function
void ACStandardGPSOWrapper::PrefabHelper(std::wstring name, CScene* scene, std::initializer_list<GraphicsLayer> useLayers, bool isDepthWrite, D3D12_CULL_MODE cullMode) {
    // Default setting (For BasicModel and TexShape)
    LayeredGPSOSetting setting({ GraphicsComponentType::BasicModel, GraphicsComponentType::TexShape });
    setting.defaultSetting.vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard3D);
    setting.defaultSetting.ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::StandardTex);
    setting.defaultSetting.rtvFormats.push_back(std::make_pair(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Gpso::BlendStateSetting::Alpha));
    setting.defaultSetting.rasterizerState.cullMode = cullMode;
    setting.defaultSetting.depth.type = (isDepthWrite) ? Gpso::DepthTestType::ReadWrite : Gpso::DepthTestType::ReadOnly;
    // Override setting (For SkeletalModel)
    {
        auto overrideSetting = setting.AddOverrideSetting({ GraphicsComponentType::SkeletalModel });
        overrideSetting->vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard3DAnim);
    }
    // Override setting (For ColorShape)
    {
        auto overrideSetting = setting.AddOverrideSetting({ GraphicsComponentType::ColorShape });
        overrideSetting->ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::StandardColor);
    }
    // Override setting (For DebugColliderShape)
    {
        auto overrideSetting = setting.AddOverrideSetting({ GraphicsComponentType::DebugColliderShape });
        overrideSetting->ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::StandardColor);
        overrideSetting->rasterizerState = Gpso::RasterizerStateSetting();
        overrideSetting->rasterizerState->cullMode = cullMode;
        overrideSetting->rasterizerState->depthBiasParam.depthBias = -5;
        overrideSetting->rasterizerState->depthBiasParam.slopeScaledDepthBias = -0.01f;
    }
    // Override setting (For Sprite3D and Billboard)
    {
        auto overrideSetting = setting.AddOverrideSetting({ GraphicsComponentType::Sprite3D, GraphicsComponentType::Billboard });
        overrideSetting->vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard3DSprite);
        overrideSetting->rasterizerState = Gpso::RasterizerStateSetting();
        overrideSetting->rasterizerState->cullMode = D3D12_CULL_MODE_NONE;
    }

    // Create
    CLayeredGPSOWrapper::Create(scene, std::move(name), setting, useLayers);
}

// Prefab function
void CStandardGPSOWrapper::Prefab(CScene* scene) {
    PrefabHelper(L"Standard GPSO", scene, { GraphicsLayer::Standard }, true, D3D12_CULL_MODE_BACK);
}

// Prefab function
void CTransparentGPSOWrapper::Prefab(CScene* scene) {
    PrefabHelper(L"Transparent GPSO", scene, { GraphicsLayer::Transparent }, false, D3D12_CULL_MODE_BACK);
}

// Prefab function
void CTransparentNoCullingGPSOWrapper::Prefab(CScene* scene) {
    PrefabHelper(L"Transparent and no culling GPSO", scene, { GraphicsLayer::TransparentNoCulling }, false, D3D12_CULL_MODE_NONE);
}
