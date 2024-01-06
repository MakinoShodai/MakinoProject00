﻿#include "Application.h"
#include "UtilityForException.h"
#include "Resource/Resource.h"

// Width of ideal window size
const UINT IDEAL_WINDOW_WIDTH = 1920;
// Height of ideal window size
const UINT IDEAL_WINDOW_HEIGHT = 1080;

// Create and display a window
void CApplication::InitializeWnd(WNDPROC wndProc, UINT width, UINT height, const std::wstring& name) {
    // Calculate magnification of current window size relative to ideal window size
    m_wndSizeMagnification = Vector2f((float)width / (float)IDEAL_WINDOW_WIDTH, (float)height / (float)IDEAL_WINDOW_HEIGHT);

    // Set the window class information
    m_wndSetting.cbSize        = sizeof(WNDCLASSEX);                                             // Struct size
    m_wndSetting.style         = CS_HREDRAW | CS_VREDRAW;                                        // If the window size changes, redraw
    m_wndSetting.lpfnWndProc   = wndProc;                                                        // Window procedure function
    m_wndSetting.cbClsExtra    = 0;                                                              // Additional amount of memory. Set to 0
    m_wndSetting.cbWndExtra    = 0;                                                              // Additional amount of memory. Set to 0
    m_wndSetting.hInstance     = GetModuleHandle(NULL);                                          // Application instance handle
    m_wndSetting.hIcon         = LoadIcon(m_wndSetting.hInstance, MAKEINTRESOURCE(IDI_ICON1));   // Application icon
    m_wndSetting.hIconSm       = LoadIcon(m_wndSetting.hInstance, MAKEINTRESOURCE(IDI_ICON1));   // Application icon (small)
    m_wndSetting.hbrBackground = NULL;                                                           // The game doesn't need it
    m_wndSetting.hCursor       = LoadCursor(NULL, IDC_ARROW);                                    // System default cursor
    m_wndSetting.lpszMenuName  = NULL;                                                           // Window does'nt have a menu
    m_wndSetting.lpszClassName = name.c_str();                                                   // Setup class name

    // Register the window class in the OS
    RegisterClassEx(&m_wndSetting);

    // Calculate the window size by the client area size
    DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT wndRect = { 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&wndRect, windowStyle, FALSE);

    // Create and show window
    m_wndHandle = CreateWindow(
        m_wndSetting.lpszClassName,         // Setup application class name
        name.c_str(),                       // Text in Title bar
        windowStyle,                        // Window that has title bar and a border
        0,                                  // Position is set in the upper left corner of the screen
        0,                                  // Position is set in the upper left corner of the screen
        wndRect.right - wndRect.left,       // Window width
        wndRect.bottom - wndRect.top,       // Window height
        nullptr,                            // Parent window doesn't exist
        nullptr,                            // Menu does'nt exist
        m_wndSetting.hInstance,             // Application instance handle
        nullptr                             // No additional parameters are set
    );
    ShowWindow(m_wndHandle, SW_SHOW);
}

// Destroy window instance
void CApplication::DestroyWnd() {
    // Early return
    if (m_wndHandle == NULL) { return; }

    // Window closing process
    UnregisterClass(m_wndSetting.lpszClassName, m_wndSetting.hInstance);

    // Initialize window handle
    m_wndHandle = NULL;
}

// Initialize DirectX such as DirectX device
void CApplication::InitializeDirectX() {
    // Prepare DXGI flag
    UINT dxgiFlag = NULL;
#ifdef _DEBUG
    dxgiFlag |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG

    // Create DXGI factory object
    HR_CHECK(L"Create DXGI factory object", CreateDXGIFactory2(dxgiFlag, IID_PPV_ARGS(m_dxgiFactory.GetAddressOf())));

    // Get a highest performance GPU
    Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter;
    HR_CHECK(_T("Get GPU information on the machine"), m_dxgiFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(dxgiAdapter.GetAddressOf())));

    // feature level array
    D3D_FEATURE_LEVEL level[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Repeat until a successful feature level is reached
    D3D_FEATURE_LEVEL featureLevel;
    for (auto lev : level) {
        HRESULT hr = D3D12CreateDevice(dxgiAdapter.Get(), lev, IID_PPV_ARGS(m_dxDevice.GetAddressOf()));
        if (SUCCEEDED(hr)) {
            featureLevel = lev;
            break;
        }
    }
    // If the initialization of all feature levels fails, perform error handling
    if (m_dxDevice == nullptr) {
        throw Utl::Error::CFatalError(L"Failed! \"Create DirectX12 device\"");
    }
}

// Process to be called at instance destruction
void CApplication::OnDestroy() {
    // Destroy window
    DestroyWnd();
}
