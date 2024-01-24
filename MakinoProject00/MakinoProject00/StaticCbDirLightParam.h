/**
 * @file   StaticCbDirLightParam.h
 * @brief  This file handles static CB allocater class for parameter of directional light.
 * 
 * @author Shodai Makino
 * @date   2024/1/22
 */

#ifndef __STATIC_CB_DIR_LIGHT_PARAM_H__
#define __STATIC_CB_DIR_LIGHT_PARAM_H__

#include "StaticCbAllocator.h"
#include "DirectionalLight.h"
#include "StaticCbLightVPMat.h"
#include "LightRegistry.h"

/** @brief Parameter for directional light in hlsl */
struct DirLightParam {
    /** @brief Color of directional light */
    Colorf lightColor;
    /** @brief Ambient color */
    Colorf ambientColor;
    /** @brief Direction vector of diretional light */
    Vector4f lightDir;
    /** @brief Position of camera */
    Vector4f cameraPos;
    /** @brief Endpoint position of each cascade Z in the projection space */
    float cascadeEndZ[4];
    /** @brief Shadow camera view projection matrix of the entire cascade */
    DirectX::XMFLOAT4X4 entireShadowVPMatrix;
    /** @brief Offsets after applying the shadow matrix to the object's vertices for each cascades */
    Vector4f cascadeOffsets[CASCADE_NUM];
    /** @brief Scales after applying the shadow matrix to the object's vertices for each cascades */
    Vector4f cascadeScales[CASCADE_NUM];
    /** @brief Intensity of this light */
    float intensity;
    /** @brief The number of point lights */
    UINT pointLightsNum;
};

 /** @brief Static CB allocater class for parameter of directional light */
class CStaticCbDirLightParam : public ACStaticCbAllocator<DirLightParam> {
public:
    /** @brief Constructor */
    CStaticCbDirLightParam() : ACStaticCbAllocator("DirLightParam"), m_directionalLight(nullptr) {}

    /** @brief Scene start processing */
    void Start(ACScene* scene) override;

    /** @brief Set class for computing all light-view projection matrices */
    void SetLightVPCalculater(CSharedPtr<CLightVPCalculator> calculater) { m_lightVPCaculator = calculater; }

protected:
    /** @brief Allocate data */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData() override;

private:
    /** @brief Registry class for lights */
    CWeakPtr<CLightRegistry> m_lightRegistry;
    /** @brief Directional light component that directional light object has */
    CWeakPtr<CDirectionalLightComponent> m_directionalLight;
    /** @brief Class for computing all light-view projection matrices */
    CSharedPtr<CLightVPCalculator> m_lightVPCaculator;
};

#endif // !__STATIC_CB_DIR_LIGHT_PARAM_H__
