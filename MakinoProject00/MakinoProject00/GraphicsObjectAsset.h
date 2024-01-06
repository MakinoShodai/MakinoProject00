/**
 * @file   GraphicsObjectAsset.h
 * @brief  This file handles assets that manage what is needed for the drawing process.
 * 
 * @author Shodai Makino
 * @date   2023/11/20
 */

#ifndef __GRAPHICS_OBJECT_ASSET_H__
#define __GRAPHICS_OBJECT_ASSET_H__

#include "UtilityForDirectX.h"
#include "MeshBuffer.h"
#include "Texture.h"

// Forward declaration
class CGraphicsPipelineState;

/** @brief Pair of handle for constant buffer view and its name in hlsl */
using CBVWithNamePair = std::pair<Utl::Dx::ShaderString, Utl::Dx::CPU_DESCRIPTOR_HANDLE>;

/**
   @brief Asset for graphics object
   @details
   When creating this class, it must be CUniquePtrWeakable.
   SRV only needs to be set once, but CBV needs to be set every frame.
*/
class CGraphicsObjectAsset : public ACWeakPtrFromThis<CGraphicsObjectAsset> {
public:
    /**
       @brief Constructor
    */
    CGraphicsObjectAsset() : m_meshBufferPtr(nullptr) {}

    /**
       @brief Destructor
    */
    ~CGraphicsObjectAsset() override = default;

    /**
       @brief Add a shader resource view
       @param tex Texture to be added
    */
    void AddSRV(CTexture tex);

    /**
       @brief Add a constant buffer view 
       @param hlslName Resource name in hlsl
       @param cpuHandle CPU handle for constant buffer view
       @details
       Must be done every frame.
    */
    void AddCBV(Utl::Dx::ShaderString hlslName, Utl::Dx::CPU_DESCRIPTOR_HANDLE cpuHandle);

    /**
       @brief Ask for SRVs assignment information to be built
       @param gpso GPSO to assign SRVs
       @param totalNum Total number of SRVs needed
       @return Have you allocated the needed number of SRVs?
    */
    bool AskToAllocateSRV(const CGraphicsPipelineState& gpso, UINT totalNum);

    /**
       @brief Clear all CBVs
    */
    void ClearCBVs() { m_cbvs.clear(); }

    /** @brief Set pointer to vertex buffer */
    void SetMeshBuff(CWeakPtr<const CMeshBuffer> mesh) { m_meshBufferPtr = mesh; }
    /** @brief Get pointer to vertex buffer */
    const CMeshBuffer* GetMeshBuff() const { return m_meshBufferPtr.Get(); }
    /** @brief Get constant pointer to SRVs array */
    const std::vector<CBVWithNamePair>* GetCBVsArray() { return &m_cbvs; }
    
private:
    /** @brief Pointer to mesh buffer */
    CWeakPtr<const CMeshBuffer> m_meshBufferPtr;
    /** @brief Dynamic array of all SRVs */
    std::vector<CTexture> m_srvs;
    /** @brief Dynamic array of all CBVs */
    std::vector<CBVWithNamePair> m_cbvs;
};

#endif // !__GRAPHICS_OBJECT_ASSET_H__
