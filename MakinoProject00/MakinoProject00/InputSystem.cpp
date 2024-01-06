#include "InputSystem.h"

// Update processing that must be called every frame
void CInputSystem::Update() {
    // Save key information from previous frame
    memcpy(m_prevKeyStates, m_currentKeyStates, sizeof(m_currentKeyStates));
    
    // Get current keyboard states
    if (!GetKeyboardState(m_currentKeyStates)) {
        // If current keyboard states can't be retrieved, output a warning message
        OutputDebugString(L"Warning! : Key entry information couldn't be retrieved correctly.\n");
    }
}

// Is the specified key currently pressed?
bool CInputSystem::IsKey(BYTE key) {
    return m_currentKeyStates[key] & 0x80;
}

// Starting to press the key specified for the current frame?
bool CInputSystem::IsKeyDown(BYTE key) {
    return (m_currentKeyStates[key] ^ m_prevKeyStates[key]) & m_currentKeyStates[key] & 0x80;
}

// Released the specified key on the current frame?
bool CInputSystem::IsKeyUp(BYTE key) {
    return (m_currentKeyStates[key] ^ m_prevKeyStates[key]) & m_prevKeyStates[key] & 0x80;
}

// Constructor
CInputSystem::CInputSystem() 
    : ACMainThreadSingleton(0)
    , m_currentKeyStates{}
    , m_prevKeyStates{} {
    std::fill(std::begin(m_currentKeyStates), std::end(m_currentKeyStates), 0);
    std::fill(std::begin(m_prevKeyStates), std::end(m_prevKeyStates), 0);
}
