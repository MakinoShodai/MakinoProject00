/**
 * @file   TextureRegistry.h
 * @brief  This file handles all texture resources together in the registry.
 * 
 * @author Shodai Makino
 * @date   2023/11/19
 */

#ifndef __TEXTURE_REGISTRY_H__
#define __TEXTURE_REGISTRY_H__

#include "Singleton.h"
#include "TextureResource.h"
#include "DescriptorHeap.h"
#include "UtilityForDirectX.h"

/** @brief Registry for all textures */
class CTextureRegistry : public ACMainThreadSingleton<CTextureRegistry> {
    // Friend declaration
    friend class ACSingletonBase;
public:
    /**
       @brief Destructor
    */
    ~CTextureRegistry() override = default;

    /**
       @brief Load texture
       @param filePath The path of texture file
       @return Is texture existed?
    */
    bool Load(const std::wstring& filePath);

    /**
       @brief Load textures from pak file
       @param filePath The path of pak file
       @return Successful loading?
    */
    bool LoadPak(const std::wstring& filePath);

    /** @brief Feature for thread-safe */
    class CThreadSafeFeature : public ACInnerClass<CTextureRegistry> {
    public:
        // Friend declaration
        using ACInnerClass<CTextureRegistry>::ACInnerClass;

        /**
           @brief Does the registry contain textures for the specified file path?
           @param filePath The path of texture file
           @return Is texture contained?
        */
        bool IsContains(const std::wstring& filePath) { return m_owner->m_nameToHandleMap.contains(filePath); }

        /**
           @brief Get a handle for SRV property of the loaded texture
           @param filePath Path of a texture file to be gotten
           @return handle for SRV property of the texture with specified path
           @details
           If the texture with specified path doesn't exist in the map, throw fatal exception
        */
        Utl::Dx::SRVPropertyHandle GetSRVPropertyHandle(const std::wstring& filePath);
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
    CTextureRegistry() : ACMainThreadSingleton(-5) {}

private:
    /** @brief Map that associate texture names with its handles */
    std::unordered_map<std::wstring, CTextureResource> m_nameToHandleMap;
};

#endif // !__TEXTURE_REGISTRY_H__
