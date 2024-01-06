#include "Component.h"
#include "GameObject.h"

// Constructor
ACComponent::ACComponent(CGameObject* owner) 
    : m_gameObj((owner != nullptr) ? owner->WeakFromThis() : nullptr)
    , m_isActive(true)
{ }
