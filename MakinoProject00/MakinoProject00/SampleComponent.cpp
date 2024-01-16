#ifdef _SAMPLE
#include "SampleComponent.h"
#include "GameObject.h"
#include "ApplicationClock.h"
#include "InputSystem.h"

// Updating process for CSampleRotateComponent
void CSampleRotateComponent::Update() {
    Quaternionf rotate = GetTransform().rotation;
    rotate.AngleAxis(m_speed * CAppClock::GetMain().GetDeltaTime(), m_axis);
    m_gameObj->SetRotation(rotate);
}

// Starting process for CSampleAnimComponent
void CSampleAnimComponent::Start() {
    // Initialize variable
    m_currentAnimID = 0;

    // Get model component and play animation
    m_model = m_gameObj->GetComponent<CSkeletalModel>();
    m_model->GetController()->Play(m_currentAnimID, true);
}

// Updating process for CSampleAnimComponent
void CSampleAnimComponent::Update() {
    // Change animation
    if (CInputSystem::GetMain().IsKeyDown('K')) {
        m_currentAnimID = (m_currentAnimID + 1) % 2;
        m_model->GetController()->Play(m_currentAnimID, true);
    }

    // Bind pose
    if (CInputSystem::GetMain().IsKeyDown('N')) {
        m_model->GetController()->BindPose();
    }
}

#endif // _SAMPLE