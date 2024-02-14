/**
 * @file   ShaderRegistry.h
 * @brief  This file handles registry class that manages all shaders.
 * 
 * @author Shodai Makino
 * @date   2023/12/10
 */

#ifndef __SHADER_REGISTRY_H__
#define __SHADER_REGISTRY_H__

#include "Singleton.h"
#include "Shader.h"

/** @brief Type of vertex shaders */
enum class VertexShaderType : UINT {
    /** @brief Standard shader for 2D sprite */
    Standard2DSprite = 0,
    /** @brief Standard shader for 3D sprite */
    Standard3DSprite,
    /** @brief Standard shader for non-animated model */
    Standard3D,
    /** @brief Standard shader for skeltal model */
    Standard3DAnim,
    /** @brief Standard remove a position from view for non-animated model */
    StandardRemovePos3D,
    /** @brief Light position and world position shader for non-animated model */
    LightPosWorldPos3D,
    /** @brief Light position and world position shader for skeletal model */
    LightPosWorldPos3DAnim,
    /** @brief Light view projection matrix shader for non-animated model */
    LightVP3D,
    /** @brief Light view projection matrix shader for skeletal model */
    LightVP3DAnim,
    /** @brief Light view projection matrix shader (output uv) for non-animated model */
    LightVPOutUV3D,
    /** @brief Max */
    Max
};

/** @brief Type of pixel shaders */
enum class PixelShaderType : UINT {
    /** @brief Standard shader for texture mesh */
    StandardTex = 0,
    /** @brief Standard shader for color only */
    StandardColor,
    /** @brief Writing depth shader */
    DepthWrite,
    /** @brief Shading for using texture */
    ShadingTex,
    /** @brief Shading for color only */
    ShadingColor,
    /** @brief Max */
    Max
};

/** @brief Registry class manages all shaders */
class CShaderRegistry : public ACMainThreadSingleton<CShaderRegistry> {
    // Friend declaration
    friend class ACSingletonBase;
public:
    /**
       @brief Destructor
    */
    ~CShaderRegistry() override = default;
    
    /**
       @brief Initialize
    */
    void Initialize();

    /** @brief Feature for thread-safe */
    class CThreadSafeFeature : public ACInnerClass<CShaderRegistry> {
    public:
        // Friend declaration
        using ACInnerClass<CShaderRegistry>::ACInnerClass;

        /** @brief Get a pointer to a vertex shader */
        CVertexShader* GetVS(VertexShaderType vsType) { return &m_owner->m_vertexShaders[static_cast<UINT>(vsType)]; }
        /** @brief Get a pointer to a pixel shader */
        CPixelShader* GetPS(PixelShaderType psType) { return &m_owner->m_pixelShaders[static_cast<UINT>(psType)]; }
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
    CShaderRegistry() : ACMainThreadSingleton(10) {}

private:
    /** @brief Array of vertex shaders */
    CVertexShader m_vertexShaders[static_cast<UINT>(VertexShaderType::Max)];
    /** @brief Array of pixel shaders */
    CPixelShader m_pixelShaders[static_cast<UINT>(PixelShaderType::Max)];
};

#endif // !__SHADER_REGISTRY_H__
