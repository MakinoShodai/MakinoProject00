/**
 * @file   GraphicsPipelineElements.h
 * @brief  This file handles elements of graphics pipeline state.
 * 
 * @author Shodai Makino
 * @date   2023/11/9
 */
#ifndef __GRAPHICS_PIPELINE_ELEMENTS_H__
#define __GRAPHICS_PIPELINE_ELEMENTS_H__

// Forward declaration
class CVertexShader;
class CPixelShader;

namespace Gpso {
    /** @brief Blend state settings */
    enum class BlendStateSetting {
        /** @brief Alpha blend state */
        Alpha,
    };

    /**
       @brief Sampler state setting
       @sa https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_static_sampler_desc
    */
    struct SamplerStateSetting {
        /** @brief How to handle texture coordinates outside of texture boundaries */
        D3D12_TEXTURE_ADDRESS_MODE addressMode;
        /** @brief Texture filtering option */
        D3D12_FILTER filterMode;
        /** @brief If mipmap is not used, then this is not relevant */
        FLOAT MinLOD;
        /** @brief If mipmap is not used, then this is not relevant */
        FLOAT MaxLOD;
        /** @brief If the filtermode is set to anisotropic, then use this clamp value */
        UINT MaxAnisotropy;
        /** @brief If depth texture, etc. are not used, then this is not relevant */
        D3D12_COMPARISON_FUNC ComparisonFunc;

        /**
           @brief Constructor
        */
        SamplerStateSetting() {
            addressMode    = D3D12_TEXTURE_ADDRESS_MODE_WRAP;            // Wrap
            filterMode     = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;  // Point sampling
            MinLOD         = 0;                                          // Minimum value
            MaxLOD         = D3D12_FLOAT32_MAX;                          // Maximum value
            MaxAnisotropy  = 1;                                          // Minimum valid value
            ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;                // Not use it
        }
    };

    /**
       @brief Rasterizer state setting
       @sa https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_rasterizer_desc
    */
    struct RasterizerStateSetting {
        /** @brief Filling mode */
        D3D12_FILL_MODE fillMode;
        /** @brief Culling mode */
        D3D12_CULL_MODE cullMode;
        /** @brief Sample values to enforce during UAV rendering or rasterization */
        BOOL forcedSampleCount;
        /** @brief Reduce artifacts in geometory with fuzzy edges and corners */
        D3D12_CONSERVATIVE_RASTERIZATION_MODE conservativeRaster;
        
        /** @brief Depth bias setting parameters */
        struct DepthBiasParam {
            /** @brief Depth bias. Can be used for shadow acne reduction, etc. */
            INT depthBias;
            /** @brief Upper limit of depth bias */
            FLOAT depthBiasClamp;
            /** @brief Scaling depth bias based on pixel slope */
            FLOAT slopeScaledDepthBias;
        } depthBiasParam;

        /** @brief Value to be set when MSAA render target */
        struct MSAAParam {
            /** @brief Perform anti-aliasing of rectangles and alpha line? */
            BOOL multiSampleEnable;
            /** @brief Perform anti-aliasing of line? */
            BOOL antialiasedLineEnable;
        } msaaParam;

        /**
           @brief Constructor
        */
        RasterizerStateSetting() {
            fillMode                            = D3D12_FILL_MODE_SOLID;                     // Solid fill mode
            cullMode                            = D3D12_CULL_MODE_BACK;                      // Back surface culling
            forcedSampleCount                   = 0;                                         // Not use it
            conservativeRaster                  = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF; // Off
            depthBiasParam.depthBias            = 0;                                         // Not use it
            depthBiasParam.depthBiasClamp       = 0.0f;                                      // Not use it
            depthBiasParam.slopeScaledDepthBias = 0.0f;                                      // Not use it
            msaaParam.multiSampleEnable         = TRUE;                                      // General setting
            msaaParam.multiSampleEnable         = FALSE;                                     // General setting
        }
    };

    /** @brief Type of depth test */
    enum class DepthTestType {
        /** @brief No depth test */
        None,
        /** @brief Reading only */
        ReadOnly,
        /** @brief Reading and writing */
        ReadWrite,
    };

