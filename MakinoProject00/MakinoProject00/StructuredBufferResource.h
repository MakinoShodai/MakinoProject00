/**
 * @file   StructuredBufferResource.h
 * @brief  This file handles resouce of structured buffer.
 * 
 * @author Shodai Makino
 * @date   2023/12/15
 */

#ifndef __STRUCTURED_BUFFER_RESOURCE_H__
#define __STRUCTURED_BUFFER_RESOURCE_H__

#include "UtilityForDirectX.h"
#include "UtilityForException.h"
#include "Application.h"

/**
   @brief Resource class for structured buffer 
   @tparam T Type of this structured buffer. If this is set to void, so specify the type in the constructor
*/
template <class T>
class CStructuredBufferResource {
public:
    /**
       @brief Constructor (T is not void)
    */
    CStructuredBufferResource() requires (!std::is_void_v<T>);

    /**
       @brief Constructor (T is void)
       @param typeSize Size of this structured buffer type
    */
    CStructuredBufferResource(size_t typeSize) requires std::is_void_v<T>;

    /**
       @brief Create structured buffer resource
       @param arraySize Size of array that structured buffer has
    */
    void Create(UINT arraySize);

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
       @param copyNum The number of elements to be copied
       @param offset The offset of buffer to be copied
    */
    inline void DataCopy(const T* src, UINT copySize, UINT offset);

    /**
       @brief Get GPU address of this structured buffer
       @param index Index of array that structured buffer has
       @return GPU address of this structured buffer
    */
    inline D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress(UINT index);

    /** @brief Get structured buffer resource */
    ID3D12Resource* GetResource() { return m_structuredBuffer.Get(); }

private:
    /** @brief Structured buffer resource */
    Microsoft::WRL::ComPtr<ID3D12Resource> m_structuredBuffer;
    /** @brief Mapped buffer */
    UINT8* m_mappedBuffer;
    /** @brief Is resource being mapped? */
    bool m_isMapped;
    /** @brief Size of array that structured buffer has */
    UINT m_arraySize;
    /** @brief Size of structured buffer type */
    UINT m_typeSize;
};

// Constructor (T is not void)
template<class T>
CStructuredBufferResource<T>::CStructuredBufferResource() requires (!std::is_void_v<T>)
    : m_mappedBuffer(nullptr)
    , m_isMapped(false)
    , m_arraySize(0)
    , m_typeSize((UINT)sizeof(T))
{ }

// Constructor (T is void)
template<class T>
CStructuredBufferResource<T>::CStructuredBufferResource(size_t typeSize) requires std::is_void_v<T>
    : m_mappedBuffer(nullptr)
    , m_isMapped(false)
    , m_arraySize(0)
    , m_typeSize((UINT)typeSize)
{ }

// Create structured buffer resource
template<class T>
void CStructuredBufferResource<T>::Create(UINT arraySize) {
    m_arraySize = arraySize;
    // Create property and descriptor
    Utl::Dx::HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
    Utl::Dx::RESOURCE_DESC resDesc(m_arraySize * (UINT64)m_typeSize);
    // Create structured buffer
    HR_CHECK(_T("Create structured buffer"), CApplication::GetAny().GetDXDevice()->CreateCommittedResource(
        &heapProp, D3D12_HEAP_FLAG_NONE,
        &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(m_structuredBuffer.GetAddressOf()))
    );
}

// Mapping starts
template<class T>
void CStructuredBufferResource<T>::Map() {
    if (false == m_isMapped) {
        m_structuredBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedBuffer));
        m_isMapped = true;
    }
}

// Mapping ends
template<class T>
void CStructuredBufferResource<T>::Unmap() {
    if (true == m_isMapped) {
        m_structuredBuffer->Unmap(0, nullptr);
        m_isMapped = false;
    }
}

// Copy data to the constant buffer
template<class T>
inline void CStructuredBufferResource<T>::DataCopy(const T* src, UINT copyNum, UINT offset) {
    if (m_isMapped) {
        memcpy(m_mappedBuffer + offset * (UINT64)m_typeSize, src, (UINT64)m_typeSize * copyNum);
    }
    else {
        throw Utl::Error::CStopDrawingSceneError(L"Warning! Structured buffer you tried to copy, is being unmapped!\n");
    }
}

// Get GPU address of this structured buffer
template<class T>
inline D3D12_GPU_VIRTUAL_ADDRESS CStructuredBufferResource<T>::GetGPUVirtualAddress(UINT index) {
    return m_structuredBuffer->GetGPUVirtualAddress() + index * (UINT64)m_typeSize;
}

#endif // !__STRUCTURED_BUFFER_RESOURCE_H__
