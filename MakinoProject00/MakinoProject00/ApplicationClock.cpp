#include "ApplicationClock.h"

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
        // If the elapsed time is within the time of one frame
        double elapsedTime = (currentTime - m_prevTime.value()).count();
        if (elapsedTime < m_frameTime) {
            // Sleep threads until 1 frame time
            Seconds sleepTime(m_frameTime - elapsedTime);
            Sleep(static_cast<DWORD>((m_frameTime - elapsedTime) * 1000));
        }

        // Get current time again
        currentTime = std::chrono::duration_cast<Seconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
        
        // Recalculate elapsed time from previous frame
        m_deltaTime = (currentTime - m_prevTime.value()).count();

        // #TODO : 
        OutputDebugString((L"FPS : " + std::to_wstring(1 / m_deltaTime) + L"\n").c_str());
    }
    else {
        // Set the elapsed time to the time of one frame as it is
        m_deltaTime = m_frameTime;
    }
    // Update prev frame time
    m_prevTime = currentTime;

    return true;
}

// Initialize
void CAppClock::Initialize() {
    // 60FPS
    m_frameTime = 1.0 / 5000.0;

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
