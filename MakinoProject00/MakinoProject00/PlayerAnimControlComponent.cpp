#include "PlayerAnimControlComponent.h"
#include "ModelRegistry.h"
#include "AssetNameDefine.h"

// Starting process
void CPlayerAnimControlComponent::Start() {
    // Get component
    m_model = m_gameObj->GetComponent<CSkeletalModel>();
    m_playerControl = m_gameObj->GetComponent<CPlayerControlComponent>();

    // Get animation id and make its wrapper
    m_idleAnim = AnimIDWithLoop(CModelRegistry::GetAny().GetAnimID(AnimName::CUTEBIRD_IDLE), true);
    m_runAnim = AnimIDWithLoop(CModelRegistry::GetAny().GetAnimID(AnimName::CUTEBIRD_RUN), true);
    m_jumpAnim = AnimIDWithLoop(CModelRegistry::GetAny().GetAnimID(AnimName::CUTEBIRD_JUMP), false);
}

// Updating process
void CPlayerAnimControlComponent::Update() {
    CDynamicModelController* animController = m_model->GetController();

    // 
    if (m_playerControl->IsMoved()) {
        animController->Play(m_runAnim);
    }
    else {
        animController->Play(m_idleAnim);
    }
}
