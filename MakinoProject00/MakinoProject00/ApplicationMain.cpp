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

#include "GameObject.h"
#include "Component.h"
#include "Scene.h"
#include "LayeredGPSOWrapper.h"

const UINT WINDOW_WIDTH = 1920;
const UINT WINDOW_HEIGHT = 1080;

class RotateComponent : public ACComponent {
public:
    RotateComponent(CGameObject* obj, Vector3f axis, float speed) : ACComponent(obj), m_axis(axis), m_speed(speed) {}

    void Start() {

    }

    void Update() {
        Quaternionf rotate = m_gameObj->GetTransform().rotation;
        rotate.AngleAxis(m_speed * (float)CAppClock::GetMain().GetDeltaTime(), m_axis);
        m_gameObj->SetRotation(rotate);
    }
private:
    Vector3f m_axis;
    float m_speed;
};

class LerpTestComponent : public ACComponent {
public:
    LerpTestComponent(CGameObject* obj, Quaternionf a, Quaternionf b) : ACComponent(obj), m_start(a), m_end(b) {}

    void Start() { m_t = 0.0f; m_sign = 1.0f; m_startV = m_gameObj->GetTransform().pos; m_endV = m_startV + Vector3f(0.0f, 3.0f, 0.0f); }

    void Update() {
        m_t += (float)CAppClock::GetMain().GetDeltaTime() * 0.5f * m_sign;
        if (m_t > 1.0f) {
            m_sign = -m_sign;
            m_t = 1.0f;
        }
        else if (m_t < 0.0f) {
            m_sign = -m_sign;
            m_t = 0.0f;
        }

        m_gameObj->SetPos(Utl::Math::Lerp(m_startV, m_endV, m_t));
        m_gameObj->SetRotation(Utl::Math::Slerp(m_start, m_end, m_t));
    }

private:
    float m_t;
    float m_sign;
    Quaternionf m_start;
    Quaternionf m_end;
    Vector3f m_startV;
    Vector3f m_endV;
};

class CAnimComponent : public ACComponent {
public:
    using ACComponent::ACComponent;

    void Start() {
        m_model = m_gameObj->GetComponent<CSkeletalModel>();

        CDynamicModelController* modelData = m_model->GetController();

        m_animNo = 0;

        modelData->Play(m_animNo, true);
        //m_model->GetUniqueData()->animes[0].speed = 0.25f;
    }

    void Update() {
        if (CInputSystem::GetMain().IsKeyDown('K')) {
            CDynamicModelController* modelData = m_model->GetController();

            m_animNo = (m_animNo + 1) % 2;
            modelData->Play(m_animNo, true);
        }

        if (CInputSystem::GetMain().IsKeyDown('N')) {
            CDynamicModelController* modelData = m_model->GetController();
            modelData->BindPose();
        }
    }

private:
    CWeakPtr<CSkeletalModel> m_model;
    ModelInfo::AnimID m_animNo;
};

