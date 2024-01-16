#include "GameObject.h"
#include "Scene.h"
#include "Component.h"
#include "ContactColliderList.h"
#include "Collider3D.h"
#include "Shape.h"

// Constructor
CGameObject::CGameObject(ACScene* ownerScene, Transformf transform)
    : m_scene((ownerScene != nullptr) ? ownerScene->WeakFromThis() : nullptr)
    , m_components()
    , m_name()
    , m_isActive(true)
    , m_transform(std::move(transform))
    , m_transformBits()
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
        if (!it->IsActiveSelf()) { continue; }
        it->Update();
    }
}

// Pre-update process for physics simulation
void CGameObject::PrePhysicsUpdate() {
    // Call pre-update process for physics simulation from components
    for (auto& it : m_components) {
        if (!it->IsActiveSelf()) { continue; }
        it->PrePhysicsUpdate();
    }
}

// Pre drawing processing
void CGameObject::PreDraw() {
    // Call pre drawing processing from graphics components
    for (auto& it : m_graphicsComponents) {
        if (!it->IsActiveSelf()) { continue; }
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

// Processing for collider collisions
void CGameObject::OnCollision() {
    for (const auto& collider : m_colliders) {
        const Mkpe::CContactColliderList& contactColliderLists = collider->GetContactColliderdLists();

        // Processing for begin list
        const std::vector<CContactOpponent>& beginList = contactColliderLists.GetBeginList();
        for (const auto& it : beginList) {
            // If it's nullptr, do nothing
            if (it.GetOpponent() == nullptr) { continue; }
            // Call processing when collision begins for all components
            for (auto& component : m_components) {
                component->OnCollisionBegin(it);
            }
        }

        // Processing for hit list
        const std::vector<CContactOpponent>& hitList = contactColliderLists.GetHitList();
        for (const auto& it : hitList) {
            // If it's nullptr, do nothing
            if (it.GetOpponent() == nullptr) { continue; }
            // Call processing during collision for all components
            for (auto& component : m_components) {
                component->OnCollisionHit(it);
            }
        }

        // Processing for end list
        const std::vector<CWeakPtr<ACCollider3D>>& endList = contactColliderLists.GetEndList();
        for (const auto& it : endList) {
            // If it's nullptr, do nothing
            if (it == nullptr) { continue; }
            // Call processing when collision ends for all components
            for (auto& component : m_components) {
                component->OnCollisionEnd(it);
            }
        }
    }
}

// Set the active flag
void CGameObject::SetIsActive(bool isActive) {
    if (m_isActive && !isActive) {
        m_isActive = false;
        
        for (auto& it : m_components) {
            if (false == it->IsActiveSelf()) { continue; }
            it->OnDisable();
        }
    }
    else if (!m_isActive && isActive) {
        m_isActive = true;

        for (auto& it : m_components) {
            if (false == it->IsActiveSelf()) { continue; }
            it->OnEnable();
        }
    }
}

// Get the callback system
CCallbackSystem* CGameObject::GetCallbackSystem() {
    if (!m_callbackSystem) { 
        m_callbackSystem = CUniquePtrWeakable<CCallbackSystem>::Make(); 
    } 
    return m_callbackSystem.Get();
}

// Processing when a collider is added
void CGameObject::ColliderAdded(const CWeakPtr<ACCollider3D>& collider) {
    m_colliders.push_back(collider.Get());

#ifdef _FOR_PHYSICS
    AddComponent<CDebugColliderShape>(GraphicsLayer::Standard, collider);
#else
#ifndef _DEBUG
    AddComponent<CDebugColliderShape>(GraphicsLayer::Transparent, collider);
#endif // _DEBUG
#endif // _FOR_PHYSICS
}
