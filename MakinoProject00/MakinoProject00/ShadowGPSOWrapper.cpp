#include "ShadowGPSOWrapper.h"
#include "ShaderRegistry.h"

// Prefab function
void CWriteShadowGPSOWrapper::Prefab(CScene* scene) {
    // Default setting (For BasicModel and TexShape, ColorShape, DebugColliderShape)
    LayeredGPSOSetting setting({ GraphicsComponentType::BasicModel, GraphicsComponentType::TexShape, GraphicsComponentType::ColorShape, GraphicsComponentType::DebugColliderShape });
    setting.defaultSetting.vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::LightVP3D);
    setting.defaultSetting.rasterizerState.cullMode = D3D12_CULL_MODE_BACK;
    setting.defaultSetting.depth.type = Gpso::DepthTestType::ReadWrite;
    // Override setting (For SkeletalModel)
    {
        auto overrideSetting = setting.AddOverrideSetting({ GraphicsComponentType::SkeletalModel });
        overrideSetting->vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::LightVP3DAnim);
    }

    // Create
    CLayeredGPSOWrapper::Create(scene, L"Write Depth GPSO", setting, { GraphicsLayer::ReadWriteShading });
}

// Prefab function
void CShadingGPSOWrapper::Prefab(CScene* scene) {
    // Default setting (For BasicModel and TexShape, ColorShape, DebugColliderShape)
    LayeredGPSOSetting setting({ GraphicsComponentType::BasicModel, GraphicsComponentType::TexShape });
    setting.defaultSetting.vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::LightPosWorldPos3D);
    setting.defaultSetting.ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::ShadingTex);
    setting.defaultSetting.rtvFormats.push_back(std::make_pair(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Gpso::BlendStateSetting::Alpha));
    setting.defaultSetting.rasterizerState.cullMode = D3D12_CULL_MODE_BACK;
    setting.defaultSetting.depth.type = Gpso::DepthTestType::ReadWrite;
    // Override setting (For SkeletalModel)
    {
        auto overrideSetting = setting.AddOverrideSetting({ GraphicsComponentType::SkeletalModel });
        overrideSetting->vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::LightPosWorldPos3DAnim);
    }
    // Override setting (For ColorShape)
    {
        auto overrideSetting = setting.AddOverrideSetting({ GraphicsComponentType::ColorShape });
        overrideSetting->ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::ShadingColor);
    }

    // Create
    CLayeredGPSOWrapper::Create(scene, L"Shading GPSO", setting, { GraphicsLayer::ReadWriteShading });
}
