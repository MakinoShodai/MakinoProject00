#include "Texture.h"

// Constructor for texture
CTexture::CTexture(Utl::Dx::ShaderString hlslName, const std::wstring& filePath)
    : m_hlslName(std::move(hlslName))
    , m_handle(CTextureRegistry::GetAny().GetSRVPropertyHandle(filePath))
{ }

// Constructor for render target texture
CTexture::CTexture(Utl::Dx::ShaderString hlslName, CRenderTargetTexture* rtt)
    : m_hlslName(std::move(hlslName))
    , m_handle(rtt->GetSRVPropertyHandle())
{ }

CTexture::CTexture(Utl::Dx::ShaderString hlslName, const Utl::Dx::SRVPropertyHandle& srvPropertyHandle)
    : m_hlslName(std::move(hlslName))
    , m_handle(srvPropertyHandle)
{ }
