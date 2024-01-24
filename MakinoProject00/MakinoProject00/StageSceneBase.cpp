#include "StageSceneBase.h"
#include "ShaderRegistry.h"
#include "PlayerPrefab.h"
#include "DebugCamera.h"
#include "PlayerCamera.h"
#include "AssetNameDefine.h"
#include "DirectionalLight.h"
#include "StaticResourceRegistry.h"
#include "PointLight.h"

const Colorf SCREEN_CLEAR_COLOR = Colorf(0.0f, 0.0f, 0.0f, 1.0f);

// Starting process
void CStageSceneBase::Start() {
    // Get static cb allocator
    m_staticCbLightVP = CStaticResourceRegistry::GetAny().GetStaticResource<CStaticCbLightVP>();
    m_staticShadowMapSrv[0] = CStaticResourceRegistry::GetAny().GetStaticResource<CStaticSrvShadowMap1>();
    m_staticShadowMapSrv[1] = CStaticResourceRegistry::GetAny().GetStaticResource<CStaticSrvShadowMap2>();
    m_staticShadowMapSrv[2] = CStaticResourceRegistry::GetAny().GetStaticResource<CStaticSrvShadowMap3>();

    // Create dsv
    ACRenderTarget* backBuffer = CSwapChain::GetAny().GetBackBuffer();
    m_dsv2D.Create(DXGI_FORMAT_D32_FLOAT, backBuffer->GetWidth(), backBuffer->GetHeight());
    m_dsv3D.Create(DXGI_FORMAT_D32_FLOAT, backBuffer->GetWidth(), backBuffer->GetHeight());

    // Create rtv and dsv for shadow map
    for (UINT i = 0; i < CASCADE_NUM; ++i) {
        m_shadowMapDsvs[i].Create(DXGI_FORMAT_D32_FLOAT, SHADOWMAP_SIZE, SHADOWMAP_SIZE);
    }

    // Call prefab function of GPSO wrapper for standard layer
    m_standardGpso.Prefab(this);
    m_transparentGpso.Prefab(this);
    m_writeShadowGpso.Prefab(this);
    m_shadingGpso.Prefab(this);

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
        obj->AddComponent<CColorOnlyShape>(GraphicsLayer::ReadWriteShading, ShapeKind::Box, Colorf(0.1f, 0.1f, 0.1f, 1.0f));
    }

    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, 10.0f, 0.0f), Vector3f(10.0f, 1.0f, 10.0f)));
        obj->AddComponent<CColorOnlyShape>(GraphicsLayer::ReadWriteShading, ShapeKind::Box, Colorf(1.0f, 1.0f, 1.0f, 1.0f));
    }

    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, 1.0f, 0.0f), Vector3f(1.0f, 1.0f, 1.0f)));
        obj->AddComponent<CPointLightComponent>(Colorf(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // Box
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(30.0f, 10.0f, 0.0f), Vector3f::Ones() * 10.0f, Utl::DEG_2_RAD * Vector3f(0.0f, 0.0f, 0.0f)));
        obj->AddComponent<CSphereCollider3D>();
        obj->AddComponent<CColorOnlyShape>(GraphicsLayer::ReadWriteShading, ShapeKind::Sphere, Colorf(1.0f, 1.0f, 1.0f, 1.0f));
        auto rb = obj->AddComponent<CRigidBody>();
        rb->SetBodyType(RigidBodyType::Dynamic);
        rb->SetMaterial(RigidBodyMaterial(0.5f, 1.0f));
    }

    // Create prefabs
    CreateGameObject<CDirectionalLightPrefab>(Transformf(Vector3f(0.0f, 10.0f, 0.0f)));
    CreateGameObject<CPlayerPrefab>(Transformf(Vector3f(0.0f, 0.0f, 0.0f)));
    CreateGameObject<CPlayerCameraPrefab>(Transformf(Vector3f(0.0f, 0.0f, -5.0f)));

    CreateGameObject<CDebugCameraPrefab>();
}

// Drawing process
void CStageSceneBase::Draw() {
    // Clear depth stencil views
    m_dsv3D.Clear();
    m_dsv2D.Clear();

    // Write shadow maps
    for (UINT i = 0; i < CASCADE_NUM; ++i) {
        // Clear DSV for shadow map
        m_shadowMapDsvs[i].Clear();
        // Apply DSV for shadow map
        m_shadowMapDsvs[i].Apply();

        // Command function of GPSO wrapper for writing shadow
        m_writeShadowGpso.SetCommand();

        // Advance static constant buffer allocator for light view projection matrix
        m_staticCbLightVP->Advance();
    }

    // Allocate shadow maps to srv
    for (UINT i = 0; i < CASCADE_NUM; ++i) {
        m_staticShadowMapSrv[i]->Allocate(&m_shadowMapDsvs[i], false);
    }

    // Get current back buffer and clear it
    ACRenderTarget* rtv = CSwapChain::GetAny().GetBackBuffer();
    rtv->Clear(SCREEN_CLEAR_COLOR);

    // Apply 3D depth stencil view
    rtv->Apply(&m_dsv3D);

    // Command function of GPSO wrapper for Shading
    m_shadingGpso.SetCommand();
    // Command function of GPSO wrapper for standard layer
    m_standardGpso.SetCommand();
    // Command function of GPSO wrapper for transparent layer
    //m_transparentGpso.SetCommand();

    // Apply 2D depth stencil view
    rtv->Apply(&m_dsv2D);

    // Command function of GPSO for UI
    m_uiGpso.SetCommand();
}
