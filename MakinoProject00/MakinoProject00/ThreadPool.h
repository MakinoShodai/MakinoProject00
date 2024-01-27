/**
 * @file   ThreadPool.h
 * @brief  This file handles thread pool.
 * 
 * @author Shodai Makino
 * @date   2023/11/25
 */

#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include "Singleton.h"
#include "UtilityForException.h"
#include "ApplicationError.h"

/** @brief This class is thread pool */
class CThreadPool : public ACMainThreadSingleton<CThreadPool> {
    // Friend declaration
    friend class ACSingletonBase;

public:
    /**
       @brief Destructor
    */
    ~CThreadPool();

    /**
       @brief Initialization process to start a thread
       @param threadNum The number of threads to be used
    */
    void Initialize(UINT threadNum);

    /** @brief Feature for thread-safe */
    class CThreadSafeFeature : public ACInnerClass<CThreadPool> {
    public:
        // Friend declaration
        using ACInnerClass<CThreadPool>::ACInnerClass;

        /**
           @brief Dequeue one task to the queue
           @tparam F Function type
           @tparam Args All argument types

           @param function Function of task
           @param args Arguments to bind to the function
           @return Future object from the task
        */
        template<class F, class... Args>
        auto EnqueueTask(F&& function, Args&&... args);
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
    CThreadPool() : ACMainThreadSingleton(-100), m_isDestruct(false) {}

private:
    /**
       @brief Function to run in each thread to handle tasks
    */
    void TasksProcessor();

private:
    /** @brief Array of thread handles */
    std::vector<std::thread> m_threads;
    /** @brief Queue for all tasks */
    std::queue<std::function<void()>> m_tasksQueue;
    /** @brief Mutex for queue exclusion */
    std::mutex m_queueMutex;
    /** @brief Variable for sleeving each threads */
    std::condition_variable m_condition;
    /** @brief Destructor called? */
    bool m_isDestruct;
};

// Dequeue one task to the queue
template<class F, class ...Args>
auto CThreadPool::CThreadSafeFeature::EnqueueTask(F&& function, Args && ...args) {
    // Get the return value type
    using ReturnType = decltype(function(std::forward<Args>(args)...));

    // Create the wrapper function
    // #NOTE : packaged_task can only supplement exception with the get function of future, 
    //         so perform exception supplement processing before packaging
    auto wrapperFunction = [function = std::forward<F>(function),
        ...args = std::forward<Args>(args)]() -> ReturnType {
        try {
            return function(args...);
        }
        catch (const Utl::Error::CFatalError& e) {
            // Error processing
            CApplicationError::GetAny().Exit(e.WhatW());
            throw e;
        }
    };

    // Make the packaged_task object
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(wrapperFunction);

    // Get the future object from the packaged_task object
    std::future<ReturnType> fet = task->get_future();

    // Operation on the queue
    {
        // Exclusion control for the queue 
        std::unique_lock<std::mutex> lock(m_owner->m_queueMutex);

        // Enqueue task
        m_owner->m_tasksQueue.emplace([task]() { (*task)(); });
    }

    // wakes up one of the waiting threads
    m_owner->m_condition.notify_one();

    // return the future object
    return fet;
}

#endif // !__THREAD_POOL_H__
