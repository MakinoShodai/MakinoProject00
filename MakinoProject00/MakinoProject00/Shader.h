/**
 * @file   Shader.h
 * @brief  This file handles each shaders.
 *
 * @author Shodai Makino
 * @date   2023/10/21
 */

#ifndef __SHADER_H__
#define __SHADER_H__

#include "Application.h"
#include "GraphicsPipelineElements.h"
#include "UtilityForDirectX.h"
#include "UniquePtr.h"

/** @brief To send sampler state to shader */
struct SamplerTransmitter {
    /** @brief Sampler state Setting */
    Gpso::SamplerStateSetting setting;
    /** @brief Variable name in hlsl */
    std::string name;
    /**
       @brief Constructor
       @param setting Sampler state Setting
       @param name Variable name in hlsl
    */
    SamplerTransmitter(const Gpso::SamplerStateSetting& setting, const std::string& name) {
        this->setting = setting;
        this->name = name;
    }
};

/** @brief Sampler information */
struct SamplerInfo {
    /** @brief Sampler descriptor */
    D3D12_STATIC_SAMPLER_DESC sampler;
    /** @brief Register number */
    UINT slotNum;
    /** @brief Space number */
    UINT spaceNum;
    /** @brief Variable name in hlsl */
    std::string name;
};

/** @brief Information for shader resource */
struct ShaderResourceInfo {
    /** @brief Resoruce type */
    _D3D_SHADER_INPUT_TYPE type;
    /** @brief index by descriptor range */
    UINT index;

    /** @brief Constructor */
    ShaderResourceInfo(_D3D_SHADER_INPUT_TYPE type, UINT index) : type(type), index(index) {}
};

/** @brief Abstract class for shaders */
class ACShader {
public:
    /**
       @brief Constructor
    */
    ACShader(Utl::Dx::ShaderType type) : m_fileSize(0), m_shaderType(type) { }

    /**
       @brief Destructor
    */
    virtual ~ACShader() {}

    /**
       @brief Loading shader files
       @param filePath File path where the shader cso file is located
    */
    void Load(const std::string& filePath);

    /**
       @brief Bind descriptor range and sampler to descriptor table for static resources that don't need to be updated on a per-object basis
       @param descArray Dynamic array of descriptor table for static resources
       @param sampArray Dynamic array of sampler state
    */
    void BindStaticDescriptorTable(std::vector<D3D12_ROOT_PARAMETER>* descArray, std::vector<D3D12_STATIC_SAMPLER_DESC>* sampArray);

    /**
       @brief Bind descriptor range and sampler to descriptor table for dynamic resources that need to be updated on a per-object basis
       @param descArray Dynamic array of descriptor table
    */
    void BindDynamicDescriptorTable(std::vector<D3D12_ROOT_PARAMETER>* descArray);

    /**
       @brief Bind this shader to the graphics pipeline state
       @param gpso Graphics pipe state to where you want to bind this shader
    */
    virtual void BindGPSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpso) = 0;

    /**
       @brief Set sampler state
       @param settings Specify sampler state settings to be used with argument name
    */
    void SetSampler(std::initializer_list<SamplerTransmitter> settings);
    
    /** @brief Get Shader type */
    Utl::Dx::ShaderType GetShaderType() { return m_shaderType; }
    /** @brief Get map to search information for shader resources (for dynamic resources) */
    const std::unordered_map<std::string, ShaderResourceInfo>& GetDescriptorNameToInfoDynamicMap() { return m_dynamicNameToInfoMap; }
    /** @brief Get map to search information for shader resources (for static resources) */
    const std::unordered_map<std::string, ShaderResourceInfo>& GetDescriptorNameToInfoStaticMap() { return m_staticNameToInfoMap; }

protected:
    /**
       @brief Processing in derived classes at load time
       @param shaderReflection Loaded shader reflection
       @param shaderDesc Loaded shader descriptor
    */
    virtual void LoadDerived(const Microsoft::WRL::ComPtr<ID3D12ShaderReflection>& shaderReflection, const D3D12_SHADER_DESC& shaderDesc) {}

private:
    /**
       @brief Add Descriptor range
       @param bindDesc Bind descriptor structure of the descriptor range to be added
       @param rangeType Range type of the descriptor range to be added
    */
    void AddDescriptorRange(const D3D12_SHADER_INPUT_BIND_DESC& bindDesc, D3D12_DESCRIPTOR_RANGE_TYPE rangeType);

    /**
       @brief Check and remove suffix
       @param str String to be checked
       @param suffix Suffix
       @return Did it contain a suffix?
       @details
       Ignore array specifiers to determine
    */
    bool CheckAndRemoveSuffix(std::string* str, const std::string& suffix);

protected:
    /** @brief Shader type */
    Utl::Dx::ShaderType m_shaderType;
    /** @brief map to search information for shader resources (for dynamic resources) */
    std::unordered_map<std::string, ShaderResourceInfo> m_dynamicNameToInfoMap;
    /** @brief map to search information for shader resources (for static resources) */
    std::unordered_map<std::string, ShaderResourceInfo> m_staticNameToInfoMap;
    /** @brief Descriptor ranges for dynamic resources */
    std::vector<D3D12_DESCRIPTOR_RANGE> m_dynamicDescriptorRanges;
    /** @brief Descriptor ranges for static resources */
    std::vector<D3D12_DESCRIPTOR_RANGE> m_staticDescriptorRanges;
    /** @brief Sampler informations */
    std::vector<SamplerInfo> m_samplers;
    /** @brief Read cso data */
    CUniquePtr<std::byte[]> m_csoData;
    /** @brief File size */
    size_t m_fileSize;
};

/** @brief Vertex shader class */
class CVertexShader : public ACShader {
public:
    /**
       @brief Constructor
    */
    CVertexShader() : ACShader(Utl::Dx::ShaderType::Vertex) {}
    /**
       @brief Constructor that loads shader
       @param filePath File path for loading the cso file
    */
    CVertexShader(const std::string& filePath) : ACShader(Utl::Dx::ShaderType::Vertex) { Load(filePath); }

    /**
       @brief Destructor
    */
    ~CVertexShader() override;

    /**
       @brief Bind this shader to the graphics pipe line state
       @param gpso Graphics pipeline state to where you want to bind this shader
    */
    void BindGPSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpso) override;

protected:
    /**
       @brief Unique Processing in vertex shader at load time
       @param shaderReflection Loaded shader reflection
       @param shaderDesc Loaded shader descriptor
    */
    void LoadDerived(const Microsoft::WRL::ComPtr<ID3D12ShaderReflection>& shaderReflection, const D3D12_SHADER_DESC& shaderDesc) override;

protected:
    /** @brief Input layout */
    CArrayUniquePtr<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
};

/** @brief Pixel shader class */
class CPixelShader : public ACShader {
public:
    /**
       @brief Constructor
    */
    CPixelShader() : ACShader(Utl::Dx::ShaderType::Pixel) {}
    /**
       @brief Conatructor that loads shader file
       @param filePath File path for loading the cso file
    */
    CPixelShader(const std::string& filePath) : ACShader(Utl::Dx::ShaderType::Pixel) { Load(filePath); }

    /**
       @brief Bind this shader to the graphics pipeline state
       @param gpso Graphics pipeline state to where you want to bind this shader
    */
    void BindGPSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpso) override;
};

#endif // !__SHADER_H__
