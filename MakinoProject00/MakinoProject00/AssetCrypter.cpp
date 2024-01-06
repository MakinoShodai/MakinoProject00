#include "AssetCrypter.h"
#include "UtilityForString.h"
#include "CryptoKey.h"
#include "UtilityForException.h"

// File information
struct FileInfo {
    // File name
    std::string filename;
    // File size
    std::streamsize size;
    // File data
    std::vector<unsigned char> data;
};

// Character for identifying between binary data
const char IDENTIFYING_BINARY = '"';
// File path for plaintext assets
const char* const PLAINTEXT_ASSETS_DIR = "AssetsPlain/";
// The maximum number of asset per file
const int MAX_NUM_ASSET_PER_FILE = 10;

// @brief Encrypt data
// @param plainData Plaintext data
// @param outputData Return variable to receive encrypted data
void EncryptData(std::vector<unsigned char>& plainData, std::vector<unsigned char>& outputData) {
    // Initialize AES encryption object
    CryptoPP::AES::Encryption aesEncryption(AssetCrypter::KEY, CryptoPP::AES::DEFAULT_KEYLENGTH);
    // Initialize CBC mode encryption object. Set iv to initialize the first block
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, AssetCrypter::IV);

    // Stream transformation filter settings
    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption,
        new CryptoPP::VectorSink(outputData));
    // Pass plaintext data to stream transfomation filter
    stfEncryptor.Put(plainData.data(), plainData.size());
    // Encryption complete
    stfEncryptor.MessageEnd();
}

// @brief Decrypt data
// @param encryptedData Encrypted data
// @param outputData Return variable to receive decrypted data
void DecryptData(std::vector<unsigned char>& encryptedData, std::vector<unsigned char>& outputData) {
    // Initialize AES decryption object
    CryptoPP::AES::Decryption aesDecryption(AssetCrypter::KEY, CryptoPP::AES::DEFAULT_KEYLENGTH);
    // Initialize CBC mode decryption object. Set iv to the target of the first block operation
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, AssetCrypter::IV);

    // Stream transformation filter settings
    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::VectorSink(outputData));
    // Pass encrypted data to stream transfomation filter
    stfDecryptor.Put(encryptedData.data(), encryptedData.size());
    // Decryption complete
    stfDecryptor.MessageEnd();
}

// Load .pak file
bool AssetCrypter::LoadPakFile(const std::wstring& filePath, LoadFunctionType function) {
    int fileIndex = 1;
    while (true) {
        std::ifstream pakFile(filePath + std::to_wstring(fileIndex) + L".pak", std::ios::binary);

        // If pak file is not loadad
        if (!pakFile.is_open()) {
            // If If none of the files could be loaded, error processing and exit
            if (fileIndex == 1) {
                return false;
            }
            // If even one file is loaded, success!
            else {
                return true;
            }
        }

        std::vector<unsigned char> encryptedFileData;
        std::vector<unsigned char> decryptedFileData;
        while (true) {
            std::string name;
            std::string sizeStr;
            size_t size;

            // Continue to the end of the file
            pakFile.get();
            if (pakFile.eof()) { break; }

            // Read file name
            std::getline(pakFile, name, IDENTIFYING_BINARY);
            if (!pakFile.good()) { return false; }

            // Read string of file size
            std::getline(pakFile, sizeStr, IDENTIFYING_BINARY);
            if (!pakFile.good()) { return false; }

            // Convert string to file size and create data array
            size = static_cast<size_t>(std::stoi(sizeStr));
            encryptedFileData.resize(size);

            // Read file data
            pakFile.read(reinterpret_cast<char*>(encryptedFileData.data()), static_cast<std::streamsize>(size));
            if (!pakFile.good()) { return false; }

            // Decrypt data
            DecryptData(encryptedFileData, decryptedFileData);

            // Execute sent function
            if (!function(decryptedFileData.data(), decryptedFileData.size(), Utl::Str::string2WString(name))) {
                return false;
            }

            // Clear arrays
            encryptedFileData.clear();
            decryptedFileData.clear();
        }

        // Advance to the next file to be loaded
        fileIndex++;
    }

    return true;
}

// Create .pak file
void AssetCrypter::CreatePakFile(const std::wstring& pakFileName, std::initializer_list<std::string> extensions, const std::string& dirPathAfterAssets) {
    std::vector<FileInfo> files;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(PLAINTEXT_ASSETS_DIR + dirPathAfterAssets)) {
        // Get file path
        auto path = entry.path();

        // Check if the file extension is correct
        bool isTrueExtension = false;
        for (const auto& it : extensions) {
            if (path.extension() == it) {
                isTrueExtension = true;
                break;
            }
        }

        // If the file extension is not correct, advance to a next file
        if (false == isTrueExtension) {
            continue;
        }

        // Open the file, output an error message if it cannot be opened, and exit the function
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw Utl::Error::CFatalError(L"Failed to open file : " + path.wstring());
        }

        // Set relative path as file name
        FileInfo info;
        info.filename = std::filesystem::relative(path, PLAINTEXT_ASSETS_DIR).string();
        info.size = file.tellg();

        // Replace all backslashes with forward slashes in the file path
        std::replace(info.filename.begin(), info.filename.end(), '\\', '/');

        // Move the read position of the file to the beginning
        file.seekg(0, std::ios::beg);
        
        // Read file
        std::vector<unsigned char> plain;
        plain.resize(static_cast<size_t>(info.size));
        file.read(reinterpret_cast<char*>(plain.data()), info.size);

        // Encrypt data
        EncryptData(plain, info.data);

        // Correct file size to encrypted
        info.size = static_cast<std::streamsize>(info.data.size());
        
        // Add file information
        files.emplace_back(std::move(info));
    }

    // Write data to the file for outputing
    std::string tmp;
    size_t fileSize = files.size();
    size_t fileIndex = 1;
    // Open a file for outputing
    std::ofstream outputFile(pakFileName + std::to_wstring(fileIndex) + L".pak", std::ios::binary);
    if (!outputFile.is_open()) {
        throw Utl::Error::CFatalError(L"Failed to open file :" + pakFileName + std::to_wstring(fileIndex) + L".pak");
    }
    // Write all files
    for (size_t i = 0; i < fileSize; ++i) {
        auto& file = files[i];

        // Write character for identifying
        outputFile.write(&IDENTIFYING_BINARY, static_cast<std::streamsize>(1));

        // Write file name and character for identifying
        outputFile.write(file.filename.c_str(), static_cast<std::streamsize>(file.filename.length()));
        outputFile.write(&IDENTIFYING_BINARY, static_cast<std::streamsize>(1));

        // Write file size and character for identifying
        tmp = (std::to_string(file.size));
        outputFile.write(tmp.c_str(), static_cast<std::streamsize>(tmp.length()));
        outputFile.write(&IDENTIFYING_BINARY, static_cast<std::streamsize>(1));

        // Write file data
        outputFile.write(reinterpret_cast<char*>(file.data.data()), file.size);
        
        // If the number of files written exceeds the maximum number of asset per file
        if (i >= fileIndex * MAX_NUM_ASSET_PER_FILE && i + 1 < fileSize) {
            // Advance to the next file to be written
            fileIndex++;
            outputFile = std::ofstream(pakFileName + std::to_wstring(fileIndex) + L".pak", std::ios::binary);
            if (!outputFile.is_open()) {
                throw Utl::Error::CFatalError(L"Failed to open file :" + pakFileName + std::to_wstring(fileIndex) + L".pak");
            }
        }
    }
}
