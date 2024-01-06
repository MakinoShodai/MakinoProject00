#include "TextureRegistry.h"
#include "AssetCrypter.h"
#include "UtilityForException.h"

// Load texture
bool CTextureRegistry::Load(const std::wstring& filePath) {
    // If specified texture exists in the map, return it
    if (m_nameToHandleMap.contains(filePath)) { return true; }

    // Try to load texture and add it to the map if successful
    TexLoadResult result;
    if (CTextureResource::Load(filePath, &result)) {
        // Build element in the map
        m_nameToHandleMap.emplace(filePath, &result);
        
        return true;
    }

    return false;
}

// Load textures from pak file
bool CTextureRegistry::LoadPak(const std::wstring& filePath) {
    AssetCrypter::LoadFunctionType loadFunc = [this](const void* data, size_t size, const std::wstring& fileName) {
        // Try to load texture and add it to the map if successful
        TexLoadResult result;
        if (CTextureResource::Load(data, size, fileName, &result)) {
            // Build element in the map
            this->m_nameToHandleMap.emplace(fileName, &result);
            return true;
        }
        else {
            return false;
        }
    };

    if (!AssetCrypter::LoadPakFile(filePath, loadFunc)) {
        throw Utl::Error::CFatalError(L"Texture data couldn't be loaded from texture pak file");
        return false;
    }

    return true;
}

// Get a handle for SRV property of the loaded texture
Utl::Dx::SRVPropertyHandle CTextureRegistry::CThreadSafeFeature::GetSRVPropertyHandle(const std::wstring& filePath) {
    auto it = m_owner->m_nameToHandleMap.find(filePath); 

    // If it is not loaded, Output error and Exit application
    if (it == m_owner->m_nameToHandleMap.end()) {
        throw Utl::Error::CFatalError(L"Texture :" + filePath + L"is not loaded.");
    }

    // Return handle
    return it->second.GetSRVPropertyHandle();
}
