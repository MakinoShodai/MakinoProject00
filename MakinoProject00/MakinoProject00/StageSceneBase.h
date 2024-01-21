/**
 * @file   StageSceneBase.h
 * @brief  This file handles scene base class for stage.
 * 
 * @author Shodai Makino
 * @date   2024/1/18
 */

#ifndef __STAGE_SCENE_BASE_H__
#define __STAGE_SCENE_BASE_H__

#include "Scene.h"
#include "StandardGPSOWrapper.h"

/** @brief Scene base class for stage 1 */
class CStageSceneBase : public ACScene {
public:
    /**
       @brief Starting process
    */
    void Start() override;

    /**
       @brief Drawing process
    */
    void Draw() override;

protected:
    /**
       @brief Starting process for derived class
    */
    virtual void StartDerived() {}

private:
    /** @brief GPSO wrapper for standard layer */
    CStandardGPSOWrapper m_standardGpso;
    /** @brief GPSO wrapper for transparent layer */
    CTransparentGPSOWrapper m_transparentGpso;
    /** @brief GPSO for UI */
    CGraphicsPipelineState m_uiGpso;
    /** @brief Depth stencil view for 3D */
    CDepthStencil m_dsv3D;
    /** @brief Depth stencil view for 2D */
    CDepthStencil m_dsv2D;
};

#endif // !__STAGE_SCENE_BASE_H__
