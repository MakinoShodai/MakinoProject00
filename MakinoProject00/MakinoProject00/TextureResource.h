/**
 * @file   TextureResource.h
 * @brief  This file handles texture resources.
 * 
 * @author Shodai Makino
 * @date   2023/11/13
 */

#ifndef __TEXTURE_RESOURCE_H__
#define __TEXTURE_RESOURCE_H__

#include "GpuOptimizedResource.h"
#include "UtilityForDirectX.h"
#include "DescriptorHeap.h"

// Forward declaration
class CTextureResource;

/** @brief Structure that stores the result of texture reading */
struct TexLoadResult {
    // Friend declaration
    friend class CTextureResource;

private:
    /** @brief Texture meta data */
    DirectX::TexMetadata metaData;
    /** @brief Scratch image */
    DirectX::ScratchImage scratchImg;
};

/** @brief Data for each specific texture, such as a specific mipmap level texture */
struct PerTexProperty {
    /** @brief width */
    size_t width;
    /** @brief height */
    size_t height;
    /** @brief row pitch */
    size_t rowPitch;
    /** @brief slice pitch */
    size_t slicePitch;
};

/** @brief This class handles texture resource */
class CTextureResource : public ACGpuOptimizedResource {
public:
    /**
       @brief Constructor
    */
    CTextureResource() : ACGpuOptimizedResource(), m_metaData(), m_perTexProperty() {}

    /**
       @brief Constructor for creating texture resource
       @param loadResult Load function results
    */
    CTextureResource(TexLoadResult* loadResult) : ACGpuOptimizedResource(), m_metaData(), m_perTexProperty() { Create(loadResult); }

    /**
       @brief Create texture resource
       @param loadResult Load function results
    */
    void Create(TexLoadResult* loadResult);

    /** @brief Get texture meta data */
    const DirectX::TexMetadata& GetMetaData() const { return m_metaData; }
    /** @brief Get resouce state controller */
    Utl::Dx::SRVPropertyHandle GetSRVPropertyHandle() { return m_srvProperty->GetHandle(); }

    /**
       @brief Load texture
       @param filePath File path of texture to be loaded
       @param result Return variable for loading result
       @return Successful loading?
    */
    static bool Load(const std::wstring& filePath, TexLoadResult* result);


    /**
       @brief Load texture
       @param data Texture data to be loaded
       @param size Size of texture data to be loaded
       @param filePath Path of file be loaded
       @param result Return variable for loading result
       @return Successful loading?
    */
    static bool Load(const void* data, size_t size, const std::wstring& filePath, TexLoadResult* result);

protected:
    /**
       @brief Update heap properties and resource desc for resource
    */
    void UpdateDataForResource(D3D12_HEAP_PROPERTIES* heapProp, D3D12_RESOURCE_DESC* resDesc) override;

    /**
       @brief Set command to copy data
    */
    void CopyRegion(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* srcResource) override;

private:
    /** @brief Texture meta data */
    DirectX::TexMetadata m_metaData;
    /** @brief Data for each specific texture, such as a specific mipmap level texture */
    PerTexProperty m_perTexProperty;
    /** @brief Properties for shader resource view */
    CUniquePtrWeakable<Utl::Dx::SRVProperty> m_srvProperty;
    /** @brief Descriptor heap */
    CDescriptorHeap m_descriptorHeap;
};

#endif // !__TEXTURE_RESOURCE_H__
