/**
 * @file   IntKeyGenerater.h
 * @brief  This file handles class that generates keys of integer numeric type.
 * 
 * @author Shodai Makino
 * @date   2023/12/13
 */

#ifndef __INT_KEY_GENERATER_H__
#define __INT_KEY_GENERATER_H__

/** @brief Key of integer numeric type */
using IntKey = UINT32;

/** @brief This class generates keys of integer numeric type */
class CIntKeyGenerater {
public:
    /** @brief Disallow the copy constructor */
    CIntKeyGenerater(const CIntKeyGenerater& other) = delete;
    /** @brief Disallow the copy assignment operator */
    CIntKeyGenerater& operator=(const CIntKeyGenerater& other) = delete;
    /** @brief Disallow the move constructor */
    CIntKeyGenerater(CIntKeyGenerater&& other) = delete;
    /** @brief Disallow the move assignment operator */
    CIntKeyGenerater& operator=(CIntKeyGenerater&& other) = delete;

    /**
       @brief Constructor
       @param initKey Key initial value
    */
    CIntKeyGenerater(IntKey initKey = 0) : m_nextKey(initKey) {}

    /**
       @brief Generate a key
       @return A generated key
    */
    IntKey GenerateKey();

    /**
       @brief Release a key
       @param key Key to be released
    */
    void ReleaseKey(IntKey key);

private:
    /** @brief Value of a next key to be generated */
    IntKey m_nextKey;
    /** @brief Keys already released */
    std::queue<IntKey> m_releasedKeys;
};

#endif // !__INT_KEY_GENERATER_H__
