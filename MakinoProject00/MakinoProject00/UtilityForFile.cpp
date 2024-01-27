#include "UtilityForFile.h"

// Get all matching file extensions in the specified directory
bool Utl::File::GetMatchExtensionInDirectory(std::vector<std::string>* ret, 
    std::initializer_list<std::string> extensions, const std::string& directory, 
    bool isRemoveExtension) {
    // Clear vector
    ret->clear();

    bool isOne = false;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
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

        std::string fileName = path.filename().string();
        if (isRemoveExtension) {
            fileName = fileName.substr(0, fileName.find_last_of('.'));
        }

        ret->push_back(fileName);
        isOne = true;
    }
    return isOne;
}
