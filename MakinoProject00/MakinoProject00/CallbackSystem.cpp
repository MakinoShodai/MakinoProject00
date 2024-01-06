#include "CallbackSystem.h"

// Constructor
CCallbackSystem::CCallbackSystem()
    : m_currentInvokeFunctionsGroup(nullptr) 
{ }

// Add a callback function to the callback function group
void CCallbackSystem::AddFunction(const std::string& callbackName, const CallbackFunctionType& callbackFunction) {
    // If a dynamic array has not been created in the map, create it
    if (!m_functions.contains(callbackName)) {
        m_functions.emplace(callbackName, 
            CUniquePtr<CallBackFunctionElement>::Make(std::make_pair(-1, std::vector<CallbackFunctionType>())));
    }

    // Add a callback function
    m_functions[callbackName]->second.push_back(callbackFunction);
}

// Remove the callback function that is currently being invoked
void CCallbackSystem::RemoveCurrentInvokeFunction(const std::string& callbackName) {
    // Remove callback functions group if they exist in the map
    if (m_functions.contains(callbackName)) {
        CallBackFunctionElement* functions = m_functions[callbackName].Get();

        // If it's not the same group that's on call, nothing
        if (m_currentInvokeFunctionsGroup != functions) {
            return;
        }

        // Remove
        functions->second.erase(functions->second.begin() + functions->first);

        // Decrease the invoking index
        functions->first--;
    }
}

// Invoke callback functions group
bool CCallbackSystem::InvokeFunction(const std::string& callbackName) {
    // If the event functions group doesn't exist in the map, invoking fails
    if (!m_functions.contains(callbackName)) { return false; }

    // Switch the callback function group currently being invoked
    auto* prevInvokeFunctionsGroup = m_currentInvokeFunctionsGroup;
    CallBackFunctionElement* functionsGroup = m_functions[callbackName].Get();
    m_currentInvokeFunctionsGroup = functionsGroup;

    // If it is the same group as the callback functions group being invoked, invoking fails.
    if (prevInvokeFunctionsGroup == m_currentInvokeFunctionsGroup) {
        return false;
    }

    // Invoke callback functions
    for (functionsGroup->first = 0; functionsGroup->first < functionsGroup->second.size(); ++functionsGroup->first) {
        functionsGroup->second[functionsGroup->first]();
    }

    // Make the index non-invoke state
    functionsGroup->first = -1;

    // If the callback functions is no longer in the group, remove the group from the map
    if (functionsGroup->second.size() <= 0) {
        m_functions.erase(callbackName);
    }

    m_currentInvokeFunctionsGroup = prevInvokeFunctionsGroup;

    // Invoking success
    return true;
}
