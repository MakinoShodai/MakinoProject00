#include "ShaderRegistry.h"

// Initialize
void CShaderRegistry::Initialize() {
    // Standard sampler
    Gpso::SamplerStateSetting standardSampSetting;
    SamplerTransmitter standardSamp(standardSampSetting, "mainSmp");
    
    // VS standard sprite
    m_vertexShaders[static_cast<UINT>(VertexShaderType::Standard2DSprite)].Load("Assets/Shader/VS_Standard2DSprite.cso");
    m_vertexShaders[static_cast<UINT>(VertexShaderType::Standard3DSprite)].Load("Assets/Shader/VS_Standard3DSprite.cso");
    m_vertexShaders[static_cast<UINT>(VertexShaderType::Standard3D)].Load("Assets/Shader/VS_Standard3D.cso");
    m_vertexShaders[static_cast<UINT>(VertexShaderType::Standard3DAnim)].Load("Assets/Shader/VS_Standard3DAnim.cso");
    
    // PS standard tex
    m_pixelShaders[static_cast<UINT>(PixelShaderType::StandardTex)].Load("Assets/Shader/PS_StandardTex.cso");
    m_pixelShaders[static_cast<UINT>(PixelShaderType::StandardTex)].SetSampler({ standardSamp });
    // PS standard color
    m_pixelShaders[static_cast<UINT>(PixelShaderType::StandardColor)].Load("Assets/Shader/PS_StandardColorOnly.cso");
}
