/**
 * @file   AssetCrypter.h
 * @brief  This file handles asset encryption and decryption functions.
 * 
 * @author Shodai Makino
 * @date   2023/12/22
 */

#ifndef __ASSET_CRYPTER_H__
#define __ASSET_CRYPTER_H__

namespace AssetCrypter {
    /** @brief Function type for loading assets with decrypted data */
    using LoadFunctionType = std::function<bool(const void* data, size_t size, const std::wstring& fileName)>;

    /**
       @brief Load .pak file
       @param filePath Path of .pak file. Please specify the name without the extension
       @param function Function for loading assets with decrypted data
    */
    bool LoadPakFile(const std::wstring& filePath, LoadFunctionType function);

    /**
       @brief Create .pak file
       @param pakFileName Name of .pak file to be created. Please specify the name without the extension
       @param extensions Extensions to make .pak file
       @param dirPathAfterAssets Directory path after "AssetsPlain/"
    */
    void CreatePakFile(const std::wstring& pakFileName, std::initializer_list<std::string> extensions, const std::string& dirPathAfterAssets = "");
}

#endif // !__ASSET_CRYPTER_H__
