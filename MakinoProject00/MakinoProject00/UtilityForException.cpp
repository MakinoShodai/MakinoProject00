#include "UtilityForException.h"
#include "SwapChain.h"
#include "CommandManager.h"

// Constructor
Utl::Error::CFatalError::CFatalError(std::wstring msg) 
    : m_message(std::move(msg)) { 
    if (Utl::CMainThreadChecker::IsMainThread()) {
        // Sequre commands
        CCommandManager::GetMain().SecureCommands();;
    }
}

// Constructor
Utl::Error::CStopDrawingSceneError::CStopDrawingSceneError(std::wstring msg) 
    : m_message(std::move(msg)) {
    if (Utl::CMainThreadChecker::IsMainThread()) {
        // Clear command list
        CCommandManager::GetMain().ClearCurrentCommandList();
    }
}
