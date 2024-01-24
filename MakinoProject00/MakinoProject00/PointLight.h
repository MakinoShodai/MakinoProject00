/**
 * @file   PointLight.h
 * @brief  This file handles point light component.
 * 
 * @author Shodai Makino
 * @date   2024/1/24
 */

#ifndef __POINT_LIGHT_H__
#define __POINT_LIGHT_H__

#include "Component.h"

/** @brief Component for point light */
class CPointLightComponent : public ACComponent {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this component
       @param color Color of this light
    */
    CPointLightComponent(CGameObject* owner, Colorf color = Colorf::Ones());

    /**
       @brief Starting process
    */
    void Start() override;

    /**
       @brief Process at enable
    */
    void OnEnable() override;

    /**
       @brief Process at disable
    */
    void OnDisable() override;

    /** @brief Set constant attenuation (1 / constant + k1*d + k2*d*d) */
    void SetConstantAttenuation(float att) { m_constantAttenuation = att; }
    /** @brief Set linear attenuation (1 / k0 + linear * d + k2*d*d) */
    void SetLinearAttenuation(float att) { m_linearAttenuation = att; }
    /** @brief Set quadratic attenuation (1 / k0 + k1*d + quadratic * d * d) */
    void SetQuadraticAttenuation(float att) { m_quadraticAttenuation = att; }

    /** @brief Get intensity */
    float GetIntensity() { return m_intensity; }
    /** @brief Set intensity */
    void SetIntensity(float intensity) { m_intensity = intensity; }

    /** @brief Get all atenuation parameters */
    Vector4f GetAllAttenuations() { return Vector4f(m_constantAttenuation, m_linearAttenuation, m_quadraticAttenuation, 0.0f); }

    /** @brief Get color */
    const Colorf& GetColor() const { return m_color; }
    /** @brief Set color */
    void SetColor(const Colorf& color) { m_color = color; }

private:
    /** @brief Color of this light */
    Colorf m_color;
    /** @brief Constant attenuation (1 / constant + k1*d + k2*d*d) */
    float m_constantAttenuation;
    /** @brief Linear attenuation (1 / k0 + linear * d + k2*d*d) */
    float m_linearAttenuation;
    /** @brief Quadratic attenuation (1 / k0 + k1*d + quadratic * d * d) */
    float m_quadraticAttenuation;
    /** @brief Intensity */
    float m_intensity;
    /** @brief Has the start process already been done? */
    bool m_isStart;
};

#endif // !__POINT_LIGHT_H__
