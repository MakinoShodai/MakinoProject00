#include "InputSystem.h"
#include "Application.h"

// Update processing that must be called every frame
void CInputSystem::Update() {
    // Save key information from previous frame
    memcpy(m_prevKeyStates, m_currentKeyStates, sizeof(m_currentKeyStates));
    
    // Get current keyboard states
    if (!GetKeyboardState(m_currentKeyStates)) {
        // If current keyboard states can't be retrieved, output a warning message
        OutputDebugString(L"Warning! : Key entry information couldn't be retrieved correctly.\n");
    }

    // Get current cursor position
    POINT cursorPos;
    if (GetCursorPos(&cursorPos)) {
        // Compute amount of change from previous frame
        m_deltaCursorPos.x = cursorPos.x - m_prevCursorPos.x;
        m_deltaCursorPos.y = cursorPos.y - m_prevCursorPos.y;

        // If the cursor position repeats in the screen, correct it.
        if (m_isRepeatCursor) {
            const RECT& rect = CApplication::GetAny().GetCursorRange();
            RepeatCursor(rect, &cursorPos);
        }

        // Update cursor position at previous frame
        m_prevCursorPos = cursorPos;
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

// Generate direction vectors based on key input
bool CInputSystem::IsKeyGenerateDir(Vector2f* dir, BYTE plusX, BYTE minusX, BYTE plusY, BYTE minusY) {
    bool isPressedX = false;
    bool isPressedY = false;
    *dir = Vector2f::Zero();
    if (IsKey(plusX)) {
        dir->x() += 1.0f; isPressedX = !isPressedX;
    }
    if (IsKey(minusX)) {
        dir->x() -= 1.0f; isPressedX = !isPressedX;
    }
    if (IsKey(plusY)) {
        dir->y() += 1.0f; isPressedY = !isPressedY;
    }
    if (IsKey(minusY)) {
        dir->y() -= 1.0f; isPressedY = !isPressedY;
    }
    return isPressedX || isPressedY;
}

// Constructor
CInputSystem::CInputSystem() 
    : ACMainThreadSingleton(0)
    , m_currentKeyStates{}
    , m_prevKeyStates{}
    , m_deltaCursorPos{ 0, 0 }
    , m_prevCursorPos{ 0, 0 }
    , m_isRepeatCursor(false) {
    std::fill(std::begin(m_currentKeyStates), std::end(m_currentKeyStates), 0);
    std::fill(std::begin(m_prevKeyStates), std::end(m_prevKeyStates), 0);
    GetCursorPos(&m_prevCursorPos);
}

// Correct cursor position to repeat within specified range
void CInputSystem::RepeatCursor(const RECT& repeatRange, POINT* cursorPos) {
    bool isCorrect = false;
    // Horizontal
    if (cursorPos->x >= repeatRange.right) {
        cursorPos->x = repeatRange.left + 1;
        isCorrect = true;
    }
    else if (cursorPos->x <= repeatRange.left) {
        cursorPos->x = repeatRange.right - 1;
        isCorrect = true;
    }
    // Vertical
    if (cursorPos->y >= repeatRange.bottom) {
        cursorPos->y = repeatRange.top + 1;
        isCorrect = true;
    }
    else if (cursorPos->y <= repeatRange.top) {
        cursorPos->y = repeatRange.bottom - 1;
        isCorrect = true;
    }
    if (isCorrect) {
        SetCursorPos(cursorPos->x, cursorPos->y);
    }
}
