/**
 * @file   StaticCbLightVPMat.h
 * @brief  This file handles the static CB allocater of the light's view projection matrix.
 * 
 * @author Shodai Makino
 * @date   2024/1/21
 */

#ifndef __STATIC_CB_LIGHT_VP_MAT_H__
#define __STATIC_CB_LIGHT_VP_MAT_H__

#include "StaticCbAllocator.h"
#include "DirectionalLight.h"
#include "SharedPtr.h"
#include "LightRegistry.h"

/** @brief Number of cascade shadow map */
const UINT CASCADE_NUM = 3;

/** @brief Pixel size of shadow map */
const UINT SHADOWMAP_SIZE = 4096;

/** @brief All light view projection matrices in hlsl */
struct AllLightVP {
    /** @brief View projection matrix for each cascade */
    DirectX::XMFLOAT4X4 m_cascadeVPMatrices[CASCADE_NUM];
};

/** @brief Class for computing all light-view projection matrices */
class CLightVPCalculator {
public:
    /**
       @brief Constructor
    */
    CLightVPCalculator();

    /** @brief Set light registry */
    void SetLightRegistry(CWeakPtr<CLightRegistry> lightRegistry) { m_lightRegistry = lightRegistry; }

    /**
       @brief Calculate all light view projection matrices and return it
       @return Pointer to all light view projection matrices
    */
    const AllLightVP* Calculate();

    /**
       @brief Set local endpoint position of each cascade Z
       @param firstEnd First cascade
       @param secondEnd Second cascade
    */
    void SetCascadeEndZ(float firstEnd, float secondEnd) { m_cascadeSplitVals[1] = firstEnd; m_cascadeSplitVals[2] = secondEnd; }

    /**
       @brief Reset calculated flag
    */
    void ResetCalculated() { m_isCalculated = false; }

    /** @brief Get local startpoint position and endpoint position of each cascade Z */
    float GetCascadeEndZ(UINT index) { return m_cascadeSplitsPassShader[index]; }

    /** @brief Get the shadow camera view projection matrix of the entire frustum */
    const DirectX::XMFLOAT4X4& GetShadowMatrix() { return m_entireShadowMatrix; }
    /** @brief Get the offset after applying the shadow matrix to the object's vertices */
    const Vector4f& GetCascadeOffset(UINT index) { return m_cascadeOffsets[index]; }
    /** @brief Get the scale after applying the shadow matrix to the object's vertices */
    const Vector4f& GetCascadeScale(UINT index) { return m_cascadeScales[index]; }

private:
    /**
       @brief Calculate the shadow camera view projection matrix of the entire frustum
       @param lightDir Direction vector of directional light
       @param frustumCorners Coordinates of each vertex of the entire frustum in world space
       @param toUVSpaceMatrix Matrix for offsetting from [-1, 1] projection space to [0, 1] UV space
       @param entireShadowMatrix Return variable that receives the shadow camera view projection matrix of the entire frustum
    */
    void CalculateShadowMatrix(const Vector3f& lightDir, const Vector3f* frustumCorners, const DirectX::XMMATRIX& toUVSpaceMatrix, DirectX::XMMATRIX* entireShadowMatrix);

private:
    /** @brief Light registry */
    CWeakPtr<CLightRegistry> m_lightRegistry;
    /** @brief All light view projection matrices */
    AllLightVP m_lightVPs;
    /** @brief Have you finished computing the values in current frame? */
    bool m_isCalculated;
    /** @brief Values to split the each cascade */
    float m_cascadeSplitVals[CASCADE_NUM];
    /** @brief Values to split the each cascade for passing on to shaders */
    float m_cascadeSplitsPassShader[CASCADE_NUM];
    /** @brief Shadow camera view projection matrix of the entire frustum */
    DirectX::XMFLOAT4X4 m_entireShadowMatrix;
    /** @brief Offsets after applying the shadow matrix to the object's vertices for each cascades */
    Vector4f m_cascadeOffsets[CASCADE_NUM];
    /** @brief Scales after applying the shadow matrix to the object's vertices for each cascades */
    Vector4f m_cascadeScales[CASCADE_NUM];
};

/** @brief Static CB allocater class that handles light view projection matrix */
class CStaticCbLightVP : public ACStaticCbAllocator<DirectX::XMFLOAT4X4> {
public:
    /** @brief Constructor */
    CStaticCbLightVP();

    /** @brief Scene start processing */
    void Start(CScene* scene) override;

    /**
       @brief Advance to the next cascade shadow map
    */
    void Advance();

protected:
    /** @brief Allocate data */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData() override;

    /** @brief Refresh process */
    void RefreshDerived() override;

    /**
       @brief Advance to the next data assignment?
    */
    bool IsAdvanceNextAllocate() override { return m_isAdvance; }

private:
    /** @brief Class for computing all light-view projection matrices */
    CSharedPtr<CLightVPCalculator> m_lightVPCaculator;
    /** @brief Index of current cascades */
    UINT m_cascadesIndex;
    /** @brief Advance to the next data assignment? */
    bool m_isAdvance;
};

#endif // !__STATIC_CB_LIGHT_VP_MAT_H__
