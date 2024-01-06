#include "GraphicsPipelineElements.h"

// GPSOSetting Constructor
Gpso::GPSOSetting::GPSOSetting() {
    // Default depth stencil setting
    depth = DepthTestSetting();
    stencil = StencilTestSetting();
    dsvFormat = DXGI_FORMAT_D32_FLOAT;

    // Specify triangle to topology type. Don't perform special setting of index
    primitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    ibStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

    // Set null to shaders
    vs = nullptr;
    ps = nullptr;

    // Don't perform special setting
    rasterizerState = RasterizerStateSetting();
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;
    sampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    isUsingMultisampleAlpha = FALSE;
}

// Bind the specified blend state
void Gpso::BindBlendState(D3D12_RENDER_TARGET_BLEND_DESC* desc, BlendStateSetting setting) {
    switch (setting) {
    // Alpha blend state
    case BlendStateSetting::Alpha:
        // Operation method settings
        desc->BlendEnable   = TRUE;     // Alpha blending
        desc->LogicOpEnable = FALSE;    // No logic operations
        // RGB blend settings
        desc->SrcBlend  = D3D12_BLEND_SRC_ALPHA;     // Multiply alpha of new color to the RGB values of new color
        desc->DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // Multiply RGB values of old color by value obtained from calculating '1 - alpha of new color'
        desc->BlendOp   = D3D12_BLEND_OP_ADD;        // New color + Old color
        // Alpha blend settings
        desc->SrcBlendAlpha  = D3D12_BLEND_ONE;      // Alpha value of new color * 1
        desc->DestBlendAlpha = D3D12_BLEND_ZERO;     // Not use alpha of old color
        desc->BlendOpAlpha   = D3D12_BLEND_OP_ADD;   // Alpha value of new color + Alpha of old color
        // Other settings
        desc->RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // Write all color channel
        break;
    }
}

// Bind the specified sampler state
void Gpso::BindSamplerState(D3D12_STATIC_SAMPLER_DESC* desc, const SamplerStateSetting& setting, D3D12_SHADER_VISIBILITY visibility, UINT slot, UINT space) {
    desc->AddressU    = setting.addressMode; // Use the set value
    desc->AddressV    = setting.addressMode; // Use the set value
    desc->AddressW    = setting.addressMode; // Use the set value
    desc->BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK; // Not relevant because 'BORDERCOLOR' is not specified in AddressUVW
    // Filtering method for texture sampling. Decide how to interpolation at sampling point
    desc->Filter           = setting.filterMode;     // Use the set value
    desc->MaxLOD           = setting.MaxLOD;         // Use the set value
    desc->MinLOD           = setting.MinLOD;         // Use the set value
    desc->MaxAnisotropy    = setting.MaxAnisotropy;  // Use the set value
    desc->ComparisonFunc   = setting.ComparisonFunc; // Use the set value
    desc->ShaderRegister   = slot;                   // Specified value
    desc->RegisterSpace    = space;                  // Specified value
    desc->ShaderVisibility = visibility;             // Specified value
}

// Bind the specified rasterizer state
void Gpso::BindRasterizerState(D3D12_RASTERIZER_DESC* desc, const RasterizerStateSetting& setting) {
    // Assigns set values
    desc->FillMode              = setting.fillMode;
    desc->CullMode              = setting.cullMode;
    desc->DepthBias             = setting.depthBiasParam.depthBias;
    desc->DepthBiasClamp        = setting.depthBiasParam.depthBiasClamp;
    desc->SlopeScaledDepthBias  = setting.depthBiasParam.slopeScaledDepthBias;
    desc->MultisampleEnable     = setting.msaaParam.multiSampleEnable;
    desc->AntialiasedLineEnable = setting.msaaParam.antialiasedLineEnable;
    desc->ForcedSampleCount     = setting.forcedSampleCount;
    desc->ConservativeRaster    = setting.conservativeRaster;

    // Set other parameter
    desc->FrontCounterClockwise = FALSE; // Clockwise is front surface
    desc->DepthClipEnable = TRUE; // Enable depth clip
}

// Bind the specified depth test setting to a D3D12_DEPTH_STENCIL_DESC
void Gpso::BindDepthSetting(D3D12_DEPTH_STENCIL_DESC* desc, const DepthTestSetting& setting) {
    switch (setting.type) {
    // Read only
    case DepthTestType::ReadOnly:
        desc->DepthEnable = TRUE;
        desc->DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        break;
    // Read write
    case DepthTestType::ReadWrite:
        desc->DepthEnable = TRUE;
        desc->DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        break;
    // None
    case DepthTestType::None:
    default:
        desc->DepthEnable = FALSE;
        desc->DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        break;
    }
    desc->DepthFunc = setting.func;
}

// Bind the specified stencil test setting to a D3D12_DEPTH_STENCIL_DESC
void Gpso::BindStencilSetting(D3D12_DEPTH_STENCIL_DESC* desc, const StencilTestSetting& setting) {
    desc->StencilEnable = setting.stencilEnable;
    desc->StencilReadMask = setting.stencilReadMask;
    desc->StencilWriteMask = setting.stencilWriteMask;
    BindStencilOp(&desc->FrontFace, setting.frontFace);
    BindStencilOp(&desc->BackFace, setting.backFace);
}

// Bind the specified stencil test operation setting to a D3D12_DEPTH_STENCILOP_DESC
void Gpso::BindStencilOp(D3D12_DEPTH_STENCILOP_DESC* opDesc, StencilTestOpType type) {
    switch (type) {
    // Default
    case Gpso::StencilTestOpType::Standard:
    default:
        opDesc->StencilFailOp = D3D12_STENCIL_OP_KEEP;
        opDesc->StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
        opDesc->StencilPassOp = D3D12_STENCIL_OP_KEEP;
        opDesc->StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        break;
    }
}
