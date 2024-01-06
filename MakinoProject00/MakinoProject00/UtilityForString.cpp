#include "UtilityForString.h"

// Convert 'std::string' to 'std::wstring' 
std::wstring Utl::Str::string2WString(const std::string& str) {
    // Early return
    if (str.empty()) {
        return std::wstring();
    }

    // Get required wide character size
    size_t charsSize;
    mbstowcs_s(&charsSize, nullptr, 0, str.c_str(), _TRUNCATE);

    // Converting strings
    std::vector<wchar_t> buffer(charsSize);
    mbstowcs_s(nullptr, buffer.data(), charsSize, str.c_str(), _TRUNCATE);

    // Return converted string
    return std::wstring(buffer.begin(), buffer.end() - 1);
}

// Convert 'std::wstring' to 'std::string' 
std::string Utl::Str::wstring2String(const std::wstring& wstr) {
    // Early return
    if (wstr.empty()) {
        return std::string();
    }

    // Get required multibyte character size
    size_t charsSize;
    wcstombs_s(&charsSize, nullptr, 0, wstr.c_str(), _TRUNCATE);

    // Converting strings
    std::vector<char> buffer(charsSize);
    wcstombs_s(nullptr, buffer.data(), charsSize, wstr.c_str(), _TRUNCATE);

    // Return converted string
    return std::string(buffer.begin(), buffer.end() - 1);
}

// Check if there is a string suffix of type char
bool Utl::Str::CheckSuffixChar(const char* str, const char* suffix) {
    size_t strLength = strlen(str);
    size_t suffixLength = strlen(suffix);

    if (strLength < suffixLength) {
        return false;
    }

    return strcmp(str + strLength - suffixLength, suffix) == 0;
}

// Extract string to a separate memory allocation
const char* Utl::Str::ExtractString(const char* src) {
    // Null check
    if (src == nullptr) { return nullptr; }

    // Get length of source string
    size_t length = strlen(src) + 1;
    
    // Allocate memory
    char* dest = new char[length];
    
    // Copy and return string
    strcpy_s(dest, length, src);
    return dest;
}
