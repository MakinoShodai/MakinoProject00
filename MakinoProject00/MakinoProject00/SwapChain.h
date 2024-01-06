/**
 * @file   SwapChain.h
 * @brief  This file handles swap chain.
 * 
 * @author Shodai Makino
 * @date   2023/11/15
 */

#ifndef __SWAP_CHAIN_H__
#define __SWAP_CHAIN_H__

#include "Singleton.h"
#include "DescriptorHeap.h"
#include "UtilityForDirectX.h"
#include "UtilityForAsync.h"
#include "RenderTarget.h"

/** @brief The number of main screen buffer  */
const UINT SCREEN_BUFFERING_NUM = 2;

/** @brief This class handles render target for swap chain */
class CRenderTargetSwapChain : public ACRenderTarget {
public:
    /** @brief Get CPU handle for rtv descriptor heap */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE GetCPUHandleRTV() const override;
    /** @brief Get pointer to resource */
    ID3D12Resource* GetResource() const override;
    /** @brief Transition resource state */
    void StateTransition(D3D12_RESOURCE_STATES state) override;

    /**
       @brief Constructor
    */
    CRenderTargetSwapChain() : ACRenderTarget() {}

    /**
       @brief Create descriptor heap and render target views
       @param swapChainPtr Pointer to the swap chain
       @param format Render target format
    */
    void Create(IDXGISwapChain4* swapChainPtr, DXGI_FORMAT format);

    /**
       @brief Create render target views
       @param swapChainPtr Pointer to the swap chain
       @param format Render target format
    */
    void CreateRTVs(IDXGISwapChain4* swapChainPtr, DXGI_FORMAT format);

    /**
       @brief Release render target buffers
    */
    void BufferRelease();

private:
    /** @brief The screen buffers of a swap chain */
    Microsoft::WRL::ComPtr<ID3D12Resource> m_screenBuffers[SCREEN_BUFFERING_NUM];
    /** @brief Descriptor heap */
    CDescriptorHeap m_descriptorHeap;
    /** @brief Resource state controllers */
    Utl::Dx::ResStateMaker m_resStateController[SCREEN_BUFFERING_NUM];
};

/** @brief Swap chain */
class CSwapChain : public ACMainThreadSingleton<CSwapChain> {
    // Friend declaration
    friend class ACSingletonBase;

public:
    /**
       @brief Destructor
    */
    ~CSwapChain() override = default;

    /**
       @brief Create a swap chain
    */
    void Initialize();

    /**
       @brief Draw and swap the back buffer
    */
    void DrawAndSwap();

    /**
       @brief Wait for present is called in the another thread
    */
    void WaitForPresent();

    /**
       @brief Set full screen mode
       @param isFullScreen Do you want fullscreen ? 
    */
    void SetFullScreen(bool isFullScreen);

    /**
       @brief Get the index of the back buffer to be used for the next drawing
       @return Index of the back buffer to be used for the next drawing
    */
    UINT GetNextBackBufferIndex() { return m_nextBackBufferIndex; }

    /** @brief Feature for thread-safe */
    class CThreadSafeFeature : public ACInnerClass<CSwapChain> {
    public:
        // Friend declaration
        using ACInnerClass<CSwapChain>::ACInnerClass;

        /**
           @brief Get render target for back buffers
           @return Render target for back buffers
        */
        ACRenderTarget* GetBackBuffer() { return &m_owner->m_renderTarget; }

        /** @brief Is it currently in full screen mode? */
        bool IsFullScreen() { return m_owner->m_isFullScreen; }
    };

    /** @brief Get feature for thread-safe */
    inline static CThreadSafeFeature& GetAny() {
        static CThreadSafeFeature instance(&GetProtected());
        return instance;
    }

protected:
    /**
       @brief Constructor
    */
    CSwapChain() : ACMainThreadSingleton(0), m_nextBackBufferIndex(0), m_isFullScreen(false), m_isRebuildBackBuffers(false) {}

    /**
       @brief GPU wait processing after drawing
    */
    void WaitForGPU();

private:
    /** @brief Swap chain */
    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
    /** @brief Render target for back buffers */
    CRenderTargetSwapChain m_renderTarget;
    /** @brief Future object to wait for GPU */
    Utl::Async::CMainThreadFuture<void> m_gpuWaitFuture;
    /** @brief Index of the back buffer to be used for the next drawing */
    UINT m_nextBackBufferIndex;
    /** @brief Is it currently in full screen mode? */
    bool m_isFullScreen;
    /** @brief Is the back buffers to be rebuilt? */
    bool m_isRebuildBackBuffers;
};

#endif // !__SWAP_CHAIN_H__

