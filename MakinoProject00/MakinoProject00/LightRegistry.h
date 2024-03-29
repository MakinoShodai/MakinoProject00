/**
 * @file   LightRegistry.h
 * @brief  This file handles registry for lights.
 * 
 * @author Shodai Makino
 * @date   2024/1/24
 */

#ifndef __LIGHT_REGISTRY_H__
#define __LIGHT_REGISTRY_H__

#include "PointLight.h"
#include "DirectionalLight.h"

/** @brief Registry class for lights */
class CLightRegistry {
public:
    /**
       @brief Constructor
    */
    CLightRegistry() = default;

    /**
       @brief Destructor
    */
    ~CLightRegistry() = default;

    /**
       @brief Add point light to the array
       @param point Point light to be added
    */
    void AddPointLight(CWeakPtr<CPointLightComponent> point) { m_pointLights.push_back(point); }

    /**
       @brief Remove point light from the array
       @param point Point light to be removed
    */
    void RemovePointLight(CPointLightComponent* point);

    /**
       @brief Set directional light
       @param dirLight Directional light to be set
       @return Is a directional light already exists on the scene?
    */
    bool SetDirectionalLight(CWeakPtr<CDirectionalLightComponent> dirLight);

    /**
       @brief Get point lights that the scene exists
       @return Array of point lights
    */
    const std::vector<CWeakPtr<CPointLightComponent>>& GetPointLights() { return m_pointLights; }

    /** @brief Get directional light */
    CWeakPtr<CDirectionalLightComponent> GetDirectionalLight() { return m_dirLight; }

    /** @brief Get the number of point lights */
    UINT GetPointLightsNum() { return (UINT)m_pointLights.size(); }

private:
    /** @brief Point lights */
    std::vector<CWeakPtr<CPointLightComponent>> m_pointLights;
    /** @brief Directional light */
    CWeakPtr<CDirectionalLightComponent> m_dirLight;
};

#endif // !__LIGHT_REGISTRY_H__
