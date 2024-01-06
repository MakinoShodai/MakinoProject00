#include "Shader.h"
#include <d3dcompiler.h>
#include "Application.h"
#include "UtilityForString.h"
#include "UtilityForException.h"

// To create shader reflection
#pragma comment(lib, "d3dcompiler.lib")

// Format constant
const DXGI_FORMAT FORMATS[][4] = {
    { // UINT
        DXGI_FORMAT_R32_UINT,
        DXGI_FORMAT_R32G32_UINT,
        DXGI_FORMAT_R32G32B32_UINT,
        DXGI_FORMAT_R32G32B32A32_UINT,
    },
    { // SINT
        DXGI_FORMAT_R32_SINT,
        DXGI_FORMAT_R32G32_SINT,
        DXGI_FORMAT_R32G32B32_SINT,
        DXGI_FORMAT_R32G32B32A32_SINT,
    },
    { // FLOAT
        DXGI_FORMAT_R32_FLOAT,
        DXGI_FORMAT_R32G32_FLOAT,
        DXGI_FORMAT_R32G32B32_FLOAT,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
    },
};

// Dynamic identifier for HLSL resources
const char* const RESOURCE_SUFFIX_DYNAMIC = "_D";
// Static identifier for HLSL resources
const char* const RESOURCE_SUFFIX_STATIC = "_S";

