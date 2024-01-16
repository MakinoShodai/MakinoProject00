/**
 * @file   PhysicsSampleScene.h
 * @brief  This file hanldes sample scene class for physics engine.
 * 
 * @author Shodai Makino
 * @date   2024/1/11
 */

#ifdef _FOR_PHYSICS

#ifndef __PHYSICS_SAMPLE_SCENE_H__
#define __PHYSICS_SAMPLE_SCENE_H__

#include "Scene.h"
#include "StandardGPSOWrapper.h"

 /** @brief Sample scene class for physics engine */
class CPhysicsSampleScene : public ACScene {
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
    /** @brief GPSO wrapper for transparent layer */
    CTransparentGPSOWrapper m_transparentGpso;
    /** @brief Depth stencil view for 3D */
    CDepthStencil m_dsv3D;
};

#endif // !__PHYSICS_SAMPLE_SCENE_H__

#endif // _FOR_PHYSICS