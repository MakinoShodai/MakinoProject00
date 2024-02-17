#include "ApplicationClock.h"
#include "Scene.h"
#include "PhysicsWorld.h"
#include "UtilityForDebug.h"
#include "ImguiHelper.h"
#include "SceneRegistry.h"

// Destructor
CAppClock::~CAppClock() {
    // Clear a minimum resolution for periodic timers
    timeEndPeriod(1);
}

// Manage frame rates
bool CAppClock::ManageFrameRate() {
    // If the application is sleeping, early return
    if (m_isSleep) {
        return false;
    }

    // Get current time
    Seconds currentTime = std::chrono::duration_cast<Seconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
    
    // If the previous frame's time was retained
    if (m_prevTime.has_value()) {
        // If the elapsed time does not exceed the time of one frame, return false
        double elapsedTime = (currentTime - m_prevTime.value()).count();
        if (elapsedTime < m_frameTime) {
            return false;
        }

        // Get current time again
        currentTime = std::chrono::duration_cast<Seconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
        
        // Recalculate elapsed time from previous frame
        m_deltaTime = (float)((currentTime - m_prevTime.value()).count());
    }
    else {
        // Set the elapsed time to the time of one frame as it is
        m_deltaTime = (float)m_frameTime;
    }
    // Update prev frame time
    m_prevTime = currentTime;

    return true;
}

// Initialize
void CAppClock::Initialize(double fps) {
#ifdef _EDITOR
    // Set window function for drawing fps
    CImguiHelper::GetAny().AddWindowFunction([this]() -> bool {
        if (CSceneRegistry::GetMain().IsDisplayFPS()) {
            char fpsText[32];
            snprintf(fpsText, sizeof(fpsText), "FPS: %.1f", 1.0f / this->m_deltaTime);

            // Set background color of the next window to 0.2
            ImGui::SetNextWindowBgAlpha(0.2f);

            // Set position of the next window to top left of screen
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);

            // Calculate size of the fps text and set it to size of the next window
            ImVec2 textSize = ImGui::CalcTextSize(fpsText);
            ImGui::SetNextWindowSize(ImVec2(textSize.x + 20, textSize.y + 20));

            // Draw fps
            ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
            ImGui::Begin("FPSWindow", nullptr, flags);
            ImGui::Text("%s", fpsText);
            ImGui::End();
        }

        return true;
        });
#endif // _EDITOR

    SetFPS(fps);

    // Request a minimum resolution for periodic timers
    timeBeginPeriod(1);
}

// Set sleep mode of the application
void CAppClock::SleepApplication(bool isSleep) {
    // If sleep is wakened, resets the time of the previous frame
    if (m_isSleep && !isSleep) {
        m_prevTime = std::nullopt;
    }

    // Set flag
    m_isSleep = isSleep;
}

// Get the delta time appropriate for the timing of the call to this function
float CAppClock::GetAppropriateDeltaTime() {
    if (CScene::GetCurrentScenePhase() == ScenePhase::Update) {
        return (float)m_deltaTime;
    }
    else {
        return Mkpe::CPhysicsWorld::GetTimeStep();
    }
}