// Loading shader files
void ACShader::Load(const std::string& filePath) {
    // Open the file while moving the file pointer to the end.
    std::ifstream csoFile(filePath, std::ios::binary | std::ios::ate); // read only
    // In case of reading failure
    if (!csoFile.good()) {
        throw Utl::Error::CFatalError(L".cso file could not be read! : " + Utl::Str::string2WString(filePath));
    }

    // Get the file size and create an array accordingly.
    // # NOTE: tellg returns the current file pointer potision.
    m_fileSize = static_cast<size_t>(csoFile.tellg());
    m_csoData = CUniquePtr<std::byte[]>::Make(m_fileSize);

    // Return file pointer.
    csoFile.seekg(0);

    // File read and close.
    csoFile.read(reinterpret_cast<char*>(m_csoData.Get()), m_fileSize);
    csoFile.close();

    // Get the shader reflection
    Microsoft::WRL::ComPtr<ID3D12ShaderReflection> reflection = nullptr;
    HRESULT hr = D3DReflect(m_csoData.Get(), m_fileSize, IID_PPV_ARGS(reflection.GetAddressOf()));
    if (FAILED(hr)) {
        throw Utl::Error::CFatalError(L"Could not get shader reflection from shader! : " + Utl::Str::string2WString(filePath));
    }

    // Get the shader descriptor
    D3D12_SHADER_DESC shaderDesc;
    reflection->GetDesc(&shaderDesc);

    // Create descriptor ranges
    for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
        D3D12_SHADER_INPUT_BIND_DESC bindDesc;
        reflection->GetResourceBindingDesc(i, &bindDesc);
        
        // Branching based on resource type
        switch (bindDesc.Type) {
        // Constant buffer
        case D3D_SIT_CBUFFER:
            AddDescriptorRange(bindDesc, D3D12_DESCRIPTOR_RANGE_TYPE_CBV);
            break;
        // Structured buffer
        case D3D_SIT_STRUCTURED:
            AddDescriptorRange(bindDesc, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
            break;
        // Texture
        case D3D_SIT_TEXTURE:
            AddDescriptorRange(bindDesc, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
            break;
        // Sampler
        case D3D_SIT_SAMPLER:
        {
            // Add sampler
            m_samplers.emplace_back();
            SamplerInfo& samplerInfo = m_samplers.back();
            samplerInfo.slotNum = bindDesc.BindPoint;
            samplerInfo.spaceNum = bindDesc.Space;
            samplerInfo.name = bindDesc.Name;
            break;
        }
        // Others
        default:
            break;
        }
    }

    // if there is processing to be done in derived class at load time, do it.
    LoadDerived(reflection, shaderDesc);
}

// Bind descriptor range and sampler to descriptor table for static resources that don't need to be updated on a per-object basis
void ACShader::BindStaticDescriptorTable(std::vector<D3D12_ROOT_PARAMETER>* descArray, std::vector<D3D12_STATIC_SAMPLER_DESC>* sampArray) {
    // If the this shader, has descriptor ranges for dynamic resources
    if (m_staticDescriptorRanges.size() > 0) {
        descArray->emplace_back();
        D3D12_ROOT_PARAMETER& descriptorTable = descArray->back();
        descriptorTable.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;                // Descriptor table
        descriptorTable.ShaderVisibility                    = Utl::Dx::ConvertShaderType2ShaderVisibility(m_shaderType); // Set shader visiblity from shader type
        descriptorTable.DescriptorTable.pDescriptorRanges   = &m_staticDescriptorRanges[0];                             // Set descriptor range
        descriptorTable.DescriptorTable.NumDescriptorRanges = (UINT)m_staticDescriptorRanges.size();                    // Set descriptor range count
    }

    // If the this shader has sampler infomations
    if (m_samplers.size() > 0) {
        for (const auto& it : m_samplers) {
            sampArray->push_back(it.sampler);
        }
    }
}

// Bind descriptor range and sampler to descriptor table for dynamic resources that need to be updated on a per-object basis
void ACShader::BindDynamicDescriptorTable(std::vector<D3D12_ROOT_PARAMETER>* descArray) {
    // If the this shader, has descriptor ranges for dynamic resources
    if (m_dynamicDescriptorRanges.size() > 0) {
        descArray->emplace_back();
        D3D12_ROOT_PARAMETER& descriptorTable = descArray->back();
        descriptorTable.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;                // Descriptor table
        descriptorTable.ShaderVisibility                    = Utl::Dx::ConvertShaderType2ShaderVisibility(m_shaderType); // Set shader visiblity from shader type
        descriptorTable.DescriptorTable.pDescriptorRanges   = &m_dynamicDescriptorRanges[0];                             // Set descriptor range
        descriptorTable.DescriptorTable.NumDescriptorRanges = (UINT)m_dynamicDescriptorRanges.size();                    // Set descriptor range count
    }
}

// Set sampler state
void ACShader::SetSampler(std::initializer_list<SamplerTransmitter> settings) {
    for (const auto& setting : settings) {
        // Search for the same name in the array
        bool isExist = false;
        for (auto& it : m_samplers) {
            if (it.name != setting.name) { continue; }

            // Bind to the sampler and flag it as existing because same name existed in the samplers being hold
            Gpso::BindSamplerState(&it.sampler, setting.setting, Utl::Dx::ConvertShaderType2ShaderVisibility(m_shaderType), it.slotNum, it.spaceNum);
            isExist = true;
        }

        // If it is not exist
        if (false == isExist) {
            throw Utl::Error::CFatalError(L"The name of samplers set as an argument in Shader's 'SetSampler' function doesn't match the name of samplers a shader has.");
        }
    }
}

// Add Descriptor range
void ACShader::AddDescriptorRange(const D3D12_SHADER_INPUT_BIND_DESC& bindDesc, D3D12_DESCRIPTOR_RANGE_TYPE rangeType) {
    std::vector<D3D12_DESCRIPTOR_RANGE>* descriptorRanges = nullptr;
    std::unordered_map<std::string, ShaderResourceInfo>* nameToInfoMap = nullptr;

    // Change variable depending on whether the resource is static or dynamic
    if (Utl::Str::CheckSuffixChar(bindDesc.Name, RESOURCE_SUFFIX_DYNAMIC)) {
        descriptorRanges = &m_dynamicDescriptorRanges;
        nameToInfoMap = &m_dynamicNameToInfoMap;
    }
    else if (Utl::Str::CheckSuffixChar(bindDesc.Name, RESOURCE_SUFFIX_STATIC)) {
        descriptorRanges = &m_staticDescriptorRanges;
        nameToInfoMap = &m_staticNameToInfoMap;
    }
    else {
        throw Utl::Error::CFatalError(L"Resource that loaded shader has, isn't allocated dynamic or static!");
    }

    // Add new descriptor range to the array
    descriptorRanges->emplace_back();

    // Set variable
    D3D12_DESCRIPTOR_RANGE& descRange           = descriptorRanges->back();
    descRange.RangeType                         = rangeType;                            // Resource type
    descRange.NumDescriptors                    = bindDesc.BindCount;                   // Count of descriptor
    descRange.BaseShaderRegister                = bindDesc.BindPoint;                   // Register number
    descRange.RegisterSpace                     = bindDesc.Space;                       // Register space
    descRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Automatic offset determination

    // Set descriptor range name and its index to static map
    size_t strLength = strlen(bindDesc.Name);
    nameToInfoMap->emplace(std::string(bindDesc.Name, strLength - 2), ShaderResourceInfo(bindDesc.Type, (UINT)descriptorRanges->size() - 1));
}

// Destructor
CVertexShader::~CVertexShader() {
    for (UINT i = 0; i < m_inputLayout.Size(); ++i) {
        if (m_inputLayout[i].SemanticName != nullptr) {
            delete[] m_inputLayout[i].SemanticName;
        }
    }
}

// Bind this shader to the graphics pipeline state
void CVertexShader::BindGPSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpso) {
    // Set the vertex shader
    gpso->VS.pShaderBytecode = m_csoData.Get();
    gpso->VS.BytecodeLength = m_fileSize;

    // Set the input layout
    gpso->InputLayout.pInputElementDescs = m_inputLayout.Get();
    gpso->InputLayout.NumElements = m_inputLayout.Size();
}

