/**
 * @file   UtilityForString.h
 * @brief  This file defines utility functions that handle strings.
 * 
 * @author Shodai Makino
 * @date   2023/11/7
 */

#ifndef __UTILITY_FOR_STRING_H__
#define __UTILITY_FOR_STRING_H__

namespace Utl {
    namespace Str {
        /**
           @brief Convert 'std::string' to 'std::wstring' 
           @param str Multibyte string to be converted
        */
        std::wstring string2WString(const std::string& str);

        /**
           @brief Convert 'std::wstring' to 'std::string' 
           @param wstr Wide string to be converted 
        */
        std::string wstring2String(const std::wstring& wstr);

        /**
           @brief Check if there is a string suffix of type char
           @param str String to be checked
           @param suffix Suffix
        */
        bool CheckSuffixChar(const char* str, const char* suffix);

        /**
           @brief Extract string to a separate memory allocation
           @param src Source string
           @return Extracted string
           @attention You must always release the returned string using 'delete[]'
        */
        const char* ExtractString(const char* src);
    } // namespace Str
} // namespace Utl

#endif // !__UTILITY_FOR_STRING_H__