class CTestScene : public ACScene {
public:
    void Start() {
        // Create dsv
        ACRenderTarget* backBuffer = CSwapChain::GetAny().GetBackBuffer();
        m_dsv2D.Create(DXGI_FORMAT_D32_FLOAT, backBuffer->GetWidth(), backBuffer->GetHeight());
        m_dsv3D.Create(DXGI_FORMAT_D32_FLOAT, backBuffer->GetWidth(), backBuffer->GetHeight());

        // Create gpso3DModel
        {
            LayeredGPSOSetting standardSetting({ GraphicsComponentType::BasicModel, GraphicsComponentType::TexShape });
            standardSetting.defaultSetting.vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard3D);
            standardSetting.defaultSetting.ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::StandardTex);
            standardSetting.defaultSetting.rtvFormats.push_back(std::make_pair(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Gpso::BlendStateSetting::Alpha));
            standardSetting.defaultSetting.rasterizerState.cullMode = D3D12_CULL_MODE_BACK;
            standardSetting.defaultSetting.depth.type = Gpso::DepthTestType::ReadWrite;
            {
                auto setting = standardSetting.AddOverrideSetting({ GraphicsComponentType::SkeletalModel });
                setting->vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard3DAnim);
            }
            {
                auto setting = standardSetting.AddOverrideSetting({ GraphicsComponentType::ColorShape });
                setting->ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::StandardColor);
            }
            {
                auto setting = standardSetting.AddOverrideSetting({ GraphicsComponentType::Sprite3D });
                setting->vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard3DSprite);
                setting->rasterizerState = Gpso::RasterizerStateSetting();
                setting->rasterizerState->cullMode = D3D12_CULL_MODE_NONE;
            }
            m_standardGpso.Create(this, L"Standard GPSO", standardSetting, { GraphicsLayer::Standard });
        }

        // Create gpso2D
        {
            Gpso::GPSOSetting gpsoSetting2D;
            gpsoSetting2D.vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard2DSprite);
            gpsoSetting2D.ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::StandardTex);
            gpsoSetting2D.rtvFormats.push_back(std::make_pair(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Gpso::BlendStateSetting::Alpha));
            gpsoSetting2D.rasterizerState.cullMode = D3D12_CULL_MODE_BACK;
            gpsoSetting2D.depth.type = Gpso::DepthTestType::ReadWrite;
            m_gpso2D.Create(this, L"UI GPSO", gpsoSetting2D, { GraphicsLayer::UI }, { GraphicsComponentType::SpriteUI });
        }

        // UI obj
        auto ui1 = CreateGameObject<CGameObject>(Transformf(Vector3f(80.0f, -50.0f, 0.0f), Vector3f::Ones(), Utl::DEG_2_RAD * Vector3f(0.0f, 0.0f, 0.0f)));
        ui1->AddComponent<CSpriteUI>(GraphicsLayer::UI, L"Texture/textest200x200.png");
        ui1->AddComponent<RotateComponent>(Vector3f(0.0f, 0.0f, 1.0f).GetNormalize(), 1.5f);
        // UI obj
        auto ui2 = CreateGameObject<CGameObject>(Transformf(Vector3f(70.0f, -40.0f, 0.5f), Vector3f::Ones(), Utl::DEG_2_RAD * Vector3f(0.0f, 0.0f, 0.0f)));
        ui2->AddComponent<CSpriteUI>(GraphicsLayer::UI, L"Texture/textest200x200.png");
        ui2->AddComponent<RotateComponent>(Vector3f(0.0f, 0.0f, 1.0f).GetNormalize(), 1.0f);

        // Sprite obj
        auto sprite1 = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, 0.0f, 0.0f), Vector3f::Ones(), Utl::DEG_2_RAD * Vector3f(0.0f, 20.0f, 0.0f)));
        sprite1->AddComponent<CSprite3D>(GraphicsLayer::Standard, L"Texture/apollo11.png");
        sprite1->AddComponent<RotateComponent>(Vector3f(1.0f, 0.0f, 0.0f).GetNormalize(), 1.0f);
        //
        auto sprite2 = CreateGameObject<CGameObject>(Transformf(Vector3f(0.5f, 0.0f, 0.0f), Vector3f::Ones(), Utl::DEG_2_RAD * Vector3f(45.0f, 0.0f, 0.0f)));
        sprite2->AddComponent<CSprite3D>(GraphicsLayer::Standard, L"Texture/textest200x200.png");
        sprite2->AddComponent<RotateComponent>(Vector3f(0.0f, 1.0f, 0.0f).GetNormalize(), 3.0f);

        // Box
        {
            auto box = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, 0.0f, 0.0f), Vector3f::Ones(), Vector3f::Zero()));
            box->AddComponent<CTexShape>(GraphicsLayer::Standard, ShapeKind::Box, L"Texture/textest200x200.png");
            box->AddComponent<RotateComponent>(Vector3f(1.0f, 1.0f, 0.0f).GetNormalize(), 1.0f);
        }

        // Capsule
        {
            auto capsule = CreateGameObject<CGameObject>(Transformf(Vector3f(5.0f, 0.0f, 0.0f), Vector3f::Ones(), Vector3f::Zero()));
            capsule->AddComponent<CColorOnlyShape>(GraphicsLayer::Standard, ShapeKind::Capsule, Colorf(1.0f, 0.0f, 0.0f, 1.0f));
            capsule->AddComponent<RotateComponent>(Vector3f(1.0f, 1.0f, 0.0f).GetNormalize(), 1.0f);
        }

        auto desk = CreateGameObject<CGameObject>(Transformf(Vector3f(-2.0f, 0.0f, 0.0f), Vector3f::Ones(), Vector3f::Zero()));
        desk->AddComponent<CBasicModel>(GraphicsLayer::Standard, L"desk/desk.fbx");
        desk->AddComponent<RotateComponent>(Vector3f(0.0f, 1.0f, 0.0f).GetNormalize(), 3.0f);
        
        for (int i = 0; i < 20; ++i) {
            auto uwan = CreateGameObject<CGameObject>(Transformf(Vector3f((float)i - 10.0f, -1.0f, 5.0f), Vector3f::Ones(), Utl::DEG_2_RAD * Vector3f(0.0f, (float)i * 30.0f, 0.0f)));
            uwan->AddComponent<CSkeletalModel>(GraphicsLayer::Standard, L"unitychan/unitychan.fbx");
            uwan->AddComponent<CAnimComponent>();
            uwan->AddComponent<RotateComponent>(Vector3f(0.0f, 1.0f, 0.0f).GetNormalize(), 3.0f);
            //uwan->AddComponent<LerpTestComponent>(
            //    Quaternionf(Utl::DEG_2_RAD * Vector3f(0.0f, 0.0f, 0.0f)),
            //    Quaternionf(Utl::DEG_2_RAD * Vector3f((float)i * 30, 180.0f, 0.0f)));
        }

        // Camera obj
        auto camera = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, 0.0f, -5.0f)));
        camera->AddComponent<CCameraComponent>(L"Main camera");
    }

    void Draw() {
        ACRenderTarget* rtv = CSwapChain::GetAny().GetBackBuffer();

        m_dsv3D.Clear();
        m_dsv2D.Clear();

        Colorf clearColor(0.4f, 0.4f, 0.8f, 1.0f);
        rtv->Clear(clearColor);
        rtv->Apply(&m_dsv3D);

        m_standardGpso.SetCommand();

        rtv->Apply(&m_dsv2D);

        m_gpso2D.SetCommand();
    }
