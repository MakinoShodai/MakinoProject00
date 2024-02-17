/**
 * @file   UtilityForFile.h
 * @brief  This file defines utility functions that are useful for file operations.
 * 
 * @author Shodai Makino
 * @date   2024/1/27
 */

#ifndef __UTILITY_FOR_FILE_H__
#define __UTILITY_FOR_FILE_H__

namespace Utl {
    namespace File {
        /**
           @brief Get all matching file extensions in the specified directory
           @param ret Return variable to store the matching filename
           @param extensions Extensions to be specified
           @param directory Directory to be specified
           @param Remove Extensions from the file name?
           @param isRemoveExtensions Remove the extension from the file name?
           @return Found even one file that matched?
        */
        bool GetMatchExtensionInDirectory(std::vector<std::string>* ret, std::initializer_list<std::string> extensions, const std::string& directory, bool isRemoveExtension);

    } // namespace File
} // namespace Utl

#endif // !__UTILITY_FOR_FILE_H__
