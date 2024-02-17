#include "ShaderRegistry.h"

// Initialize
void CShaderRegistry::Initialize() {
    // Standard sampler
    Gpso::SamplerStateSetting standardSampSetting;
    SamplerTransmitter standardSamp(standardSampSetting, "mainSmp");
    // clamp sampler
    Gpso::SamplerStateSetting clampSampSetting;
    clampSampSetting.addressMode = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    SamplerTransmitter shadowMapSamp(clampSampSetting, "shadowMapSmp");
    
    // VS standard
    m_vertexShaders[static_cast<UINT>(VertexShaderType::Standard2DSprite)].Load("Assets/Shader/VS_Standard2DSprite.cso");
    m_vertexShaders[static_cast<UINT>(VertexShaderType::Standard3DSprite)].Load("Assets/Shader/VS_Standard3DSprite.cso");
    m_vertexShaders[static_cast<UINT>(VertexShaderType::Standard3D)].Load("Assets/Shader/VS_Standard3D.cso");
    m_vertexShaders[static_cast<UINT>(VertexShaderType::Standard3DAnim)].Load("Assets/Shader/VS_Standard3DAnim.cso");
    // VS standard remove a position from view
    m_vertexShaders[static_cast<UINT>(VertexShaderType::StandardRemovePos3D)].Load("Assets/Shader/VS_StandardRemovePos3D.cso");
    // VS light position and world position 
    m_vertexShaders[static_cast<UINT>(VertexShaderType::LightPosWorldPos3D)].Load("Assets/Shader/VS_LightPosWorldPos3D.cso");
    m_vertexShaders[static_cast<UINT>(VertexShaderType::LightPosWorldPos3DAnim)].Load("Assets/Shader/VS_LightPosWorldPos3DAnim.cso");
    // VS light view projection matrix
    m_vertexShaders[static_cast<UINT>(VertexShaderType::LightVP3D)].Load("Assets/Shader/VS_LightVP3D.cso");
    m_vertexShaders[static_cast<UINT>(VertexShaderType::LightVP3DAnim)].Load("Assets/Shader/VS_LightVP3DAnim.cso");
    m_vertexShaders[static_cast<UINT>(VertexShaderType::LightVPOutUV3D)].Load("Assets/Shader/VS_LightVPOutUV3D.cso");
    
    // PS standard tex
    m_pixelShaders[static_cast<UINT>(PixelShaderType::StandardTex)].Load("Assets/Shader/PS_StandardTex.cso");
    m_pixelShaders[static_cast<UINT>(PixelShaderType::StandardTex)].SetSampler({ standardSamp });
    // PS standard color
    m_pixelShaders[static_cast<UINT>(PixelShaderType::StandardColor)].Load("Assets/Shader/PS_StandardColorOnly.cso");
    // PS writing depth
    m_pixelShaders[static_cast<UINT>(PixelShaderType::DepthWrite)].Load("Assets/Shader/PS_DepthWrite.cso");
    // PS Shading for using texture
    m_pixelShaders[static_cast<UINT>(PixelShaderType::ShadingTex)].Load("Assets/Shader/PS_ShadingTex.cso");
    m_pixelShaders[static_cast<UINT>(PixelShaderType::ShadingTex)].SetSampler({ standardSamp, shadowMapSamp });
    // PS Shading for color only
    m_pixelShaders[static_cast<UINT>(PixelShaderType::ShadingColor)].Load("Assets/Shader/PS_ShadingColorOnly.cso");
    m_pixelShaders[static_cast<UINT>(PixelShaderType::ShadingColor)].SetSampler({ shadowMapSamp });
}
