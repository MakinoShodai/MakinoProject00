#include "SwapChain.h"
#include "Application.h"
#include "CommandManager.h"
#include "ThreadPool.h"
#include "UtilityForException.h"

//#define _SEQUENTIAL_GPU_
#define _UNLIMITED_REFRESHRATE_

#ifdef _UNLIMITED_REFRESHRATE_
const UINT SWAPCHAIN_FLAGS = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
#else
const UINT SWAPCHAIN_FLAGS = 0;
#endif // _UNLIMITED_REFRESHRATE_

// Create descriptor heap and render target views
void CRenderTargetSwapChain::Create(IDXGISwapChain4* swapChainPtr, DXGI_FORMAT format) {
    assert(swapChainPtr != nullptr);
    // Create the desciptor heap
    m_descriptorHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, SCREEN_BUFFERING_NUM);

    // Create render target views
    CreateRTVs(swapChainPtr, format);
}

// Create render target views
void CRenderTargetSwapChain::CreateRTVs(IDXGISwapChain4* swapChainPtr, DXGI_FORMAT format) {
    // Create a render target view descriptor
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format               = format;                        // Specified format
    rtvDesc.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D; // 2D texture
    rtvDesc.Texture2D.MipSlice   = 0;                             // Mipmap is not used
    rtvDesc.Texture2D.PlaneSlice = 0;                             // Not use

    // Get screen buffers
    for (UINT i = 0; i < SCREEN_BUFFERING_NUM; ++i) {
        // Get the screen buffers of swap chain
        HR_CHECK(L"Get the screen buffer of swap chain", swapChainPtr->GetBuffer(i, IID_PPV_ARGS(m_screenBuffers[i].GetAddressOf())));
        // Create render target view
        CApplication::GetAny().GetDXDevice()->CreateRenderTargetView(m_screenBuffers[i].Get(), &rtvDesc,
            m_descriptorHeap.GetCpuHandle(i));

        // Create resource state controller
        m_resStateController[i].Reset();
        m_resStateController[i] = CUniquePtrWeakable<Utl::Dx::ResStateMaker>::Make();
        m_resStateController[i]->Create(m_screenBuffers[i].Get(), D3D12_RESOURCE_STATE_PRESENT);
    }

    // Update resolution
    SetResolution(m_screenBuffers[0].Get());
}

// Release render target buffers
void CRenderTargetSwapChain::BufferRelease() {
    for (UINT i = 0; i < SCREEN_BUFFERING_NUM; ++i) {
        m_screenBuffers[i].Reset();
    }
}

