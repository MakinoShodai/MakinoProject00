/**
 * @file   DepthStencil.h
 * @brief  This file handles depth stencil.
 * 
 * @author Shodai Makino
 * @date   2023/12/11
 */

#ifndef __DEPTH_STENCIL_H__
#define __DEPTH_STENCIL_H__

#include "DescriptorHeap.h"

/** @brief Depth stencil class */
class CDepthStencil {
public:
    /**
       @brief Constructor
    */
    CDepthStencil() = default;

    /**
       @brief Destructor
    */
    ~CDepthStencil() = default;

    /**
       @brief Create depth stencil resource
       @param format Depth stencil format
       @param width Width of render target
       @param height Height of render target
       @param defaultClearValue Default clear value. If this value is used at clear process, processing is optimized
    */
    void Create(DXGI_FORMAT format, UINT width, UINT height, D3D12_DEPTH_STENCIL_VALUE defaultClearValue = { 1.0f, 0});

    /**
       @brief Clear the depth stencil
       @param clearValue Clear value
       @param clearFlag Clear mask
    */
    void Clear(D3D12_DEPTH_STENCIL_VALUE clearValue = { 1.0f, 0 }, D3D12_CLEAR_FLAGS clearFlag = D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);

    /**
       @brief Get SRV property handle for depth buffer
       @return SRV property handle for depth buffer
    */
    Utl::Dx::SRVPropertyHandle GetSRVPropertyHandleForDepth() { return m_srvPropertyForDepth->GetHandle(); }
    /**
       @brief Get SRV property handle for stencil buffer
       @return SRV property handle for stencil buffer
    */
    Utl::Dx::SRVPropertyHandle GetSRVPropertyHandleForStencil() { return m_srvPropertyForStencil->GetHandle(); }

    /** @brief Get CPU handle for dsv descriptor heap */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE GetCPUHandleDSV() const { return m_dsvDescriptorHeap.GetCpuHandle(); }
    /** @brief Get CPU handle for srv descriptor heap for depth buffer */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE GetCPUHandleSRVForDepth() const { return m_srvDescriptorHeap.GetCpuHandle(); }
    /** @brief Get CPU handle for srv descriptor heap for stencil buffer */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE GetCPUHandleSRVForStencil() const { return m_srvDescriptorHeap.GetCpuHandle(1); }
    /** @brief Transition resource state */
    void StateTransition(D3D12_RESOURCE_STATES state) { m_srvPropertyForDepth->stateMaker->StateTransition(state); }

private:
    /**
       @brief Convert DSV format to SRV format for depth buffer
       @param dsvFormat DSV format
       @return SRV format for depth buffer
    */
    DXGI_FORMAT ConvertDSVFormat2SRVFormatForDepth(DXGI_FORMAT dsvFormat);

    /**
       @brief Convert DSV format to SRV format for stencil buffer
       @param dsvFormat DSV format
       @return SRV format for stencil buffer
    */
    DXGI_FORMAT ConvertDSVFormat2SRVFormatForStencil(DXGI_FORMAT dsvFormat);

    /**
       @brief Is specified format uses stencil buffer?
       @param dsvFormat DSV format
       @return Result
    */
    bool IsStencilUseFormat(DXGI_FORMAT dsvFormat);

private:
    /** @brief Depth stencil */
    Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencil;
    /** @brief DSV Descriptor heap */
    CDescriptorHeap m_dsvDescriptorHeap;
    /** @brief SRV Descriptor heap */
    CDescriptorHeap m_srvDescriptorHeap;
    /** @brief SRV Property for depth buffer */
    CUniquePtrWeakable<Utl::Dx::SRVProperty> m_srvPropertyForDepth;
    /** @brief SRV Property for stencil buffer */
    CUniquePtrWeakable<Utl::Dx::SRVPropertyNoStateMaker> m_srvPropertyForStencil;
};

#endif // !__DEPTH_STENCIL_H__
