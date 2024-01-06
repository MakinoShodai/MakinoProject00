/**
 * @file   Texture.h
 * @brief  This file handles textures.
 * 
 * @author Shodai Makino
 * @date   2023/11/19
 */

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "UtilityForDirectX.h"
#include "RenderTarget.h"
#include "TextureRegistry.h"

/** @brief This class handles something like a reference to a texture and does'nt have an entity */
class CTexture {
public:
    /**
       @brief Constructor
       @param hlslName name of resource in hlsl
       @param filePath The file path of texture to use
    */
    CTexture(Utl::Dx::ShaderString hlslName, const std::wstring& filePath);

    /**
       @brief Constructor
       @param hlslName name of resource in hlsl
       @param rtt Render target texture to be used
    */
    CTexture(Utl::Dx::ShaderString hlslName, CRenderTargetTexture* rtt);

    /**
       @brief Constructor
       @param hlslName name of resource in hlsl
       @param srvPropertyHandle SRV property handle
    */
    CTexture(Utl::Dx::ShaderString hlslName, const Utl::Dx::SRVPropertyHandle& srvPropertyHandle);

    /** @brief Copy constructor */
    CTexture(const CTexture& other) = default;
    /** @brief Copy constructor */
    CTexture& operator=(const CTexture& other) = default;
    /** @brief Move constructor */
    CTexture(CTexture&& other) = default;
    /** @brief Move assignment constructor */
    CTexture& operator=(CTexture&& other) = default;

    /** @brief Get name of texture in hlsl */
    const Utl::Dx::ShaderString& GetHLSLName() const { return m_hlslName; }
    /** @brief Get handle for SRV property */
    Utl::Dx::SRVPropertyHandle* GetHandle() { return &m_handle; }
    
private:
    /** @brief The name of texture in hlsl */
    Utl::Dx::ShaderString m_hlslName;
    /** @brief Handle for SRV property */
    Utl::Dx::SRVPropertyHandle m_handle;
};

#endif // !__TEXTURE_H__
