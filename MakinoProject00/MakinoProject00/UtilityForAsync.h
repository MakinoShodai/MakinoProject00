/**
 * @file   UtilityForAsync.h
 * @brief  This file defines utility functions that are useful for asynchronous.
 *
 * @author Shodai Makino
 * @date   2023/12/28
 */

#ifndef __UTILITY_FOR_ASYNC_H__
#define __UTILITY_FOR_ASYNC_H__

#include "ApplicationError.h"

namespace Utl {
    namespace Async {
        /** @brief Interval to check for errors */
        const std::chrono::seconds INTERVAL_CHECK = std::chrono::seconds(100);
        
        /** @brief Class that wraps future to wait while the main thread checks for the exit of the application */
        template<typename T>
        class CMainThreadFuture {
        public:
            /**
               @brief Constructor
            */
            CMainThreadFuture() : m_future() {}

            /**
               @brief Constructor
            */
            CMainThreadFuture(std::future<T>&& fut) : m_future(std::move(fut)) {}

            /** @brief Destructor */
            ~CMainThreadFuture() = default;

            /** @brief Move assignment operator */
            CMainThreadFuture& operator=(std::future<T>&& fut) { m_future = std::move(fut); return *this; }

            /**
               @brief Wait for processing to complete
            */
            void Wait() const;

            /** @brief Does this future class have a shared state? */
            inline bool Valid() { return m_future.valid(); }

            /** @brief Get result */
            T Get() requires (!std::is_void_v<T>) { Wait(); return std::move(m_future.get()); }

            /** @brief Get result for void */
            void Get() requires (std::is_void_v<T>) { Wait(); m_future.get(); }

        private:
            /** @brief Future class */
            std::future<T> m_future;
        };

    } // namespace Async
} // namespace Utl

// Wait for processing to complete
template<typename T>
void Utl::Async::CMainThreadFuture<T>::Wait() const {
    while (m_future.wait_for(INTERVAL_CHECK) != std::future_status::ready) {
        // Check application exit
        CApplicationError::GetAny().Check();
    }
}

#endif // !__UTILITY_FOR_ASYNC_H__
