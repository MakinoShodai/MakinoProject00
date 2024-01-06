#include "ThreadPool.h"
#include "UtilityForException.h"
#include "ApplicationError.h"

// Destructor
CThreadPool::~CThreadPool() {
    // Avoid conflicts with queue operations
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_isDestruct = true;
    }

    // Wake up all waiting threads
    m_condition.notify_all();

    // Wait for all threads to finish processing
    for (std::thread& th : m_threads) {
        th.join();
    }
}

// Initialization process to start a thread
void CThreadPool::Initialize(UINT threadNum) {
    for (UINT i = 0; i < threadNum; ++i) {
        m_threads.emplace_back(&CThreadPool::TasksProcessor, this);
    }
}

// Function to run in each thread to handle tasks
void CThreadPool::TasksProcessor() {
    while (true) {
        std::function<void()> task;

        // Operation on the queue
        {
            // Exclusion control for the queue 
            std::unique_lock<std::mutex> lock(m_queueMutex);

            // If the queue is empty, put this thread to sleep until instructed
            if (m_tasksQueue.empty()) {
                m_condition.wait(lock, [this] {
                    return this->m_isDestruct || false == this->m_tasksQueue.empty();
                    });
            }

            // If the destructor has beed called and all tasks have been completed, exit this thread
            if (m_isDestruct && m_tasksQueue.empty()) {
                return;
            }

            // Dequeue one task from the queue
            task = std::move(m_tasksQueue.front());
            m_tasksQueue.pop();
        }

        // Execute task
        task();
    }
}
