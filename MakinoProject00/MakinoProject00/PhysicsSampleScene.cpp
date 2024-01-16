#ifdef _FOR_PHYSICS
#include "PhysicsSampleScene.h"
#include "PhysicsSampleComponent.h"
#include "BoxCollider3D.h"
#include "SphereCollider3D.h"
#include "RigidBody.h"
#include "InputSystem.h"
#include "FreeCamera.h"

// Clear color for screen render target
const Colorf CLEAR_COLOR = Colorf(0.4f, 0.4f, 0.8f, 1.0f);

// Starting process
void CPhysicsSampleScene::Start() {
    // Create dsv
    ACRenderTarget* backBuffer = CSwapChain::GetAny().GetBackBuffer();
    m_dsv3D.Create(DXGI_FORMAT_D32_FLOAT, backBuffer->GetWidth(), backBuffer->GetHeight());

    // Call prefab function of GPSO wrapper
    m_standardGpso.Prefab(this);
    m_transparentGpso.Prefab(this);

    // Box
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, 10.0f, 0.0f), Vector3f::Ones() * 1.0f, Utl::DEG_2_RAD * Vector3f(0.0f, 0.0f, 0.0f)));
        obj->AddComponent<CCapsuleCollider3D>(5.0f);
        obj->AddComponent<CSampleOnCollisionComponent>();
        auto rb = obj->AddComponent<CRigidBody>();
        rb->SetBodyType(RigidBodyType::Dynamic);
        rb->SetMaterial(RigidBodyMaterial(0.5f, 1.0f));
    }

    // Box
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(10.0f, 1.0f, 10.0f), Utl::DEG_2_RAD * Vector3f(0.0f, 0.0f, 0.0f)));
        obj->AddComponent<CBoxCollider3D>();
        auto rb = obj->AddComponent<CRigidBody>();
        rb->SetBodyType(RigidBodyType::Static);
        rb->SetGravityScale(0.0f);
        rb->SetMaterial(RigidBodyMaterial(0.5f, 0.0f));
    }

    // Main camera obj
    {
        auto obj = CreateGameObject<CGameObject>(Transformf(Vector3f(0.0f, 2.0f, -5.0f)));
        obj->AddComponent<CCameraComponent>(L"Main camera");
        obj->AddComponent<CFreeCameraControl>();
    }
}

// Drawing process
void CPhysicsSampleScene::Draw() {
    // Clear depth stencil view
    m_dsv3D.Clear();

    // Get current back buffer and clear it
    ACRenderTarget* rtv = CSwapChain::GetAny().GetBackBuffer();
    rtv->Clear(CLEAR_COLOR);

    // Apply 3D depth stencil view
    rtv->Apply(&m_dsv3D);

    // Command function of GPSO wrapper for standard layer
    m_standardGpso.SetCommand();

    // Command function of GPSO wrapper for transparent layer
    m_transparentGpso.SetCommand();
}

#endif // _FOR_PHYSICS