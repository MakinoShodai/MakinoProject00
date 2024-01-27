/**
 * @file   StandardRenderPass.h
 * @brief  This file handles scene base class for stage.
 * 
 * @author Shodai Makino
 * @date   2024/1/18
 */

#ifndef __STANDARD_RENDER_PASS_H__
#define __STANDARD_RENDER_PASS_H__

#include "RenderPassAsset.h"
#include "StandardGPSOWrapper.h"
#include "ShadowGPSOWrapper.h"
#include "StaticCbLightVPMat.h"
#include "StaticSrvShadowMap.h"

/** @brief Default render pass name */
const std::string STANDARD_RENDER_PASS_NAME = "CStandardRenderPass";

/** @brief Default rendering pass asset */
class CStandardRenderPass : public ACRenderPassAsset {
public:
    using ACRenderPassAsset::ACRenderPassAsset;

    /**
       @brief Start processing
       @param scene Scene that has this class
    */
    void Start() override;

    /**
       @brief Drawing process
    */
    void Draw() override;

private:
    /** @brief GPSO wrapper for standard layer */
    CStandardGPSOWrapper m_standardGpso;
    /** @brief GPSO wrapper for transparent layer */
    CTransparentGPSOWrapper m_transparentGpso;
    /** @brief GPSO wrapper for writing shadow */
    CWriteShadowGPSOWrapper m_writeShadowGpso;
    /** @brief GPSO wrapper for shading */
    CShadingGPSOWrapper m_shadingGpso;
    /** @brief GPSO for UI */
    CGraphicsPipelineState m_uiGpso;
    /** @brief Depth stencil view for 3D */
    CDepthStencil m_dsv3D;
    /** @brief Depth stencil view for 2D */
    CDepthStencil m_dsv2D;
    /** @brief Depth stencil views for shadow map */
    CDepthStencil m_shadowMapDsvs[CASCADE_NUM];

    /** @brief Static CB allocater class that handles light view projection matrix */
    CStaticCbLightVP* m_staticCbLightVP;
    /** @brief Static SRV allocator class for shadow maps */
    ACStaticSRVAllocator* m_staticShadowMapSrv[CASCADE_NUM];
};
REGISTER_RENDERPASS_CLASS(CStandardRenderPass);

#endif // !__STANDARD_RENDER_PASS_H__
