/**
 * @file   ConstantBufferResource.h
 * @brief  This file handles constant buffers.
 * 
 * @author Shodai Makino
 * @date   2023/11/13
 */

#ifndef __CONSTANT_BUFFER_RESOURCE_H__
#define __CONSTANT_BUFFER_RESOURCE_H__

#include "Utility.h"
#include "UtilityForDirectX.h"
#include "Application.h"
#include "UtilityForException.h"

/** @brief This class handles constant buffer */
template<class T>
class CConstantBufferResource {
public:
    /**
       @brief Constructor
    */
    CConstantBufferResource();

    /**
       @brief Constructor that creates constant buffer
       @param maxUsePerFrame The maximum number of buffers that can be used in one frame
    */
    CConstantBufferResource(UINT maxUsePerFrame);
    
    /**
       @brief Create constant buffer
       @param maxUsePerFrame The maximum number of buffers that can be used in one frame
    */
    void Create(UINT maxUsePerFrame);

    /**
       @brief Mapping starts
    */
    void Map();

    /**
       @brief Mapping ends
    */
    void Unmap();

    /**
       @brief Copy data to the constant buffer
       @param src Data to be copy
       @param offset The offset of buffer to be used
    */
    inline void DataCopy(const T* src, UINT offset);

    /**
       @brief Get GPU address of this constant buffer
       @param offset The offset of buffer to be used
       @return GPU address of this constant buffer
       @attention
       The size of buffer unit is multiplied to the offset, so do not multiply it beforehand
    */
    inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress(UINT offset);

    /** @brief Get the size of buffer per */
    UINT GetSize() { return m_alignedSize; }

private:
    /** @brief Constant buffer resource */
    Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBuffer;
    /** @brief Mapped buffer */
    UINT8* m_mappedBuffer;
    /** @brief Is resource being mapped? */
    bool m_isMapped;
    /** @brief Aligned size */
    UINT m_alignedSize;
    /** @brief The maximum number of buffers that can be used in one frame */
    UINT m_maxUsePerFrame;
};

// Constructor
template<class T>
CConstantBufferResource<T>::CConstantBufferResource()
    : m_mappedBuffer(nullptr)
    , m_isMapped(false)
    , m_alignedSize(Utl::Align<UINT>(sizeof(T), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT))
    , m_maxUsePerFrame(0)
{ }

// Constructor that creates constant buffer
template<class T>
CConstantBufferResource<T>::CConstantBufferResource(UINT maxUsePerFrame)
    : m_mappedBuffer(nullptr)
    , m_isMapped(false)
    , m_alignedSize(Utl::Align<UINT>(sizeof(T), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT)) {
    Create(maxUsePerFrame);
}

// Create constant buffer
template<class T>
void CConstantBufferResource<T>::Create(UINT maxUsePerFrame) {
    m_maxUsePerFrame = maxUsePerFrame;
    // Create property and descriptor
    Utl::Dx::HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
    Utl::Dx::RESOURCE_DESC resDesc((UINT64)m_alignedSize * m_maxUsePerFrame);
    // Create constant buffer
    HR_CHECK(_T("Create constant buffer"), CApplication::GetAny().GetDXDevice()->CreateCommittedResource(
        &heapProp, D3D12_HEAP_FLAG_NONE,
        &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(m_constantBuffer.GetAddressOf()))
    );
}

// Mapping starts
template<class T>
void CConstantBufferResource<T>::Map() {
    if (false == m_isMapped) {
        m_constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedBuffer));
        m_isMapped = true;
    }
}

// Mapping ends
template<class T>
void CConstantBufferResource<T>::Unmap() {
    if (true == m_isMapped) {
        m_constantBuffer->Unmap(0, nullptr);
        m_isMapped = false;
    }
}

// Copy data to the constant buffer
template<class T>
inline void CConstantBufferResource<T>::DataCopy(const T* src, UINT offset) {
    if (m_isMapped) {
        memcpy(m_mappedBuffer + (UINT64)offset * m_alignedSize, src, sizeof(T));
    }
    else {
        OutputDebugString(L"Warning! Constant buffer you tried to copy, is being unmapped!\n");
    }
}

// Get GPU address of this constant buffer
template<class T>
inline D3D12_GPU_VIRTUAL_ADDRESS CConstantBufferResource<T>::GetGPUVirtualAddress(UINT offset) {
    return m_constantBuffer->GetGPUVirtualAddress() + (UINT64)offset * m_alignedSize;
}

#endif // !__CONSTANT_BUFFER_RESOURCE_H__
