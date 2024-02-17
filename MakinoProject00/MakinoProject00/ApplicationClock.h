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
       @param fps Frame per seconds
    */
    void Initialize(double fps);

    /**
       @brief Set sleep mode of the application
       @param isSleep Sleep the application? 
    */
    void SleepApplication(bool isSleep);

    /**
       @brief Set frame per seconds
       @param fps Frame per seconds
    */
    void SetFPS(double fps) { m_frameTime = 1 / fps; }

    /**
       @brief Get the delta time appropriate for the timing of the call to this function
       @return Delta time
    */
    float GetAppropriateDeltaTime();

    /** @brief Get elapsed time from prev frame */
    float GetDeltaTime() { return m_deltaTime; }

    /** @brief Feature for thread-safe */
    class CThreadSafeFeature : public ACInnerClass<CAppClock> {
    public:
        // Friend declaration
        using ACInnerClass<CAppClock>::ACInnerClass;

        /** @brief Get time per frame */
        float GetFrameTime() { return (float)m_owner->m_frameTime; }
    };

    /** @brief Get feature for thread-safe */
    inline static CThreadSafeFeature& GetAny() {
        static CThreadSafeFeature instance(GetProtected().Get());
        return instance;
    }
protected:
    /**
       @brief Constructor
    */
    CAppClock() : ACMainThreadSingleton(0), m_deltaTime(0.0f), m_frameTime(0.0), m_isSleep(false) {}

private:
    /** @brief Time of prev frame */
    std::optional<Seconds> m_prevTime;
    /** @brief Elapsed time from prev frame */
    float m_deltaTime;
    /** @brief Time per frame */
    std::atomic<double> m_frameTime;
    /** @brief Sleep the application? */
    bool m_isSleep;
};

#endif // !__APPLICATION_CLOCK_H__