    /** @brief Depth test setting */
    struct DepthTestSetting {
        /** @brief Type of depth test */
        DepthTestType type;
        /** @brief Comparison function */
        D3D12_COMPARISON_FUNC func;

        /** @brief Constructor */
        DepthTestSetting() {
            type = DepthTestType::None;
            func = D3D12_COMPARISON_FUNC_LESS;
        }
    };

    /** @brief Type of stencil test operator */
    enum class StencilTestOpType {
        /** @brief Standard */
        Standard,
    };

    /** @brief Stencil test setting */
    struct StencilTestSetting {
        /** @brief Is stencil test valid? */
        BOOL stencilEnable;
        /** @brief Reading mask of stencil test */
        UINT8 stencilReadMask;
        /** @brief Writing mask of stencil test */
        UINT8 stencilWriteMask;
        /** @brief Type of stencil test for front face */
        StencilTestOpType frontFace;
        /** @brief Type of stencil test for back face */
        StencilTestOpType backFace;

        /** @brief Constructor */
        StencilTestSetting() {
            stencilEnable = FALSE;
            stencilReadMask = 0;
            stencilWriteMask = 0;
            frontFace = StencilTestOpType::Standard;
            backFace = StencilTestOpType::Standard;
        };
    };

    /**
       @brief Necessary settings to create a graphics pipeline state object
       @details
       Please set at least 'rtvFormats' by yourself
    */
    struct GPSOSetting {
        /** @brief Rasterizer state setting */
        RasterizerStateSetting rasterizerState;
        /** @brief Depth test setting */
        DepthTestSetting depth;
        /** @brief Stencil test setting */
        StencilTestSetting stencil;
        /** @brief Format of Depth stencil buffer */
        DXGI_FORMAT dsvFormat;
        /** @brief Formats of Rendertarget */
        std::vector<std::pair<DXGI_FORMAT, BlendStateSetting>> rtvFormats;
        /** @brief Vertex configuration setting */
        D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType;
        /** @brief If you are using 'D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE' with 'primitiveTopologyType', specify the properties of the index buffer */
        D3D12_INDEX_BUFFER_STRIP_CUT_VALUE ibStripCutValue;
        /** @brief Sampling setting */
        DXGI_SAMPLE_DESC sampleDesc;
        /** @brief Sample mask */
        UINT sampleMask;
        /** @brief Use alpha with multisampling? */
        BOOL isUsingMultisampleAlpha;
        /** @brief Vertex shader used */
        CVertexShader* vs;
        /** @brief Pixel shader used */
        CPixelShader* ps;

        /**
           @brief Constructor
        */
        GPSOSetting();
    };

    /**
       @brief Structure to override necessary settings to create a graphics pipeline state object
       @details
       Set values only for variables to override
    */
    struct GPSOSettingOverride {
        /** @brief Rasterizer state setting */
        std::optional<RasterizerStateSetting> rasterizerState;
        /** @brief Depth test setting */
        std::optional<DepthTestSetting> depth;
        /** @brief Stencil test setting */
        std::optional<StencilTestSetting> stencil;
        /** @brief Format of Depth stencil buffer */
        std::optional<DXGI_FORMAT> dsvFormat;
        /** @brief Formats of Rendertarget */
        std::optional<std::vector<std::pair<DXGI_FORMAT, BlendStateSetting>>> rtvFormats;
        /** @brief Vertex configuration setting */
        std::optional<D3D12_PRIMITIVE_TOPOLOGY_TYPE> primitiveTopologyType;
        /** @brief If you are using 'D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE' with 'primitiveTopologyType', specify the properties of the index buffer */
        std::optional<D3D12_INDEX_BUFFER_STRIP_CUT_VALUE> ibStripCutValue;
        /** @brief Sampling setting */
        std::optional<DXGI_SAMPLE_DESC> sampleDesc;
        /** @brief Sample mask */
        std::optional<UINT> sampleMask;
        /** @brief Use alpha with multisampling? */
        std::optional<BOOL> isUsingMultisampleAlpha;
        /** @brief Vertex shader used */
        std::optional<CVertexShader*> vs;
        /** @brief Pixel shader used */
        std::optional<CPixelShader*> ps;

