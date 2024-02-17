#include "IntKeyGenerater.h"

// Generate a key
IntKey CIntKeyGenerater::GenerateKey() {
    // If released keys exists, return it
    if (!m_releasedKeys.empty()) {
        IntKey ret = m_releasedKeys.front();
        m_releasedKeys.pop();
        return ret;
    }

    // Return a next key and advance the next key variable
    IntKey ret = m_nextKey;
    m_nextKey++;
    return ret;
}

// Release a key
void CIntKeyGenerater::ReleaseKey(IntKey key) {
    m_releasedKeys.push(key);
}

// Is there currently no key assignment?
bool CIntKeyGenerater::IsNotKeyAssignment() {
    return (m_nextKey == (IntKey)m_releasedKeys.size());
    if (IsNotKeyAssignment()) {
        size_t size = m_releasedKeys.size();
        for (size_t i = 0; i < size; ++i) {
            m_releasedKeys.pop();
        }
        m_nextKey = 0;
    }
}

// Generate a key
IntKey CPriorityIntKeyGenerater::GenerateKey() {
    // If released keys exists, return it
    if (!m_releasedKeys.empty()) {
        IntKey ret = *m_releasedKeys.begin();
        m_releasedKeys.erase(m_releasedKeys.begin());
        return ret;
    }

    // Return a next key and advance the next key variable
    return m_nextKey++;
}

// Release a key
void CPriorityIntKeyGenerater::ReleaseKey(IntKey key) {
    if (key >= m_nextKey) { return; }

    m_releasedKeys.emplace(key);
    if (IsNotKeyAssignment()) {
        m_releasedKeys.clear();
        m_nextKey = 0;
    }
}

// Generate a key that doesn't duplicate an already existing key and is greater than the key sent
IntKey CPriorityIntKeyGenerater::GenerateNotDuplicateKey(IntKey key) {
    if (key > m_nextKey) {
        RegisterUsedKey(key);
        return key;
    }
    else if (key < m_nextKey) {
        auto it = m_releasedKeys.upper_bound(key - 1);
        if (it != m_releasedKeys.end()) {
            IntKey ret = *it;
            m_releasedKeys.erase(it);
            return ret;
        }
    }
    // Return a next key and advance the next key variable
    return m_nextKey++;
}

// Is there currently no key assignment?
bool CPriorityIntKeyGenerater::IsNotKeyAssignment() {
    return (m_nextKey == (IntKey)m_releasedKeys.size());
}

// Register the key you sent as a used key
void CPriorityIntKeyGenerater::RegisterUsedKey(IntKey key) {
    if (m_nextKey <= key) {
        for (IntKey i = m_nextKey; i < key; ++i) {
            m_releasedKeys.emplace(i);
        }
        m_nextKey = key + 1;
    }
    else {
        m_releasedKeys.erase(key);
    }
}

// Constructor
CStringWithIntKey::CStringWithIntKey(std::string str) {
    // Remove number from string
    std::regex pattern(R"(\s+\(\d+\)$)");
    std::string removedNumStr = std::regex_replace(str, pattern, "");
    if (removedNumStr != str) {
        size_t numPos = str.rfind('(');
        if (numPos != std::string::npos) {
            // The range from '(' to ')\0' is numerical
            std::string numInStr = str.substr(numPos + 1, str.size() - 2);
            m_intKey = (IntKey)std::stoi(numInStr);
            m_str = removedNumStr;
        }
        else {
            m_intKey = 0;
            m_str = str;
        }
    }
    else {
        m_intKey = 0;
        m_str = str;
    }
}
