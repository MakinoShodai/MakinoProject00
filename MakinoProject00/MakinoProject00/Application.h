/**
 * @file   Application.h
 * @brief  This file handles application objects such as window and DirectX device.
 * 
 * @author Shodai Makino
 * @date   2023/10/25
 */

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Singleton.h"
#include "UtilityForMath.h"

/** @brief Singleton class for handling application objects such as window and DirectX device */
class CApplication : public ACMainThreadSingleton<CApplication> {
    // Friend declaration
    friend class ACSingletonBase;

public:
    /**
       @brief Destructor
    */
    ~CApplication() = default;

    /**
       @brief Create and display a window
       @param wndProc Window procedure function
       @param width Width of window size
       @param height Height of window size
       @param name Name of Window or window class
    */
    void InitializeWnd(WNDPROC wndProc, UINT width, UINT height, const std::wstring& name);

    /**
       @brief Destroy window instance
    */
    void DestroyWnd();

    /**
       @brief Initialize DirectX such as DirectX device and DXGI factory
    */
    void InitializeDirectX();

    /** @brief Feature for thread-safe */
    class CThreadSafeFeature : public ACInnerClass<CApplication> {
    public:
        // Friend declaration
        using ACInnerClass<CApplication>::ACInnerClass;

        /** @brief Window handle getter */
        HWND GetWndHandle() const { return m_owner->m_wndHandle; }
        /** @brief DirectX device getter */
        ID3D12Device* GetDXDevice() { return m_owner->m_dxDevice.Get(); }
        /** @brief DXGI factory object getter */
        IDXGIFactory6* GetDXGIFactory() { return m_owner->m_dxgiFactory.Get(); }
        /** @brief Get the size of descriptor heap type */
        UINT GetHeapTypeSize(D3D12_DESCRIPTOR_HEAP_TYPE type) { return m_owner->m_heapTypeSize[type]; }
        /** @brief Magnification of current window size relative to ideal window size */
        const Vector2f& GetWndSizeMagnification() { return m_owner->m_wndSizeMagnification; }
        /** @brief Get range of cursor position */
        const RECT& GetCursorRange() { return m_owner->m_cursorRange; }
    };

    /** @brief Get feature for thread-safe */
    inline static CThreadSafeFeature& GetAny() {
        static CThreadSafeFeature instance(GetProtected().Get());
        return instance;
    }

protected:
    /**
       @brief Constrcutor
    */
    CApplication() : ACMainThreadSingleton(9999), m_wndSetting(), m_wndHandle(NULL), m_wndSizeMagnification(Vector2f::Ones()), m_cursorRange{} {}

    /**
       @brief Process to be called at instance destruction
    */
    void OnDestroy() override;

    /**
       @brief Calculate the range of cursor position
    */
    void CalculateCursorRange();

private:
    /** @brief Window setting */
    WNDCLASSEX m_wndSetting;
    /** @brief Window handle */
    HWND m_wndHandle;
    /** @brief DirectX device */
    Microsoft::WRL::ComPtr<ID3D12Device> m_dxDevice;
    /** @brief DXGI factory object */
    Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory;
    /** @brief Magnification of current window size relative to ideal window size */
    Vector2f m_wndSizeMagnification;

    /** @brief Range of cursor position */
    RECT m_cursorRange;
    /** @brief Sizes of descriptor heap for each types */
    UINT m_heapTypeSize[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
};

#endif // !__APPLICATION_H__
