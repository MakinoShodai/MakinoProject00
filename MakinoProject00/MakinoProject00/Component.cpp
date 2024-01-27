#include "Component.h"
#include "GameObject.h"
#include "Scene.h"

// Constructor
ACComponent::ACComponent(CGameObject* owner) 
    : m_gameObj((owner != nullptr) ? owner->WeakFromThis() : nullptr)
    , m_isActive(true)
{ }

// Get transform of the object
const Transformf& ACComponent::GetTransform() const {
    return m_gameObj->GetTransform();
}

// Get scene where this component exists
CWeakPtr<CScene> ACComponent::GetScene() {
    return m_gameObj->GetScene();
}

// Get scene where this component exists
CWeakPtr<const CScene> ACComponent::GetScene() const {
    return m_gameObj->GetScene();
}

// Get the callback system that the gameobject has
CCallbackSystem* ACComponent::GetCallbackSystem() {
    return m_gameObj->GetCallbackSystem();
}

// Set the active flag of this component
void ACComponent::SetIsActive(bool isActive) {
    if (m_isActive && !isActive) {
        m_isActive = false;
        OnDisable();
    }
    else if (!m_isActive && isActive) {
        m_isActive = true;
        OnEnable();
    }
}

// Is this component itself and the game object that owns it active?
bool ACComponent::IsActiveOverall() const {
    return m_isActive && m_gameObj->IsActive();
}
