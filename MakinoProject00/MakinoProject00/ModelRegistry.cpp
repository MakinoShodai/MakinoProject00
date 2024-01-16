#include "ModelRegistry.h"
#include "AssetCrypter.h"

// Load models from pak file
bool CModelRegistry::LoadModelPak(const std::wstring& filePath) {
    AssetCrypter::LoadFunctionType loadFunc = [this](const void* data, size_t size, const std::wstring& fileName) {
        // Make file path without "Model/"
        size_t start = fileName.find(MODELASSET_DIR);
        std::wstring withOutDirPath = fileName.substr((start != std::wstring::npos) ? start + MODELASSET_DIR.length() : 0, fileName.length());

        // Find descriptor to load model
        auto descIt = this->m_modelLoadDescs->find(withOutDirPath);
        ModelInfo::Load::ModelDesc desc = (descIt != this->m_modelLoadDescs->end()) ? descIt->second : ModelInfo::Load::ModelDesc();

        // Load model and add it to the map
        CUniquePtr<CStaticModelData> model = CUniquePtr<CStaticModelData>::Make();
        model->LoadModel(data, size, fileName, desc);
        // Build element in the map
        this->m_loadedModelMap.emplace(withOutDirPath, std::move(model));
        return true;
    };

    if (!AssetCrypter::LoadPakFile(filePath, loadFunc)) {
        throw Utl::Error::CFatalError(L"Model data couldn't be loaded from model pak file");
    }

    // Clear the descriptor to load animation
    m_modelLoadDescs.Reset();

    return true;
}

// Load animations from pak file
bool CModelRegistry::LoadAnimPak(const std::wstring& filePath) {
    AssetCrypter::LoadFunctionType loadFunc = [this](const void* data, size_t size, const std::wstring& fileName) {
        // Make file path without "Model/"
        size_t start = fileName.find(ANIMASSET_DIR);
        std::wstring withOutDirPath = fileName.substr((start != std::wstring::npos) ? start + ANIMASSET_DIR.length() : 0, fileName.length());

        // Create extracted path of loaded file
        std::wstring extractedPath = ExtractRelevantPart(withOutDirPath);

        // Find a model that support this animation
        for (auto& modelIt : this->m_loadedModelMap) {
            if (ExtractRelevantPart(modelIt.first) == extractedPath) {
                // Find descriptor to load model
                auto descIt = this->m_animLoadDescs->find(withOutDirPath);
                ModelInfo::Load::AnimDesc desc = (descIt != this->m_animLoadDescs->end()) ? descIt->second : ModelInfo::Load::AnimDesc();

                // Load animation and add its ID to the map
                ModelInfo::AnimID animID = modelIt.second->LoadAnimation(data, size, desc);
                this->m_loadedAnimIDMap.emplace(withOutDirPath, animID);

                // Set a model that support this animation to the map for animation to animation interpolation
                auto interIt = this->m_animInterpolationMap->find(withOutDirPath);
                if (interIt != this->m_animInterpolationMap->end()) {
                    interIt->second.model = modelIt.second.Get();
                }

                return true;
            }
        }
        return false;
    };

    if (!AssetCrypter::LoadPakFile(filePath, loadFunc)) {
        throw Utl::Error::CFatalError(L"Animation data couldn't be loaded from animation pak file");
    }

    // Clear the descriptor to load animation
    m_animLoadDescs.Reset();

    // Make setting for animation to animation interpolation
    for (auto& it : *m_animInterpolationMap) {
        CStaticModelData* model = it.second.model;
        if (model == nullptr) {
            throw Utl::Error::CFatalError(L"The model doesn't exist for the animation that is set to interpolate! path : " + it.first);
        }

        // Get ID of this animation
        ModelInfo::AnimID thisID = m_loadedAnimIDMap[it.first];

        for (auto& targetIt : it.second.nameAndSettings) {
            // Get ID of target for interpolation
            auto targetAnimIt = m_loadedAnimIDMap.find(targetIt.first);
            if (targetAnimIt == m_loadedAnimIDMap.end()) {
                throw Utl::Error::CFatalError(L"The target animation with interpolation settings doesn't exist! path : " + targetIt.first);
            }
            ModelInfo::AnimID targetID = targetAnimIt->second;

            // Add setting for interpolation
            model->AddAnimInterpolationSetting(thisID, targetID, targetIt.second);
        }
    }

    // Clear map for animation to animation interpolation
    m_animInterpolationMap.Reset();

    return true;
}

// Add descriptor to load model
void CModelRegistry::AddModelLoadDesc(const std::wstring& filePath, ModelInfo::Load::ModelDesc loadDesc) {
    if (m_modelLoadDescs == nullptr) { throw Utl::Error::CFatalError(L"All models have already been loaded"); }
    m_modelLoadDescs->emplace(filePath, std::move(loadDesc));
}

// Add descriptor to load animation
void CModelRegistry::AddAnimLoadDesc(const std::wstring& filePath, ModelInfo::Load::AnimDesc loadDesc) {
    if (m_animLoadDescs == nullptr) { throw Utl::Error::CFatalError(L"All animations have already been loaded"); }
    m_animLoadDescs->emplace(filePath, std::move(loadDesc));
}

// Add setting for animation to animation interpolation
void CModelRegistry::AddAnimToAnimSetting(const std::wstring& srcFilePath, const std::wstring& destFilePath, ModelInfo::InterpolationSetting setting) {
    if (m_animInterpolationMap == nullptr) { throw Utl::Error::CFatalError(L"All interpolation setting have already been created"); }
    if (!m_animInterpolationMap->contains(srcFilePath)) {
        m_animInterpolationMap->emplace(srcFilePath, ForMakeAnimToAnimData());
    }
    (*m_animInterpolationMap)[srcFilePath].nameAndSettings.push_back(std::make_pair(destFilePath, setting));
}

// Get a static data of a model
const CStaticModelData& CModelRegistry::CThreadSafeFeature::GetModel(const std::wstring& filePath) {
    auto it = m_owner->m_loadedModelMap.find(filePath);
    if (it != m_owner->m_loadedModelMap.end()) {
        return *it->second;
    }
    else {
        throw Utl::Error::CFatalError(L"The model for specified path doesn't exist in the map! path : " + filePath);
    }
}

// Constructor
CModelRegistry::CModelRegistry()
    : ACMainThreadSingleton(-10) {
    m_modelLoadDescs = CUniquePtr<std::unordered_map<std::wstring, ModelInfo::Load::ModelDesc>>::Make();
    m_animLoadDescs = CUniquePtr<std::unordered_map<std::wstring, ModelInfo::Load::AnimDesc>>::Make();
    m_animInterpolationMap = CUniquePtr<std::map<std::wstring, ForMakeAnimToAnimData>>::Make();
}

// Extract relevant part from sent path
std::wstring CModelRegistry::ExtractRelevantPart(const std::wstring& path) {
    size_t end = path.rfind(L'/'); // Find the location of "/" from the ending
    return path.substr(0, end);
}
