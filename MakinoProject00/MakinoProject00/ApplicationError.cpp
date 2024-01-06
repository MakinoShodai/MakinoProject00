#include "ApplicationError.h"
#include "UtilityForException.h"

// Exit the application
void CApplicationError::Exit(const std::wstring& exitMessage) {
    // If the application hasn't yet decided to exit, decide to exit
    if (!m_isExit.load()) {
        m_isExit.store(true);

        // Set application exit message
        m_exitMessage = exitMessage;
    }
}

// Check application exit and throws an exception if it exits
void CApplicationError::Check() {
    if (m_isExit.load()) {
        throw Utl::Error::CFatalError(m_exitMessage);
    }
}
