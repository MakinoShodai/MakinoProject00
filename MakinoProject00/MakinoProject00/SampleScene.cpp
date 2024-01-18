#ifdef _SAMPLE
#include "SampleScene.h"
#include "SampleComponent.h"
#include "ShaderRegistry.h"
#include "AssetNameDefine.h"
#include "FreeCamera.h"

// Clear color for screen render target
const Colorf CLEAR_COLOR = Colorf(0.4f, 0.4f, 0.8f, 1.0f);

// Starting process
void CSampleScene::Start() {
    // Create dsv
    ACRenderTarget* backBuffer = CSwapChain::GetAny().GetBackBuffer();
    m_dsv2D.Create(DXGI_FORMAT_D32_FLOAT, backBuffer->GetWidth(), backBuffer->GetHeight());
    m_dsv3D.Create(DXGI_FORMAT_D32_FLOAT, backBuffer->GetWidth(), backBuffer->GetHeight());

    // Call prefab function of GPSO wrapper for standard layer
    m_standardGpso.Prefab(this);

    // Create gpso2D
    {
        Gpso::GPSOSetting gpsoSetting2D;
        gpsoSetting2D.vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard2DSprite);
        gpsoSetting2D.ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::StandardTex);
        gpsoSetting2D.rtvFormats.push_back(std::make_pair(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Gpso::BlendStateSetting::Alpha));
        gpsoSetting2D.rasterizerState.cullMode = D3D12_CULL_MODE_BACK;
        gpsoSetting2D.depth.type = Gpso::DepthTestType::ReadWrite;
        m_gpsoUI.Create(this, L"UI GPSO", gpsoSetting2D, { GraphicsLayer::UI }, { GraphicsComponentType::SpriteUI });
    }

    // UI obj
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(80.0f, -50.0f, 0.0f), Vector3f::Ones(), Utl::DEG_2_RAD * Vector3f(0.0f, 0.0f, 0.0f)));
        obj->AddComponent<CSpriteUI>(GraphicsLayer::UI, TEX_NAME_TEST);
        obj->AddComponent<CSampleRotateComponent>(Vector3f(0.0f, 0.0f, 1.0f).GetNormalize(), 1.5f);
    }
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(70.0f, -40.0f, 0.5f), Vector3f::Ones(), Utl::DEG_2_RAD * Vector3f(0.0f, 0.0f, 0.0f)));
        obj->AddComponent<CSpriteUI>(GraphicsLayer::UI, TEX_NAME_TEST);
        obj->AddComponent<CSampleRotateComponent>(Vector3f(0.0f, 0.0f, 1.0f).GetNormalize(), 1.0f);
    }

    // Sprite obj
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, 0.0f, 0.0f), Vector3f::Ones(), Utl::DEG_2_RAD * Vector3f(0.0f, 20.0f, 0.0f)));
        obj->AddComponent<CSprite3D>(GraphicsLayer::Standard, TEX_NAME_TEST2);
        obj->AddComponent<CSampleRotateComponent>(Vector3f(1.0f, 0.0f, 0.0f).GetNormalize(), 1.0f);
    }
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(0.5f, 0.0f, 0.0f), Vector3f::Ones(), Utl::DEG_2_RAD * Vector3f(45.0f, 0.0f, 0.0f)));
        obj->AddComponent<CSprite3D>(GraphicsLayer::Standard, TEX_NAME_TEST);
        obj->AddComponent<CSampleRotateComponent>(Vector3f(0.0f, 1.0f, 0.0f).GetNormalize(), 3.0f);
    }

    // Billboard obj
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, -1.5f, 0.0f), Vector3f::Ones(), Utl::DEG_2_RAD * Vector3f(0.0f, 0.0f, 0.0f)));
        obj->AddComponent<CBillboard>(GraphicsLayer::Standard, TEX_NAME_TEST2);
    }

    // Box
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, 0.0f, 0.0f), Vector3f::Ones(), Vector3f::Zero()));
        obj->AddComponent<CTexShape>(GraphicsLayer::Standard, ShapeKind::Box, TEX_NAME_TEST);
        obj->AddComponent<CSampleRotateComponent>(Vector3f(1.0f, 1.0f, 0.0f).GetNormalize(), 1.0f);
    }

    // Capsule
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(5.0f, 0.0f, 0.0f), Vector3f::Ones(), Vector3f::Zero()));
        obj->AddComponent<CColorOnlyShape>(GraphicsLayer::Standard, ShapeKind::Capsule, Colorf(1.0f, 0.0f, 0.0f, 1.0f));
        obj->AddComponent<CSampleRotateComponent>(Vector3f(1.0f, 1.0f, 0.0f).GetNormalize(), 1.0f);
    }

    // Desk
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(-2.0f, 0.0f, 0.0f), Vector3f::Ones(), Vector3f::Zero()));
        obj->AddComponent<CBasicModel>(GraphicsLayer::Standard, MODEL_NAME_DESK);
        obj->AddComponent<CSampleRotateComponent>(Vector3f(0.0f, 1.0f, 0.0f).GetNormalize(), 3.0f);
    }

    // Skeletal models
    for (int i = 0; i < 20; ++i) {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f((float)i * 2 + 3.0f, -1.0f, 5.0f), Vector3f::Ones(), Utl::DEG_2_RAD * Vector3f(0.0f, (float)i * 30.0f, 0.0f)));
        obj->AddComponent<CSkeletalModel>(GraphicsLayer::Standard, MODEL_NAME_CUTEBIRD);
        obj->AddComponent<CSampleAnimComponent>();
        obj->AddComponent<CSampleRotateComponent>(Vector3f(0.0f, 1.0f, 0.0f).GetNormalize(), 3.0f);
    }

    // Main camera obj
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, 0.0f, -5.0f)));
        obj->AddComponent<CCameraComponent>(L"Main camera");
        obj->AddComponent<CFreeCameraControl>();
    }
}

// Drawing process
void CSampleScene::Draw() {
    // Clear depth stencil views
    m_dsv3D.Clear();
    m_dsv2D.Clear();

    // Get current back buffer and clear it
    ACRenderTarget* rtv = CSwapChain::GetAny().GetBackBuffer();
    rtv->Clear(CLEAR_COLOR);

    // Apply 3D depth stencil view
    rtv->Apply(&m_dsv3D);

    // Command function of GPSO wrapper for standard layer
    m_standardGpso.SetCommand();

    // Apply 2D depth stencil view
    rtv->Apply(&m_dsv2D);

    // Command function of GPSO for UI
    m_gpsoUI.SetCommand();
}

#endif // _SAMPLE