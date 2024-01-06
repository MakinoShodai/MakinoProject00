/**
 * @file   RenderTarget.h
 * @brief  This file handles render targets.
 * 
 * @author Shodai Makino
 * @date   2023/11/23
 */

#ifndef __RENDER_TARGET_H__
#define __RENDER_TARGET_H__

#include "UtilityForDirectX.h"
#include "DescriptorHeap.h"
#include "DepthStencil.h"

/** @brief This class handles render target */
class ACRenderTarget {
public:
    /** @brief Get pointer to current applied render target */
    static ACRenderTarget* GetCurrentAppliedRenderTarget() { return ms_currentAppliedRenderTarget; }
    /** @brief Get pointer to current applied depth stencil */
    static CDepthStencil* GetCurrentAppliedDepthStencil() { return ms_currentAppliedDepthStencil; }

    /** @brief Get CPU handle for rtv descriptor heap */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE GetCPUHandleRTV() const = 0;
    /** @brief Get pointer to resource */
    virtual ID3D12Resource* GetResource() const = 0;
    /** @brief Transition resource state */
    virtual void StateTransition(D3D12_RESOURCE_STATES state) = 0;
    
    /**
       @brief Constructor
    */
    ACRenderTarget() : m_width(NULL), m_height(NULL) {}

    /**
       @brief Destructor
    */
    ~ACRenderTarget() { if (ms_currentAppliedRenderTarget == this) { ms_currentAppliedRenderTarget = nullptr; } }

    /**
       @brief Apply this render target to the destination render target by calling 'OMSetRenderTargets'
       @param dsv Depth stencil to be used. If it's not used, please set this argument to nullptr
       @param otherRT When doing multiple render targets, specify other render targets
    */
    void Apply(CDepthStencil* dsv, std::initializer_list<ACRenderTarget*> otherRT = {});

    /**
       @brief Clear the color of all pixels on this render target
       @param color Color to clear
    */
    void Clear(const Colorf& color);

    /** @brief Get width of resolution */
    UINT GetWidth() { return m_width; }

    /** @brief Get height of resolution */
    UINT GetHeight() { return m_height; }

protected:
    /**
       @brief Set resolution
       @param renderTarget Own render target
    */
    void SetResolution(ID3D12Resource* renderTarget);

protected:
    /** @brief Pointer to current applied render target */
    static ACRenderTarget* ms_currentAppliedRenderTarget;
    /** @brief Pointer to current applied depth stencil */
    static CDepthStencil* ms_currentAppliedDepthStencil;
    /** @brief Width of render target */
    UINT m_width;
    /** @brief Height of render target */
    UINT m_height;
};

/** @brief This class handles render target that can also be used as SRV */
class CRenderTargetTexture : public ACRenderTarget {
public:
    /** @brief Get CPU handle for rtv descriptor heap */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE GetCPUHandleRTV() const override { return m_rtvDescriptorHeap.GetCpuHandle(); }
    /** @brief Get CPU handle for srv descriptor heap */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE GetCPUHandleSRV() const { return m_srvDescriptorHeap.GetCpuHandle(); }
    /** @brief Get pointer to resource */
    ID3D12Resource* GetResource() const override { return m_renderTarget.Get(); }
    /** @brief Transition resource state */
    void StateTransition(D3D12_RESOURCE_STATES state) override { m_srvProperty->stateMaker->StateTransition(state); }

    /**
       @brief Constructor
    */
    CRenderTargetTexture() : ACRenderTarget() {}

    /**
       @brief Create render target resource
       @param format Render target format
       @param width Width of render target
       @param height Height of render target
       @param defaultClearColor Default clear color. If this value is used at clear process, processing is optimized
    */
    void Create(DXGI_FORMAT format, UINT width, UINT height, const Colorf& defaultClearColor);

    /**
       @brief Get handle for SRV property
       @return Handle for SRV property
    */
    Utl::Dx::SRVPropertyHandle GetSRVPropertyHandle() { return m_srvProperty->GetHandle(); }

private:
    /** @brief Render target */
    Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTarget;
    /** @brief RTV Descriptor heap */
    CDescriptorHeap m_rtvDescriptorHeap;
    /** @brief SRV Descriptor heap */
    CDescriptorHeap m_srvDescriptorHeap;
    /** @brief SRV Property */
    CUniquePtrWeakable<Utl::Dx::SRVProperty> m_srvProperty;
};

#endif // !__RENDER_TARGET_H__
