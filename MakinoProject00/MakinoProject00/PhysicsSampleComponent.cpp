#ifdef _FOR_PHYSICS
#include "PhysicsSampleComponent.h"
#include "GameObject.h"

// Starting process
void CSampleOnCollisionComponent::Start() {
    m_shapeComponent = m_gameObj->GetComponent<CDebugColliderShape>();
}

// Processing when collision begins
void CSampleOnCollisionComponent::OnCollisionBegin(const CContactOpponent& opponent) {
    m_shapeComponent->SetColor(Colorf(1.0f, 0.0f, 0.0f, 0.5f));
}

#endif // _FOR_PHYSICS