private:
    CLayeredGPSOWrapper m_standardGpso;
    CGraphicsPipelineState m_gpso2D;
    CDepthStencil m_dsv3D;
    CDepthStencil m_dsv2D;
};

class CTestScene2 : public ACScene {
public:
    void Start() {
        // Create gpso
        Gpso::GPSOSetting gpsoSetting;
        gpsoSetting.vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard2DSprite);
        gpsoSetting.ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::StandardTex);
        gpsoSetting.rtvFormats.push_back(std::make_pair(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Gpso::BlendStateSetting::Alpha));
        gpsoSetting.rasterizerState.cullMode = D3D12_CULL_MODE_NONE;
        m_gpso.Create(this, L"All objects GPSO", gpsoSetting, { GraphicsLayer::UI }, { GraphicsComponentType::SpriteUI });

        // Sprite obj
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(-50.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.0f, 1.0f), Vector3f(Utl::DEG_2_RAD * 0.0f, Utl::DEG_2_RAD * 0.0f, Utl::DEG_2_RAD * 0.0f)));
        obj->AddComponent<CSpriteUI>(GraphicsLayer::UI, L"Texture/textest200x200.png");

        // Camera obj
        auto camera = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, 0.0f, 5.0f)));
        camera->AddComponent<CCameraComponent>(L"Main camera");
    }

    void Draw() {
        ACRenderTarget* rtv = CSwapChain::GetAny().GetBackBuffer();

        Colorf clearColor(0.2f, 0.2f, 0.2f, 1.0f);
        rtv->Clear(clearColor);
        rtv->Apply(nullptr);

        m_gpso.SetCommand();
    }
