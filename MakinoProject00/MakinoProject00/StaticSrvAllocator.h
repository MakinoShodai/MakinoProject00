/**
 * @file   StaticSrvAllocator.h
 * @brief  This file handles class for allocating static resource SRV.
 * 
 * @author Shodai Makino
 * @date   2024/1/21
 */

#ifndef __STATIC_SRV_ALLOCATOR_H__
#define __STATIC_SRV_ALLOCATOR_H__

#include "StaticResource.h"
#include "TextureRegistry.h"

 /** @brief Class for allocating static resource SRV */
class ACStaticSRVAllocator : public IStaticResource {
public:
    /**
       @brief Allocate SRV handle
       @param rtt Render target texture to be allocated
       @param isUsedPixelShader Use with pixel shaders?
    */
    void Allocate(CRenderTargetTexture* rtt, bool isUsedPixelShader = true);

    /**
       @brief Allocate SRV handle
       @param dsv Depth stencil texture to be allocated
       @param isForStencil false to specify texture as depth, true to specify as stencil
       @param isUsedPixelShader Use with pixel shaders?
    */
    void Allocate(CDepthStencil* dsv, bool isForStencil, bool isUsedPixelShader = true);

    /**
       @brief Allocate SRV handle
       @param texPath Path of texture file to be allocated
       @param isUsedPixelShader Use with pixel shaders?
    */
    void Allocate(const std::wstring& texPath, bool isUsedPixelShader = true);

    /**
       @brief Get handle of allocated data in resource
       @return Resource CPU handle
    */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle() override final;

    /**
       @brief Refresh process that must be called at the end of every frame
    */
    void Refresh() override final;

    /** @brief Get the name of resource defined in hlsl */
    const std::string& GetName() override { return m_hlslResouceName; }

protected:
    /**
       @brief Constructor
       @param hlslResourceName The name of resouce defined in hlsl
    */
    ACStaticSRVAllocator(const std::string& hlslResourceName) : m_hlslResouceName(hlslResourceName), m_resState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) { }

    /**
       @brief Destructor
    */
    virtual ~ACStaticSRVAllocator() = default;

    /**
       @brief Refresh process that must be called at the end of every frame, for derived class
    */
    virtual void RefreshDerived() {}

    /**
       @brief Allocate and return data
       @return Allocated data
    */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData() override final { m_srvHandle.GetStateController()->StateTransition(m_resState); return m_srvHandle.GetCPUHandle(); }

private:
    /** @brief The name of resouce defined in hlsl */
    const std::string m_hlslResouceName;
    /** @brief Transition destination resource state */
    D3D12_RESOURCE_STATES m_resState;
    /** @brief Handle for SRV property */
    Utl::Dx::SRVPropertyHandle m_srvHandle;
};

#endif // !__STATIC_SRV_ALLOCATOR_H__
