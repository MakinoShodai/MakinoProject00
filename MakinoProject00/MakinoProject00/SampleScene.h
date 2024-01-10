/**
 * @file   SampleScene.h
 * @brief  This file handles scene class for sample.
 * 
 * @author Shodai Makino
 * @date   2024/1/11
 */

#ifndef __SAMPLE_SCENE_H__
#define __SAMPLE_SCENE_H__

#include "Scene.h"
#include "StandardGPSOWrapper.h"

/** @brief Scene class for sample */
class CSampleScene : public ACScene {
public:
    /**
       @brief Starting process
    */
    void Start() override;

    /**
       @brief Drawing process
    */
    void Draw() override;

private:
    /** @brief GPSO wrapper for standard layer */
    CStandardGPSOWrapper m_standardGpso;
    /** @brief GPSO for UI */
    CGraphicsPipelineState m_gpsoUI;
    /** @brief Depth stencil view for 3D */
    CDepthStencil m_dsv3D;
    /** @brief Depth stencil view for 2D */
    CDepthStencil m_dsv2D;
};

#endif // !__SAMPLE_SCENE_H__