// Get CPU handle for descriptor heap
Utl::Dx::CPU_DESCRIPTOR_HANDLE CRenderTargetSwapChain::GetCPUHandleRTV() const {
    return m_descriptorHeap.GetCpuHandle()
        + CSwapChain::GetMain().GetNextBackBufferIndex() * CApplication::GetAny().GetHeapTypeSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

// Get pointer to resource
ID3D12Resource* CRenderTargetSwapChain::GetResource() const {
    return m_screenBuffers[CSwapChain::GetMain().GetNextBackBufferIndex()].Get();
}

// Transition resource state
void CRenderTargetSwapChain::StateTransition(D3D12_RESOURCE_STATES state) {
    m_resStateController[CSwapChain::GetMain().GetNextBackBufferIndex()]->StateTransition(state);
}

// Create a swap chain
void CSwapChain::Initialize() {
    // Disable Alt + Enter full screen switching
    CApplication::GetAny().GetDXGIFactory()->MakeWindowAssociation(CApplication::GetAny().GetWndHandle(), DXGI_MWA_NO_ALT_ENTER);

    // Create a swap chain descriptor
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width              = NULL;                            // Automatic
    swapChainDesc.Height             = NULL;                            // Automatic
    swapChainDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;      // RGBA8_UNORM
    swapChainDesc.SampleDesc.Count   = 1;                               // No multisampling
    swapChainDesc.SampleDesc.Quality = 0;                               // Default quality
    swapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT; // How to use swap chain
    swapChainDesc.BufferCount        = SCREEN_BUFFERING_NUM;            // The number of main screen buffer
    swapChainDesc.Stereo             = FALSE;                           // Stereo is not used
    swapChainDesc.Scaling            = DXGI_SCALING_STRETCH;            // Scale to fit output window size
    swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;   // Discard immediately after flip
    swapChainDesc.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;     // Alpha channel processing method is not specified.
    swapChainDesc.Flags              = SWAPCHAIN_FLAGS;                 // Defined flags

    // Create swap chain fullscreen descriptor
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc = {};
    fullScreenDesc.Windowed = TRUE;
    fullScreenDesc.RefreshRate.Denominator = 0;
    fullScreenDesc.RefreshRate.Numerator = 0;
    fullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    fullScreenDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;

    // Create 'swapchain1'
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
    HR_CHECK(L"Create swap chain", 
        CApplication::GetAny().GetDXGIFactory()->CreateSwapChainForHwnd(
        CCommandManager::GetMain().GetCommandQueue(), CApplication::GetAny().GetWndHandle(), &swapChainDesc,
        &fullScreenDesc, nullptr, swapChain1.GetAddressOf()
    ));

    // Cast 'swapchain1' to 'swapchain4'
    HR_CHECK(L"Cast 'swapchain1' to 'swapchain4'", swapChain1->QueryInterface(IID_PPV_ARGS(m_swapChain.GetAddressOf())));

    // Create render target for the back buffers
    m_renderTarget.Create(m_swapChain.Get(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB); // RGBA normalized to 0-1 in the sRGB color space

    // Get next index
    m_nextBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

    // Full screen is false
    m_isFullScreen = false;
}

// Draw and swap the back buffer
void CSwapChain::DrawAndSwap() {
    // Transition state of back buffer
    CSwapChain::GetAny().GetBackBuffer()->StateTransition(D3D12_RESOURCE_STATE_PRESENT);

#ifndef _SEQUENTIAL_GPU_
    // If the GPU wait for the previous frame has not been completed, it waits
    if (m_gpuWaitFuture.Valid()) {
        m_gpuWaitFuture.Get();
    }
#endif // !_SEQUENTIAL_GPU_

    // Execute the commands
    CCommandManager::GetMain().UnsafeGraphicsCommandsExecute();

    // Advance the next index
    m_nextBackBufferIndex = (m_nextBackBufferIndex + 1) % SCREEN_BUFFERING_NUM;

    // Normal wait processsing if no need to rebuild the back buffers
    if (false == m_isRebuildBackBuffers) {
#ifndef _SEQUENTIAL_GPU_
        // Throw the task of waiting for the GPU into a separate thread
        m_gpuWaitFuture = CThreadPool::GetAny().EnqueueTask([this]() { this->WaitForGPU(); });
#else
        WaitForGPU();
#endif // !_SEQUENTIAL_GPU_
    }
    else {
        // If you are rebuilding the back buffer, finish drawing to the current back buffer
        WaitForGPU();

        // Set false
        m_isRebuildBackBuffers = false;

        // Wait until 'Present' function finishes
        CCommandManager::GetAny().WaitForGPU();

        // Get needed information
        const HWND& hwnd = CApplication::GetAny().GetWndHandle();
        int screenSizeX = GetSystemMetrics(SM_CXSCREEN);
        int screenSizeY = GetSystemMetrics(SM_CYSCREEN);
        DWORD windowStyle = NULL;

        // Fullscreen mode
        if (m_isFullScreen) {
            // Borderless window
            windowStyle |= WS_POPUP;
        }
        // Window mode
        else {
            // Default window
            windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

            // Adjust window rectangle
            RECT wndRect = { 0, 0, (LONG)screenSizeX, (LONG)screenSizeY };
            AdjustWindowRect(&wndRect, windowStyle, FALSE);
            screenSizeX = wndRect.right - wndRect.left;
            screenSizeY = wndRect.bottom - wndRect.top;
        }

        // Delete the border and the title bar in the window
        SetWindowLongPtr(hwnd, GWL_STYLE, windowStyle);
        // Relocate the window for borderless
        SetWindowPos(hwnd, NULL, 0, 0, screenSizeX, screenSizeY, SWP_FRAMECHANGED);
        // Restore the window
        ShowWindow(hwnd, SW_RESTORE);

        // Discard all buffers reference
        m_renderTarget.BufferRelease();
        // Resize buffers
        m_swapChain->ResizeBuffers(NULL, NULL, NULL, DXGI_FORMAT_UNKNOWN, SWAPCHAIN_FLAGS);
        // Recreate render target views
        m_renderTarget.CreateRTVs(m_swapChain.Get(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

        // Update back buffer index for next frame
        m_nextBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
    }
}

// Wait for present is called in the another thread
void CSwapChain::WaitForPresent() {
#ifndef _SEQUENTIAL_GPU_
    // If the GPU wait for the previous frame has not been completed, it waits
    if (m_gpuWaitFuture.Valid()) {
        m_gpuWaitFuture.Wait();
    }
#endif // !_SEQUENTIAL_GPU_
}

// Set full screen mode
void CSwapChain::SetFullScreen(bool isFullScreen) {
    // If the window mode is already the same, it does nothing.
    if (m_isFullScreen == isFullScreen) {
        return;
    }

    // Change full screen mode
    m_isFullScreen = isFullScreen;

    // Schedule the back buffers rebuild.
    m_isRebuildBackBuffers = true;
}

// GPU wait processing after drawing
void CSwapChain::WaitForGPU() {
    // Wait for GPU
    CCommandManager::GetAny().WaitForGPU();

    // Clear commands
    CCommandManager::GetAny().UnsafeGraphicsCommandsClear(m_swapChain->GetCurrentBackBufferIndex());

    // Swap
    DXGI_PRESENT_PARAMETERS params = {};
#ifdef _UNLIMITED_REFRESHRATE_
    m_swapChain->Present1(0, DXGI_PRESENT_ALLOW_TEARING, &params);
#else
    m_swapChain->Present1(0, 0, &params);
#endif // _UNLIMITED_REFRESHRATE_
}
