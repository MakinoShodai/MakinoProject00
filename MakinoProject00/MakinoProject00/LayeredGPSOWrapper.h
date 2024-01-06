/**
 * @file   LayeredGPSOWrapper.h
 * @brief  This file handles class that wraps GPSO with common layers and different settings for each component type.
 * 
 * @author Shodai Makino
 * @date   2024/1/1
 */

#ifndef __LAYERED_GPSO_WRAPPER_H__
#define __LAYERED_GPSO_WRAPPER_H__

#include "GraphicsPipelineElements.h"
#include "GraphicsPipelineState.h"
#include "GraphicsComponent.h"

/** @brief GPSO setting for overriding and graphics component types using it */
struct GPSOSettingOverrideWithType {
    /** @brief Overriding part of the default setting */
    Gpso::GPSOSettingOverride setting;
    /** @brief Graphics component types using this setting  */
    std::vector<GraphicsComponentType> types;

    /**
       @brief Constructor
       @param types Graphics component types using this setting
    */
    GPSOSettingOverrideWithType(std::initializer_list<GraphicsComponentType> types) { 
        for (auto type : types) {
            this->types.push_back(type);
        }
    }
};

/** @brief Setting for CLayeredGPSOWrapper */
struct LayeredGPSOSetting {
    /** @brief Default setting */
    Gpso::GPSOSetting defaultSetting;
    /** @brief Graphics component types using default setting */
    std::vector<GraphicsComponentType> defaultTypes;
    /** @brief GPSO settings for overriding and graphics component types using it */
    std::vector<GPSOSettingOverrideWithType> overrideSettings;

    /**
       @brief Constructor
       @param types Graphics component types using default setting
    */
    LayeredGPSOSetting(std::initializer_list<GraphicsComponentType> types) {
        for (auto type : types) {
            this->defaultTypes.push_back(type);
        }
    }

    /**
       @brief Add override setting
       @param types Graphics component types using override setting
       @return Pointer to added setting
    */
    Gpso::GPSOSettingOverride* AddOverrideSetting(std::initializer_list<GraphicsComponentType> types) {
        overrideSettings.emplace_back(GPSOSettingOverrideWithType(types)); 
        return &overrideSettings.back().setting;
    }
};

/** @brief This class wraps GPSO with common layers and different settings for each component type */
class CLayeredGPSOWrapper {
public:
    /**
       @brief Constructor
    */
    CLayeredGPSOWrapper();

    /** @brief Destructor */
    virtual ~CLayeredGPSOWrapper() = default;

    /**
       @brief Create graphics pipeline states
       @param scene Scene that has this graphics pipeline states
       @param gpsoName The name of this gpso wrapper. Used for debugging
       @param setting Graphics pipeline state setting
       @param useLayer Layers handled by this GPSO (Draw in the order passed)
    */
    void Create(ACScene* scene, const std::wstring gpsoName, const LayeredGPSOSetting& setting, std::initializer_list<GraphicsLayer> useLayers);

    /**
       @brief Call 'SetCommand' function from GPSOs wrapped this class
    */
    void SetCommand();

    /**
       @brief Call 'EndFrameProcess' function from GPSOs wrapped this class
    */
    void EndFrameProcess();

private:
    /** @brief Managed graphics pipeline state objects */
    std::vector<CGraphicsPipelineState> m_gpso;

#ifdef _DEBUG
    /** @brief The name of this gpso wrapper. Used for debbuging */
    std::wstring m_name;
#endif // _DEBUG
};

#endif // !__LAYERED_GPSO_WRAPPER_H__
