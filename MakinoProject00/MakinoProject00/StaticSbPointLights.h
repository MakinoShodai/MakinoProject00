/**
 * @file   StaticSbPointLights.h
 * @brief  This file handles structured buffer allocator for point lights.
 * 
 * @author Shodai Makino
 * @date   2024/1/24
 */

#ifndef __STATIC_SB_POINT_LIGHTS_H__
#define __STATIC_SB_POINT_LIGHTS_H__

#include "StaticSbAllocator.h"
#include "LightRegistry.h"

/** @brief Point light parameter in hlsl */
struct PointLightInHlsl {
    /** @brief Light color */
    Colorf color;
    /** @brief Light position in world coordinate space */
    Vector4f posWS;
    /** @brief Attenuation */
    Vector4f attenuation;
    /** @brief Intensity */
    float intensity;
};

 /** @brief Static CB class that handles orthographic projection matrix */
class CStaticSbPointLights : public ACStaticSbAllocator<PointLightInHlsl> {
public:
    /** @brief Constructor */
    CStaticSbPointLights() : ACStaticSbAllocator("pointLights") {}

    /** @brief Scene start processing */
    void Start(ACScene* scene) override;

protected:
    /** @brief Allocate data */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData() override;

private:
    /** @brief Light registry that current scene has */
    CWeakPtr<CLightRegistry> m_lightRegistry;
};

#endif // !__STATIC_SB_POINT_LIGHTS_H__
