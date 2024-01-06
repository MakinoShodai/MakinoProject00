/**
 * @file   Sprite.h
 * @brief  This file handles sprite.
 * 
 * @author Shodai Makino
 * @date   2023/12/6
 */

#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "GraphicsComponent.h"

/** @brief Abstract class for sprite */
class ACSprite : public ACGraphicsComponent {
public:
    /**
       @brief Constructor
       @param type Type of this graphics component
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
       @param texFilePath File path of texture to be used
    */
    ACSprite(GraphicsComponentType type, CGameObject* owner, GraphicsLayer layer, const std::wstring& texFilePath);

    /**
       @brief Destructor
    */
    virtual ~ACSprite() = default;

    /**
       @brief Initialize shader class
    */
    static void Initialize();

    /**
       @brief Get width of sprite resolution
       @return Width of sprite resolution
    */
    UINT GetWidth() { return m_width; }

    /**
       @brief Get height of sprite resolution
       @return Height of sprite resolution
    */
    UINT GetHeight() { return m_height; }

    /** @brief Get resolution ratio of sprite  */
    float GetResolutionRatio() { return m_resolutionRatio; }

private:
    /** @brief Common vertex buffer */
    static CUniquePtrWeakable<CVertexBuffer> ms_vertexBuffer;
    /** @brief Common index buffer */
    static CUniquePtrWeakable<CIndexBuffer> ms_indexBuffer;

    /** @brief Resolution ratio of sprite. width / height */
    float m_resolutionRatio;
    /** @brief Width of sprite resolution */
    UINT m_width;
    /** @brief Height of sprite resolution */
    UINT m_height;
};

/** @brief Sprite class for UI */
class CSpriteUI : public ACSprite {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
       @param texFilePath File path of texture to be used
    */
    CSpriteUI(CGameObject* owner, GraphicsLayer layer, const std::wstring& texFilePath) : ACSprite(GraphicsComponentType::SpriteUI, owner, layer, texFilePath) {}
};

/** @brief Sprite class for 3D coordinate space */
class CSprite3D : public ACSprite {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
       @param texFilePath File path of texture to be used
    */
    CSprite3D(CGameObject* owner, GraphicsLayer layer, const std::wstring& texFilePath) : ACSprite(GraphicsComponentType::Sprite3D, owner, layer, texFilePath) {}
};

/** @brief Billboard class */
class CBillboard : public ACSprite {
public:
    /**
       @brief Constructor
       @param owner Game object that is the owner of this graphics component
       @param layer Graphics layer to which this class belongs
       @param texFilePath File path of texture to be used
    */
    CBillboard(CGameObject* owner, GraphicsLayer layer, const std::wstring& texFilePath) : ACSprite(GraphicsComponentType::Billboard, owner, layer, texFilePath) {}
};

#endif // !__SPRITE_H__
