#ifdef _ENCRYPT_ASSET
#include "EncryptAssetMain.h"
#include <iostream>
#include "AssetCrypter.h"
#include "UtilityForString.h"
#include "ModelRegistry.h"

// Main function for encrypting assets
void EncryptAssetMain() {
    // Create .pak file for texture
    AssetCrypter::CreatePakFile(L"Assets/textures", { ".png", ".jpg", ".tga" });

    // Create .pak file for model
    AssetCrypter::CreatePakFile(L"Assets/models", { ".fbx", }, Utl::Str::wstring2String(MODELASSET_DIR));

    // Create .pak file for animation
    AssetCrypter::CreatePakFile(L"Assets/anims", { ".fbx", }, Utl::Str::wstring2String(ANIMASSET_DIR));

    // Output successful message
    std::wcout << L"Successful encryption of assets!" << std::endl;
}

#endif // _ENCRYPT_ASSET