private:
    CGraphicsPipelineState m_gpso;
};

bool g_isScene2 = false;
CUniquePtrWeakable<CGraphicsObjectAsset> asset;

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
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
        case VK_SPACE:
            if (!g_isScene2) {
                CSceneRegistry::GetMain().SetNextScene<CTestScene2>();
                g_isScene2 = true;
            }
            else {
                CSceneRegistry::GetMain().SetNextScene<CTestScene>();
                g_isScene2 = false;
            }
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
    // Initialize the thread pool
    CThreadPool::GetMain().Initialize((std::max)((unsigned int)1, std::thread::hardware_concurrency()));

    // Create a window
    CApplication::GetMain().InitializeWnd((WNDPROC)WindowProcedure, WINDOW_WIDTH, WINDOW_HEIGHT, L"DirectX12 Project");

#ifdef _DEBUG
    // Enable DirectX debug layer
    Dbg::EnableDirectXDebugLayer();
#endif // _DEBUG

#ifdef _DEBUG
    // Load dll of Pix
    Dbg::LoadMicrosoftPixDLL();
#endif // _DEBUG

    // Initialize directX
    CApplication::GetMain().InitializeDirectX();
    
    // Initialize the application clock
    CAppClock::GetMain().Initialize();
    
    // Initialize the command manager
    CCommandManager::GetMain().Initialize();
    
    // Initialize the swap chain
    CSwapChain::GetMain().Initialize();
    
    // Initialize the shader registry
    CShaderRegistry::GetMain().Initialize();
    
    // Initialize the registry for constant buffers
    CDynamicCbRegistry::GetMain().Initialize();
    
    // Initialize descriptor heap pool
    CDescriptorHeapPool::GetMain().Initialize(2000, 1);
    
    // Initialize the registry for static resources
    CStaticResourceRegistry::GetMain().Initialize();
    
    // Initialize the sprite class
    CSpriteUI::Initialize();

    // Initialize the static shape mesh class
    CShapeMesh::Initialize();
    
    CTextureRegistry::GetMain().LoadPak(L"Assets/textures");

    CModelRegistry::GetMain().AddModelLoadDesc(L"unitychan/unitychan.fbx", ModelInfo::Load::ModelDesc(0.025f, ModelInfo::Load::CoordinateSystem::Standard, 1.0f));
    CModelRegistry::GetMain().AddModelLoadDesc(L"desk/desk.fbx", ModelInfo::Load::ModelDesc(0.025f, ModelInfo::Load::CoordinateSystem::Blender));

    CModelRegistry::GetMain().AddAnimLoadDesc(L"unitychan/jump.fbx", ModelInfo::Load::AnimDesc(1.0f));
    CModelRegistry::GetMain().AddAnimLoadDesc(L"unitychan/run.fbx", ModelInfo::Load::AnimDesc(1.0f, true));

    CModelRegistry::GetMain().AddAnimToAnimSetting(L"unitychan/jump.fbx", L"unitychan/run.fbx", ModelInfo::InterpolationSetting(5.0f));
    CModelRegistry::GetMain().AddAnimToAnimSetting(L"unitychan/run.fbx", L"unitychan/jump.fbx", ModelInfo::InterpolationSetting(0.3f));

    CModelRegistry::GetMain().LoadModelPak(L"Assets/models");
    CModelRegistry::GetMain().LoadAnimPak(L"Assets/anims");

    CSceneRegistry::GetMain().Initialize<CTestScene>();

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
