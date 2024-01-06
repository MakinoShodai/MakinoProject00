/**
 * @file   ApplicationError.h
 * @brief  This file handles processing when the application makes an error.
 * 
 * @author Shodai Makino
 * @date   2023/12/28
 */

#ifndef __APPLICATION_ERROR_H__
#define __APPLICATION_ERROR_H__

#include "Singleton.h"

/** @brief This class handles processing when the application makes an error */
class CApplicationError : public ACAnyThreadSingleton<CApplicationError> {
    // Friend declaration
    friend class ACSingletonBase;

public:
    /** @brief Destructor */
    ~CApplicationError() = default;

    /**
       @brief Exit the application
       @param errorMessage Application exit message
    */
    void Exit(const std::wstring& exitMessage);

    /**
       @brief Check application exit and throws an exception if it exits
    */
    void Check();

private:
    /**
       @brief Constructor
    */
    CApplicationError() : ACAnyThreadSingleton(100) {}

private:
    /** @brief Exit this application? */
    std::atomic<bool> m_isExit;
    /** @brief Application exit message */
    std::wstring m_exitMessage;
};


#endif // !__APPLICATION_ERROR_H__
