#include "DirectionalLight.h"
#include "Scene.h"
#include "SceneRegistry.h"

// Initial light direction
const Vector3f INIT_LIGHT_DIR = Vector3f(0.2f, -1.0f, -0.2f);
// Initial light color
const Colorf INIT_LIGHT_COLOR = Colorf(1.0f, 0.9568627450980392f, 0.8392156862745098f, 1.0f);
// Initial ambient color
const Colorf INIT_AMBIENT_COLOR = Colorf(0.01f, 0.01f, 0.01f, 1.0f);
// Initial intensity
const float INIT_INTENSITY = 1.0f;

// Awake processing
void CDirectionalLightComponent::Awake() {
    SetLightDir(INIT_LIGHT_DIR);
    SetLightColor(INIT_LIGHT_COLOR);
    SetAmbientColor(INIT_AMBIENT_COLOR);
    SetIntensity(INIT_INTENSITY);

#ifdef _EDITOR
    if (CSceneRegistry::GetAny().IsEditorMode()) {
        GetScene()->GetLightRegistry()->SetDirectionalLight(WeakFromThis());
    }
#endif // _EDITOR
}

// Starting process
void CDirectionalLightComponent::Start() {
#ifdef _EDITOR
    if (false == CSceneRegistry::GetAny().IsEditorMode()) {
        GetScene()->GetLightRegistry()->SetDirectionalLight(WeakFromThis());
    }
#else
    GetScene()->GetLightRegistry()->SetDirectionalLight(WeakFromThis());
#endif // !_EDITOR
}

// Set direction vector of this light
void CDirectionalLightComponent::SetLightDir(const Vector3f& dir) {
    m_gameObj->SetRotation(Utl::Math::VectorToVectorQuaternion(Utl::Math::UNIT3_FORWARD, dir.GetNormalize()));
}

// Prefab function
void CDirectionalLightPrefab::Prefab() {
    AddComponent<CDirectionalLightComponent>();

    SetName(OBJNAME_DIRECTIONAL_LIGHT);
}
