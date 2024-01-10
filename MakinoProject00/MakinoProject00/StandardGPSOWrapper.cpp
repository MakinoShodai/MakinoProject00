#include "StandardGPSOWrapper.h"
#include "ShaderRegistry.h"

// Prefab function
void CStandardGPSOWrapper::Prefab(ACScene* scene) {
    // Default setting (For BasicModel and TexShape)
    LayeredGPSOSetting setting({ GraphicsComponentType::BasicModel, GraphicsComponentType::TexShape });
    setting.defaultSetting.vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard3D);
    setting.defaultSetting.ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::StandardTex);
    setting.defaultSetting.rtvFormats.push_back(std::make_pair(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Gpso::BlendStateSetting::Alpha));
    setting.defaultSetting.rasterizerState.cullMode = D3D12_CULL_MODE_BACK;
    setting.defaultSetting.depth.type = Gpso::DepthTestType::ReadWrite;
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
    // Override setting (For Sprite3D and Billboard)
    {
        auto overrideSetting = setting.AddOverrideSetting({ GraphicsComponentType::Sprite3D, GraphicsComponentType::Billboard });
        overrideSetting->vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard3DSprite);
        overrideSetting->rasterizerState = Gpso::RasterizerStateSetting();
        overrideSetting->rasterizerState->cullMode = D3D12_CULL_MODE_NONE;
    }

    // Create
    CLayeredGPSOWrapper::Create(scene, L"Standard GPSO", setting, { GraphicsLayer::Standard });
    
}
