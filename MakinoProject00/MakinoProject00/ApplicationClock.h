/**
 * @file   ApplicationClock.h
 * @brief  This file manages the clock in the application.
 * 
 * @author Shodai Makino
 * @date   2023/11/27
 */

#ifndef __APPLICATION_CLOCK_H__
#define __APPLICATION_CLOCK_H__

#include "Singleton.h"

/** @brief This class manages the clock in the application */
class CAppClock : public ACMainThreadSingleton<CAppClock> {
    // Friend declaration
    friend class ACSingletonBase;

    using Seconds = std::chrono::duration<double>;

public:
    /**
       @brief Destructor
    */
    ~CAppClock();

    /**
       @brief Manage frame rates
       @return Proceed with the application process?
    */
    bool ManageFrameRate();

    /**
       @brief Initialize
    */
    void Initialize();

    /**
       @brief Set sleep mode of the application
       @param isSleep Sleep the application? 
    */
    void SleepApplication(bool isSleep);

    /** @brief Get elapsed time from prev frame */
    double GetDeltaTime() { return m_deltaTime; }

protected:
    /**
       @brief Constructor
    */
    CAppClock() : ACMainThreadSingleton(0), m_deltaTime(0.0), m_frameTime(0.0), m_isSleep(false) {}

private:
    /** @brief Time of prev frame */
    std::optional<Seconds> m_prevTime;
    /** @brief Elapsed time from prev frame */
    double m_deltaTime;
    /** @brief Time per frame */
    double m_frameTime;
    /** @brief Sleep the application? */
    bool m_isSleep;
};

#endif // !__APPLICATION_CLOCK_H__
