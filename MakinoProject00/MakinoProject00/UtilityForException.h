/**
 * @file   UtilityForException.h
 * @brief  This file defines utility functions that are useful for exception.
 * 
 * @author Shodai Makino
 * @date   2023/12/28
 */

#ifndef __UTILITY_FOR_EXCEPTION_H__
#define __UTILITY_FOR_EXCEPTION_H__

namespace Utl {
    namespace Error {
        /** @brief Fatal exception class that must exit the application */
        class CFatalError : public std::exception {
        public:
            /**
               @brief Constructor
               @param msg Exception message
            */
            CFatalError(std::wstring msg);
            
            /** @brief Get error details */
            const std::wstring& WhatW() const noexcept { return m_message; }

        private:
            /** @brief Exception message */
            std::wstring m_message;
        };

        /** @brief Exception to stop drawing the scene */
        class CStopDrawingSceneError : public std::exception {
        public:
            /**
               @brief Constructor
               @param msg Exception message
            */
            CStopDrawingSceneError(std::wstring msg);

            /** @brief Get error details */
            const std::wstring& WhatW() const noexcept { return m_message; }

        private:
            /** @brief Exception message */
            std::wstring m_message;
        };
    } // namespace Error
} // namespace Utl

/** @brief Error handling if HRESULT fails (Exit application) */
#define HR_CHECK(str, hr) { if(FAILED(hr)) { throw Utl::Error::CFatalError(L"Failed : \"" + std::wstring(str) + L"\""); } }
/** @brief Error handling if HRESULT fails (Output window) */
#define HR_CHECK_OUTWIND(str, hr) { if(FAILED(hr)) { OutputDebugString(L"Failed : \"" str L"\"\n"); } }

#endif // !__UTILITY_FOR_EXCEPTION_H__
