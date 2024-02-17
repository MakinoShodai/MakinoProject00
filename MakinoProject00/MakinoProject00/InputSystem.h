/**
 * @file   InputSystem.h
 * @brief  This file handles key input information.
 * 
 * @author Shodai Makino
 * @date   2023/12/25
 */

#ifndef __INPUT_SYSTEM_H__
#define __INPUT_SYSTEM_H__

#include "Singleton.h"
#include "UtilityForMath.h"

/** @brief This class handles key input information */
class CInputSystem : public ACMainThreadSingleton<CInputSystem> {
    // Friend declaration
    friend ACSingletonBase;

public:
    /** @brief Destructor */
    ~CInputSystem() = default;

    /**
       @brief Update processing that must be called every frame
    */
    void Update();

    /**
       @brief Is the specified key currently pressed?
       @param key Key to be checked
       @return Result
    */
    bool IsKey(BYTE key);

    /**
       @brief Starting to press the key specified for the current frame?
       @param key Key to be checked
       @return Result
    */
    bool IsKeyDown(BYTE key);

    /**
       @brief Released the specified key on the current frame?
       @param key Key to be checked
       @return Result
    */
    bool IsKeyUp(BYTE key);

    /**
       @brief Generate direction vectors based on key input
       @param dir Return value for direction
       @param plusX +X key
       @param minusX -X key
       @param plusY +Y key
       @param minusY -Y key
       @return Is the key pressed?
       @details
       If keys on the same axis are pressed at the same time, no input is assumed
    */
    bool IsKeyGenerateDir(Vector2f* dir, BYTE plusX = 'D', BYTE minusX = 'A', BYTE plusY = 'W', BYTE minusY = 'S');

    /** @brief Get amount of change in cursor position from previous frame */
    POINT GetDeltaCursorPos() { return m_deltaCursorPos; }

    /** @brief Set a flag that the cursor repeats in the screen */
    void SetIsRepeatCursorInScreen(bool isRepeat) { m_isRepeatCursor = isRepeat; }

protected:
    /**
       @brief Constructor
    */
    CInputSystem();

    /**
       @brief Correct cursor position to repeat within specified range
    */
    void RepeatCursor(const RECT& repeatRange, POINT* cursorPos);

private:
    /** @brief Key states at current frame */
    BYTE m_currentKeyStates[256];
    /** @brief Key states at previous frame */
    BYTE m_prevKeyStates[256];
    /** @brief Amount of change in cursor position from previous frame */
    POINT m_deltaCursorPos;
    /** @brief Cursor position at previous frame */
    POINT m_prevCursorPos;

    /** @brief Does the cursor position repeat on the screen? */
    bool m_isRepeatCursor;
};

#endif // !__INPUT_SYSTEM_H__