// Processing vertex shader at load time
void CVertexShader::LoadDerived(const Microsoft::WRL::ComPtr<ID3D12ShaderReflection>& shaderReflection, const D3D12_SHADER_DESC& shaderDesc) {
    // Get the size of the input layout setting and prepare an array.
    m_inputLayout.Resize(shaderDesc.InputParameters);

    for (UINT i = 0; i < shaderDesc.InputParameters; ++i) {
        // Get the parameters structure.
        D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
        shaderReflection->GetInputParameterDesc(i, &paramDesc);

        // Set various parameters.
        m_inputLayout[i].SemanticName         = Utl::Str::ExtractString(paramDesc.SemanticName); // Semantics name
        m_inputLayout[i].SemanticIndex        = paramDesc.SemanticIndex;                         // Semantics index
        m_inputLayout[i].InputSlot            = 0;                                               // All inputs are grouped in a structure, so there is one. Set to 0
        m_inputLayout[i].AlignedByteOffset    = D3D12_APPEND_ALIGNED_ELEMENT;                    // Follow the order
        m_inputLayout[i].InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;      // This is not instance rendering
        m_inputLayout[i].InstanceDataStepRate = 0;                                               // This is not instance rendering, so set it to 0.

        // Compute number of components used.
        // # NOTE: Mask uses only 4 bits.
        BYTE num = paramDesc.Mask;
        num = (num & 0b0101) + ((num >> 1) & 0b0101);
        num = (num & 0b0011) + ((num >> 2) & 0b0011);

        // Guarantee that num is less than 4.
        assert(num <= 4);

        // Specify format by component kind.
        switch (paramDesc.ComponentType) {
        case D3D_REGISTER_COMPONENT_UINT32:
            m_inputLayout[i].Format = FORMATS[0][num - 1]; // UINT
            break;
        case D3D_REGISTER_COMPONENT_SINT32:
            m_inputLayout[i].Format = FORMATS[1][num - 1]; // SINT
            break;
        case D3D_REGISTER_COMPONENT_FLOAT32:
            m_inputLayout[i].Format = FORMATS[2][num - 1]; // FLOAT
            break;
        }
    }
}

// Bind this shader to the graphic pipeline state
void CPixelShader::BindGPSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpso) {
    // Set the vertex shader
    gpso->PS.pShaderBytecode = m_csoData.Get();
    gpso->PS.BytecodeLength = m_fileSize;
}
