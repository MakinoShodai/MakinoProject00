#include "GameObject.h"
#include "Scene.h"
#include "Component.h"

// Constructor
CGameObject::CGameObject(ACScene* ownerScene, Transformf transform)
    : m_scene((ownerScene != nullptr) ? ownerScene->WeakFromThis() : nullptr)
    , m_components()
    , m_name()
    , m_isActive(true)
    , m_transform(std::move(transform))
    , m_transformBit(TransformObserve::None)
{ }

// Processing for the first update frame
void CGameObject::Start() {
    // Call start processing from components
    for (auto& it : m_components) {
        it->Start();
    }
    // Call start processing from graphics components
    for (auto& it : m_graphicsComponents) {
        it->Start();
    }
}

// Update processing
void CGameObject::Update() {
    // Call update processing from components
    for (auto& it : m_components) {
        it->Update();
    }
}

// Pre drawing processing
void CGameObject::PreDraw() {
    // Call pre drawing processing from graphics components
    for (auto& it : m_graphicsComponents) {
        it->PreDraw();
    }
}

// Process to be called at instance destruction
void CGameObject::OnDestroy() {
    // Call processing at instance destruction from components
    for (auto& it : m_components) {
        it->OnDestroy();
    }
}

// Processing at end of a frame
void CGameObject::EndFrameProcess() {
    // Transfers the change of coordinates to the previous frame
    m_transformBit <<= 4;
}

// Get the callback system
CCallbackSystem* CGameObject::GetCallbackSystem() {
    if (m_callbackSystem.Get() == nullptr) { 
        m_callbackSystem = CUniquePtrWeakable<CCallbackSystem>::Make(); 
    } 
    return m_callbackSystem.Get();
}
