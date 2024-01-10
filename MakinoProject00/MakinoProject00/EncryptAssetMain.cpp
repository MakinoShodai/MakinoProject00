#ifdef _ENCRYPT_ASSET
#include "EncryptAssetMain.h"
#include <iostream>
#include "AssetCrypter.h"
#include "UtilityForString.h"
#include "ModelRegistry.h"

// Main function for encrypting assets
void EncryptAssetMain() {
    // Create .pak file for texture
    AssetCrypter::CreatePakFile(PAK_FILE_TEXTURES, { ".png", ".jpg", ".tga" });

    // Create .pak file for model
    AssetCrypter::CreatePakFile(PAK_FILE_MODELS, { ".fbx", }, Utl::Str::wstring2String(MODELASSET_DIR));

    // Create .pak file for animation
    AssetCrypter::CreatePakFile(PAK_FILE_ANIMATIONS, { ".fbx", }, Utl::Str::wstring2String(ANIMASSET_DIR));

    // Output successful message
    std::wcout << L"Successful encryption of assets!" << std::endl;
}

#endif // _ENCRYPT_ASSET
