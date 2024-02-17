/**
 * @file   ModelRegistry.h
 * @brief  This file handles registry class that manages all loaded models.
 * 
 * @author Shodai Makino
 * @date   2023/12/27
 */

#ifndef __MODEL_REGISTRY_H__
#define __MODEL_REGISTRY_H__

#include "Singleton.h"
#include "LoadedModelInfo.h"
#include "UtilityForException.h"

/** @brief Directory of model */
const std::wstring MODELASSET_DIR = L"Model/";
/** @brief Directory of animation */
const std::wstring ANIMASSET_DIR = L"Anim/";

/** @brief Data to make animation to animation interpolation */
struct ForMakeAnimToAnimData {
    /** @brief Static data of model supported own animation */
    CStaticModelData* model;
    /** @brief Names of animation for transition destination and its settings */
    std::vector<std::pair<std::wstring, ModelInfo::InterpolationSetting>> nameAndSettings;

    /** @brief Constructor */
    ForMakeAnimToAnimData() : model(nullptr) {}
};

/** @brief This class manages all loaded models */
class CModelRegistry : public ACMainThreadSingleton<CModelRegistry> {
    // Friend declaration
    friend class ACSingletonBase;

public:
    /** @brief Destructor */
    ~CModelRegistry() = default;

    /**
       @brief Load models from pak file
       @param filePath The path of pak file
       @return Successful loading?
    */
    bool LoadModelPak(const std::wstring& filePath);

    /**
       @brief Load animations from pak file
       @param filePath The path of pak file
       @return Successful loading?
    */
    bool LoadAnimPak(const std::wstring& filePath);

    /**
       @brief Add descriptor to load model
       @param filePath Path of model file
       @param loadDesc Descriptor to load model
    */
    void AddModelLoadDesc(const std::wstring& filePath, ModelInfo::Load::ModelDesc loadDesc);

    /**
       @brief Add the additional texture of model
       @param modelFilePath Path of model file
       @param srcTexPath Texture file path of the source to be added
       @param additionalTex Additional texture
    */
    void AddModelAdditionalTex(const std::wstring& modelFilePath, const std::wstring& srcTexPath, ModelInfo::Load::AdditionalModelTex additionalTex);

    /**
       @brief Add a texture of the material to be forced in the transparent layer
       @param modelFilePath Path of model file
       @param transparentTexPath Path of basic texture file of the material to be forced in the transparent layer
    */
    void AddModelTransparentTex(const std::wstring& modelFilePath, const std::wstring& transparentTexPath);

    /**
       @brief Add descriptor to load animation
       @param filePath Path of model file
       @param loadDesc Descriptor to load animation
    */
    void AddAnimLoadDesc(const std::wstring& filePath, ModelInfo::Load::AnimDesc loadDesc);

    /**
       @brief Add setting for animation to animation interpolation
       @param srcFilePath Path of animation file for transition source
       @param destFilePath Path of animation file for transition destination
       @param setting Setting for animation to animation interpolation
    */
    void AddAnimToAnimSetting(const std::wstring& srcFilePath, const std::wstring& destFilePath, ModelInfo::InterpolationSetting setting);

    /** @brief Feature for thread-safe */
    class CThreadSafeFeature : public ACInnerClass<CModelRegistry> {
    public:
        // Friend declaration
        using ACInnerClass<CModelRegistry>::ACInnerClass;

        /**
           @brief Get a static data of a model
           @param filePath Path of a model file to be gotten
           @return Const reference to a static data of a model
           @details
           If the model with specified path doesn't exist in the map, throw fatal exception
        */
        const CStaticModelData& GetModel(const std::wstring& filePath);

        /**
           @brief Get ID of a loaded animation
           @param filePath Path of animation file
           @return ID of a loaded animation
        */
        const ModelInfo::AnimID GetAnimID(const std::wstring& filePath);
    };

    /** @brief Get feature for thread-safe */
    inline static CThreadSafeFeature& GetAny() {
        static CThreadSafeFeature instance(GetProtected().Get());
        return instance;
    }

protected:
    /**
       @brief Constructor
    */
    CModelRegistry();

    /**
       @brief Extract relevant part from sent path
       @param path Path
       @return Extracted string
    */
    static std::wstring ExtractRelevantPart(const std::wstring& path);

private:
    /** @brief Map for descriptor to load model */
    CUniquePtr<std::unordered_map<std::wstring, ModelInfo::Load::ModelDesc>> m_modelLoadDescs;
    /** @brief Map for descriptor to load animation */
    CUniquePtr<std::unordered_map<std::wstring, ModelInfo::Load::AnimDesc>> m_animLoadDescs;
    /** @brief Map for animation to animation interpolation */
    CUniquePtr<std::map<std::wstring, ForMakeAnimToAnimData>> m_animInterpolationMap;
    /** @brief Map for loaded models */
    std::unordered_map<std::wstring, CUniquePtr<CStaticModelData>> m_loadedModelMap;
    /** @brief Map for loaded animations ID */
    std::unordered_map<std::wstring, ModelInfo::AnimID> m_loadedAnimIDMap;
};

#endif // !__MODEL_REGISTRY_H__
