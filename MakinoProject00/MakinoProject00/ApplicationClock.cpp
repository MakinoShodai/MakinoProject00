#include "ApplicationClock.h"
#include "Scene.h"
#include "PhysicsWorld.h"
#include "UtilityForDebug.h"

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

        // Output FPS
        float fps = 1.0f / m_deltaTime;
        OutputDebugStringNumeric(fps);
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
    if (ACScene::GetCurrentScenePhase() == ScenePhase::Update) {
        return (float)m_deltaTime;
    }
    else {
        return Mkpe::CPhysicsWorld::GetTimeStep();
    }
}
