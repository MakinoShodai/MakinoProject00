/**
 * @file   CallbackSystem.h
 * @brief  This file handles object's callback function system.
 * 
 * @author Shodai Makino
 * @date   2023/07/22
 */

#ifndef __CALLBACK_SYSTEM_H__
#define __CALLBACK_SYSTEM_H__

#include "UniquePtr.h"

// Callback function type
using CallbackFunctionType = std::function<void()>;
// Type of element for callback function and its invoking index
using CallBackFunctionElement = std::pair<int, std::vector<CallbackFunctionType>>;
// Type of map to find callback functions and their invoking index from name
using CallbackFunctionMap = std::unordered_map<std::string, CUniquePtr<CallBackFunctionElement>>;

// Object's callback function system
class CCallbackSystem {
public:
    
    /**
       @brief Constructor
    */
    CCallbackSystem();

    /**
       @brief Destructor
    */
    ~CCallbackSystem() = default;

    /**
       @brief Add a callback function to the callback function group
       @param callbackName Name of the callback function group to be added
       @param callbackFunction The callback function to be added
    */
    void AddFunction(const std::string& callbackName, const CallbackFunctionType& callbackFunction);

    /**
       @brief Remove the callback function that is currently being invoked
       @param callbackName Name of the callback function group to be removed
       @attention The invoking function itself should call this function
    */
    void RemoveCurrentInvokeFunction(const std::string& callbackName);

    /**
       @brief Invoke callback functions group
       @param callbackName Name of the callback function group to be invoked
       @return Successful invoking?
    */
    bool InvokeFunction(const std::string& callbackName);

private:
    /** @brief Map to find callback functions and their invoking index from name */
    CallbackFunctionMap m_functions;
    /** @brief Current invoking group of functions */
    CallBackFunctionElement* m_currentInvokeFunctionsGroup;
};

#endif // !__CALLBACK_SYSTEM_H__