        /**
           @brief Get overridden setting
           @param baseSetting Base setting
           @return Overridden setting
        */
        GPSOSetting GetOverriddenSetting(const GPSOSetting& baseSetting) const {
            GPSOSetting ret;
            ret.rasterizerState         = (this->rasterizerState)         ? this->rasterizerState.value()         : baseSetting.rasterizerState;
            ret.depth                   = (this->depth)                   ? this->depth.value()                   : baseSetting.depth;
            ret.stencil                 = (this->stencil)                 ? this->stencil.value()                 : baseSetting.stencil;
            ret.dsvFormat               = (this->dsvFormat)               ? this->dsvFormat.value()               : baseSetting.dsvFormat;
            ret.rtvFormats              = (this->rtvFormats)              ? this->rtvFormats.value()              : baseSetting.rtvFormats;
            ret.primitiveTopologyType   = (this->primitiveTopologyType)   ? this->primitiveTopologyType.value()   : baseSetting.primitiveTopologyType;
            ret.ibStripCutValue         = (this->ibStripCutValue)         ? this->ibStripCutValue.value()         : baseSetting.ibStripCutValue;
            ret.sampleDesc              = (this->sampleDesc)              ? this->sampleDesc.value()              : baseSetting.sampleDesc;
            ret.sampleMask              = (this->sampleMask)              ? this->sampleMask.value()              : baseSetting.sampleMask;
            ret.isUsingMultisampleAlpha = (this->isUsingMultisampleAlpha) ? this->isUsingMultisampleAlpha.value() : baseSetting.isUsingMultisampleAlpha;
            ret.vs                      = (this->vs)                      ? this->vs.value()                      : baseSetting.vs;
            ret.ps                      = (this->ps)                      ? this->ps.value()                      : baseSetting.ps;

            return ret;
        }
    };

    /**
       @brief Bind the specified blend state
       @param desc Variable to which you want to bind blend state
       @param setting Blend state setting to be used
    */
    void BindBlendState(D3D12_RENDER_TARGET_BLEND_DESC* desc, BlendStateSetting setting);

    /**
       @brief Bind the specified sampler state
       @param desc Variable to which you want to bind sampler state
       @param setting Sampler state setting to be used
       @param visibility Shader visibility
       @param slot Slot index
       @param space Space index
    */
    void BindSamplerState(D3D12_STATIC_SAMPLER_DESC* desc, const SamplerStateSetting& setting, D3D12_SHADER_VISIBILITY visibility, UINT slot, UINT space);

    /**
       @brief Bind the specified rasterizer state
       @param desc Variable to which you want to bind rasterizer state
       @param setting Rasterizer state setting to be used
    */
    void BindRasterizerState(D3D12_RASTERIZER_DESC* desc, const RasterizerStateSetting& setting);

    /**
       @brief Bind the specified depth test setting to a D3D12_DEPTH_STENCIL_DESC
       @param desc Variable to which you want to bind a depth stencil desc
       @param setting Depth test setting to be used
    */
    void BindDepthSetting(D3D12_DEPTH_STENCIL_DESC* desc, const DepthTestSetting& setting);

    /**
       @brief Bind the specified stencil test setting to a D3D12_DEPTH_STENCIL_DESC
       @param desc Variable to which you want to bind a depth stencil desc
       @param setting Stencil test setting to be used
    */
    void BindStencilSetting(D3D12_DEPTH_STENCIL_DESC* desc, const StencilTestSetting& setting);

    /**
       @brief Bind the specified stencil test operation setting to a D3D12_DEPTH_STENCILOP_DESC
       @param desc Variable to which you want to bind a depth stencil operation desc
       @param setting Type of tencil test operation to be used
    */
    void BindStencilOp(D3D12_DEPTH_STENCILOP_DESC* opDesc, StencilTestOpType type);

} // namespace Gpso

#endif // !__GRAPHICS_PIPELINE_ELEMENTS_H__
