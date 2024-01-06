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

protected:
    /**
       @brief Constructor
    */
    CInputSystem();

private:
    /** @brief Key states at current frame */
    BYTE m_currentKeyStates[256];
    /** @brief Key states at previous frame */
    BYTE m_prevKeyStates[256];
};

#endif // !__INPUT_SYSTEM_H__
