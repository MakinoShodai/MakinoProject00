#include "ApplicationMain.h"
#include "ApplicationError.h"
#include "InputSystem.h"
#include "ThreadPool.h"
#include "Application.h"
#include "ApplicationClock.h"
#include "DebugForApplicationFramework.h"
#include "GraphicsPipelineState.h"
#include "CommandManager.h"
#include "SwapChain.h"
#include "TextureRegistry.h"
#include "DynamicCbRegistry.h"
#include "GraphicsLayerRegistry.h"
#include "StaticResourceRegistry.h"
#include "SceneRegistry.h"
#include "ShaderRegistry.h"
#include "DescriptorHeapPool.h"
#include "ModelRegistry.h"
#include "ModelLoadSetting.h"
#include "TagSetting.h"
#include "EncryptAssetMain.h"
#include "ImguiHelper.h"

// Width of window size
const UINT WINDOW_WIDTH = 1920;
// Height of window size
const UINT WINDOW_HEIGHT = 1080;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Window procedure
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
        return true;

    switch (msg) {
    case WM_KEYDOWN:
        switch (wparam) {
        // Change full screen mode
        case VK_F11:
            CSwapChain::GetMain().SetFullScreen(!CSwapChain::GetAny().IsFullScreen());
            break;
        // Exit the application
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
            break;
        }
        break;
        // Resume application processing
    case WM_SETFOCUS:
    case WM_EXITSIZEMOVE:
        CAppClock::GetMain().SleepApplication(false);
        break;
        // Pause application processing
    case WM_KILLFOCUS:
    case WM_ENTERSIZEMOVE:
        CAppClock::GetMain().SleepApplication(true);
        break;
        // Destroy message
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        break;
    default:
        break;
    }

    // Perform default processing
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

// Main funciton for application
void ApplicationMain() {
#ifdef _DEBUG
    // Enable DirectX debug layer
    Dbg::EnableDirectXDebugLayer();
#endif // _DEBUG

#ifdef _DEBUG
    // Load dll of Pix
    Dbg::LoadMicrosoftPixDLL();
#endif // _DEBUG

    // Initialize the thread pool
    CThreadPool::GetMain().Initialize((std::max)((unsigned int)1, std::thread::hardware_concurrency()));

    // Create a window
    CApplication::GetMain().InitializeWnd((WNDPROC)WindowProcedure, WINDOW_WIDTH, WINDOW_HEIGHT, L"Physics x Async Render");

    // Initialize directX
    CApplication::GetMain().InitializeDirectX();

    // Initialize imgui
    CImguiHelper::GetMain().Initialize();
    
    // Initialize the application clock
    CAppClock::GetMain().Initialize(999.0);
    
    // Initialize the command manager
    CCommandManager::GetMain().Initialize();
    
    // Initialize the swap chain
    CSwapChain::GetMain().Initialize();
    
    // Initialize the shader registry
    CShaderRegistry::GetMain().Initialize();
    
    // Initialize the registry for constant buffers
    CDynamicCbRegistry::GetMain().Initialize();
    
    // Initialize descriptor heap pool
    CDescriptorHeapPool::GetMain().Initialize(5000, 1);
    
    // Initialize the registry for static resources
    CStaticResourceRegistry::GetMain().Initialize();
    
    // Initialize the sprite class
    CSpriteUI::Initialize();

    // Initialize the static shape mesh class
    CShapeMesh::Initialize();

    // Load textures
    CTextureRegistry::GetMain().LoadPak(PAK_FILE_TEXTURES);

    // Perform setting and load models and animations
    ModelLoadSetting();
    CModelRegistry::GetMain().LoadModelPak(PAK_FILE_MODELS);
    CModelRegistry::GetMain().LoadAnimPak(PAK_FILE_ANIMATIONS);

    // Perform setting for collider tag
    TagSetting();

    // Initialize scene
    CSceneRegistry::GetMain().Initialize();

    // Main loop
    MSG msg = {};
    while (true) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // message becomes WM_QUIT when the application is finished
        if (msg.message == WM_QUIT) {
            break;
        }

        // FPS limitation
        if (false == CAppClock::GetMain().ManageFrameRate()) {
            continue;
        }

        // Check application exit
        CApplicationError::GetAny().Check();

        // Update keyboard input information
        CInputSystem::GetMain().Update();

        // Scene processing per frame
        CSceneRegistry::GetMain().ProcessPerFrame();
    }
}
