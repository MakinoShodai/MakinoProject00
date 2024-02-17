#include "ScenePhase.h"
#include "Scene.h"
#include "GameObject.h"

// Set scene phases
void ScenePhasesWrapper::SetPhases(std::initializer_list<ScenePhase> phases) {
    m_phases.clear();
    for(auto it : phases) {
        m_phases.push_back(it);
    }
}

// Check the change of transform for the phase this class has
bool ScenePhasesWrapper::CheckTransformObserve(CGameObject* gameObj, TransformObserve observe) {
    for (auto it : m_phases) {
        if (gameObj->CheckTransformObserve(it, observe)) {
            return true;
        }
    }
    return false;
}

// Constructor for TransformObserveBits
TransformObserveBits::TransformObserveBits() 
    : updateBit(0)
    , prePhysicsBit(0)
    , physicsBit(0)
    , onCollisionBit(0)
{}

// Check bit strings in the specified phase
bool TransformObserveBits::Check(ScenePhase phase, TransformObserve observe) const {
    // If the phase is currently processing, refer to the previous bitstring
    if (phase == CScene::GetCurrentScenePhase()) {
        switch (phase) {
        case ScenePhase::Update: 
            return (updateBit >> 3)      & (uint8_t)observe;
        case ScenePhase::PrePhysicsUpdate:
            return (prePhysicsBit >> 3)  & (uint8_t)observe;
        case ScenePhase::Physics:
            return (physicsBit >> 3)     & (uint8_t)observe;
        case ScenePhase::OnCollision:
            return (onCollisionBit >> 3) & (uint8_t)observe;
        default:
            throw Utl::Error::CFatalError(L"A non-existent ScenePhase was sent");
        }
    }
    else {
        switch (phase) {
        case ScenePhase::Update:
            return updateBit      & (uint8_t)observe;
        case ScenePhase::PrePhysicsUpdate:
            return prePhysicsBit  & (uint8_t)observe;
        case ScenePhase::Physics:
            return physicsBit     & (uint8_t)observe;
        case ScenePhase::OnCollision:
            return onCollisionBit & (uint8_t)observe;
        default:
            throw Utl::Error::CFatalError(L"A non-existent ScenePhase was sent");
        }
    }
}

// Transfer current bit strings of the current scene phase to previous bit strings
void TransformObserveBits::Transfer() {
    switch (CScene::GetCurrentScenePhase()) {
    case ScenePhase::Update:
        updateBit      <<= 3;
        break;
    case ScenePhase::PrePhysicsUpdate:
        prePhysicsBit  <<= 3;
        break;
    case ScenePhase::Physics:
        physicsBit     <<= 3;
        break;
    case ScenePhase::OnCollision:
        onCollisionBit <<= 3;
        break;
    default:
        throw Utl::Error::CFatalError(L"A non-existent ScenePhase was sent");
    }
}

// Stand a bit string of the current scene phase
void TransformObserveBits::SetBit(TransformObserve observe) {
    switch (CScene::GetCurrentScenePhase()) {
    case ScenePhase::Update:
        updateBit      |= (uint8_t)observe;
        break;
    case ScenePhase::PrePhysicsUpdate:
        prePhysicsBit  |= (uint8_t)observe;
        break;
    case ScenePhase::Physics:
        physicsBit     |= (uint8_t)observe;
        break;
    case ScenePhase::OnCollision:
        onCollisionBit |= (uint8_t)observe;
        break;
    default:
        throw Utl::Error::CFatalError(L"A non-existent ScenePhase was sent");
    }
}
