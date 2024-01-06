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
