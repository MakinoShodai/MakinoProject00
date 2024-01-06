#include "DebugForApplicationFramework.h"

const wchar_t* const PIX_DLL_NAME = L"WinPixGpuCapturer.dll"; // Name of Pix dll

#ifdef _DEBUG
// Enable directX debug layer
void Dbg::EnableDirectXDebugLayer() {
    // Create debug layer interface
    ID3D12Debug* debugLayer = nullptr;
    HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));

    // Enable and release debug layer
    debugLayer->EnableDebugLayer();
    debugLayer->Release();
}
#endif // _DEBUG

#ifdef _DEBUG
// Loads the dll required to use Microsoft Pix
void Dbg::LoadMicrosoftPixDLL() {
    // If the dll is already loaded, exit
    if (GetModuleHandle(PIX_DLL_NAME) != NULL) {
        return;
    }

    // Variable declarations
    WIN32_FIND_DATA findData;                           // File data is found by FindXXFile Function
    bool            foundDirectory = false;             // Has the directory not been found yet?
    wchar_t         latestVersionDirectory[MAX_PATH];   // Path of latest version directory

    // Get path of Program Files
    LPWSTR programFilesPath = nullptr;
    SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);

    // Build path of Microsoft Pix
    std::wstring pixSearchPath = programFilesPath + std::wstring(L"\\Microsoft PIX\\*");

    // Retrieve the folder that in exist "...\\ProgramFiles\\Microsoft PIX" one by one
    HANDLE hFind = FindFirstFile(pixSearchPath.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) // If the retrieved path is a directory
                && (findData.cFileName[0] != '.')) { // Filtering special entries
                // The version directory has not been found yet, or found a newer version directory
                if (!foundDirectory || wcscmp(latestVersionDirectory, findData.cFileName) <= 0) {
                    foundDirectory = true; // Found directory
                    StringCchCopy(latestVersionDirectory, _countof(latestVersionDirectory), findData.cFileName);
                }
            }
        } while (FindNextFile(hFind, &findData) != FALSE); // Get next file
    }

    // Close file
    FindClose(hFind);

    // If the path of Pix has not been found, exit
    if (!foundDirectory) {
        // # TODO: Error processing
        return;
    }

    // Merge path
    pixSearchPath.resize(pixSearchPath.size() - 1); // Delete "*"
    pixSearchPath += latestVersionDirectory;        // Add the path of latest version directory
    pixSearchPath += L"\\WinPixGpuCapturer.dll";    // Add "\\WinPixGpuCapturer.dll"

    // Load dll
    LoadLibrary(pixSearchPath.c_str());
}
#endif // _DEBUG

