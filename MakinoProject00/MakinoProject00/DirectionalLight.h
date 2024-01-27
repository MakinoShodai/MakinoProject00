/**
 * @file   DirectionalLight.h
 * @brief  This file handles component and object for directional light.
 * 
 * @author Shodai Makino
 * @date   2024/1/21
 */

#ifndef _DIRECTIONAL_LIGHT_H__
#define _DIRECTIONAL_LIGHT_H__

#include "Component.h"
#include "GameObject.h"

/** @brief Name of directional light object */
const std::wstring OBJNAME_DIRECTIONAL_LIGHT = L"Directional light";

/** @brief Component for directional light */
class CDirectionalLightComponent : public ACComponent {
public:
    using ACComponent::ACComponent;

    /**
       @brief Awake processing
    */
    void Awake();

    /**
       @brief Starting process
    */
    void Start();

    /** @brief Set direction vector of this light */
    void SetLightDir(const Vector3f& dir);

    /** @brief Get light color */
    const Colorf& GetLightColor() const { return m_lightColor; }
    /** @brief Set light color */
    void SetLightColor(const Colorf& color) { m_lightColor = color; }

    /** @brief Get ambient color */
    const Colorf& GetAmbientColor() const { return m_ambientColor; }
    /** @brief Set ambient color */
    void SetAmbientColor(const Colorf& color) { m_ambientColor = color; }

    /** @brief Get intensity */
    float GetIntensity() const { return m_intensity; }
    /** @brief Set intensity */
    void SetIntensity(float intensity) { m_intensity = intensity; }

private:
    /** @brief Color of directional light */
    Colorf m_lightColor;
    /** @brief Ambient color */
    Colorf m_ambientColor;
    /** @brief Intensity of this light */
    float m_intensity;
};

/** @brief Prefab for directional light */
class CDirectionalLightPrefab : public CGameObject {
public:
    using CGameObject::CGameObject;

    /**
       @brief Prefab function
    */
    void Prefab() override;
};
REGISTER_PREFABCLASS(CDirectionalLightPrefab);

#endif // !_DIRECTIONAL_LIGHT_H__
