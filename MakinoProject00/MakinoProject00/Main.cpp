#include "ReleaseSingleton.h"
#include "UtilityForException.h"
#include "Application.h"
#include "ApplicationMain.h"
#include "EncryptAssetMain.h"
#ifdef _ENCRYPT_ASSET
#include <iostream>
#endif // _ENCRYPT_ASSET

#ifdef _DEBUG
// Define needed to check for memory leaks
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#endif

// Entry point
#ifdef _DEBUG
int main() {
#elif _ENCRYPT_ASSET
int main() {
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#endif
    
#ifdef _DEBUG
    // Check memory leak
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    std::setlocale(LC_ALL, "");

    // Initialize ID of the main thread
    Utl::CMainThreadChecker::Initialize();

#ifdef _ENCRYPT_ASSET
    try {
        // Main function for encrypting assets
        EncryptAssetMain();
    }
    catch (const Utl::Error::CFatalError& e) {
        std::wcout << L"Failure encryption of assets..." << std::endl;
        std::wcout << e.WhatW() << std::endl;
    }
#else
    try {
        // Main function for application
        ApplicationMain();
    }
    catch (const Utl::Error::CFatalError& e) {
        // Error processing
        HWND hwnd = CApplication::GetAny().GetWndHandle();
        if (hwnd != NULL) {
            MessageBox(hwnd, e.WhatW().c_str(), L"Error!", MB_OK | MB_ICONERROR);
        }
        else {
            OutputDebugString((L"Error! " + e.WhatW() + L"\n").c_str());
        }
    }
#endif // _ENCRYPT_ASSET

    // Release all singleton instances
    ACReleaseSingleton::AllRelease();

    return 0;
}
