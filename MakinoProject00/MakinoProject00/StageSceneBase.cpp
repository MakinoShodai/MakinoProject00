#include "StageSceneBase.h"
#include "ShaderRegistry.h"
#include "PlayerPrefab.h"
#include "DebugCamera.h"
#include "PlayerCamera.h"
#include "AssetNameDefine.h"

const Colorf CLEAR_COLOR = Colorf(0.0f, 0.0f, 0.0f, 1.0f);

// Starting process
void CStageSceneBase::Start() {
    // Create dsv
    ACRenderTarget* backBuffer = CSwapChain::GetAny().GetBackBuffer();
    m_dsv2D.Create(DXGI_FORMAT_D32_FLOAT, backBuffer->GetWidth(), backBuffer->GetHeight());
    m_dsv3D.Create(DXGI_FORMAT_D32_FLOAT, backBuffer->GetWidth(), backBuffer->GetHeight());

    // Call prefab function of GPSO wrapper for standard layer
    m_standardGpso.Prefab(this);
    m_transparentGpso.Prefab(this);

    // Create gpso2D
    {
        Gpso::GPSOSetting gpsoSetting2D;
        gpsoSetting2D.vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard2DSprite);
        gpsoSetting2D.ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::StandardTex);
        gpsoSetting2D.rtvFormats.push_back(std::make_pair(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Gpso::BlendStateSetting::Alpha));
        gpsoSetting2D.rasterizerState.cullMode = D3D12_CULL_MODE_BACK;
        gpsoSetting2D.depth.type = Gpso::DepthTestType::ReadWrite;
        m_uiGpso.Create(this, L"UI GPSO", gpsoSetting2D, { GraphicsLayer::UI }, { GraphicsComponentType::SpriteUI });
    }

    // Provisional terrain
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, -0.5f, 0.0f), Vector3f(100.0f, 1.0f, 100.0f)));
        obj->AddComponent<CBoxCollider3D>();
        obj->AddComponent<CTexShape>(GraphicsLayer::Standard, ShapeKind::Box, TexName::TEST);
    }

    // Create prefabs
    CreateGameObject<CPlayerPrefab>(Transformf(Vector3f(0.0f, 0.0f, 0.0f)));
    CreateGameObject<CPlayerCameraPrefab>(Transformf(Vector3f(0.0f, 0.0f, -5.0f)));

    CreateGameObject<CDebugCameraPrefab>();
}

// Drawing process
void CStageSceneBase::Draw() {
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
    // Command function of GPSO wrapper for transparent layer
    m_transparentGpso.SetCommand();

    // Apply 2D depth stencil view
    rtv->Apply(&m_dsv2D);

    // Command function of GPSO for UI
    m_uiGpso.SetCommand();
}
