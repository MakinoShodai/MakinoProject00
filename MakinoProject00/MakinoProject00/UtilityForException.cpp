#include "UtilityForException.h"
#include "SwapChain.h"
#include "CommandManager.h"

// Constructor
Utl::Error::CFatalError::CFatalError(std::wstring msg) 
    : m_message(std::move(msg)) { 
    if (Utl::CMainThreadChecker::IsMainThread()) {
        // Wait for present is called
        CSwapChain::GetMain().WaitForPresent();

        // Wait for GPU
        if (CCommandManager::GetMain().GetCommandQueue() != nullptr) {
            CCommandManager::GetAny().WaitForGPU();
        }
    }
}
