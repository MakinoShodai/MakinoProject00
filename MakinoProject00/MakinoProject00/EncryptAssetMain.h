/**
 * @file   EncryptAssetMain.h
 * @brief  This file defines main function for encrypting assets.
 * 
 * @author Shodai Makino
 * @date   2023/12/22
 */

#ifndef __ENCRYPT_ASSET_MAIN_H__
#define __ENCRYPT_ASSET_MAIN_H__

/** @brief Name of .pak file for textures */
const std::wstring PAK_FILE_TEXTURES = L"Assets/textures";
/** @brief Name of .pak file for models */
const std::wstring PAK_FILE_MODELS = L"Assets/models";
/** @brief Name of .pak file for animations */
const std::wstring PAK_FILE_ANIMATIONS = L"Assets/anims";

#ifdef _ENCRYPT_ASSET
/** @brief Main function for encrypting assets */
void EncryptAssetMain();
#endif // _ENCRYPT_ASSET

#endif // !__ENCRYPT_ASSET_MAIN_H__
