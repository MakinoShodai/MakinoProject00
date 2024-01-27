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

    /**
       @brief Is there currently no key assignment?
       @return Result
    */
    bool IsNotKeyAssignment();

private:
    /** @brief Value of a next key to be generated */
    IntKey m_nextKey;
    /** @brief Keys already released */
    std::queue<IntKey> m_releasedKeys;
};

/** @brief This class generates keys of integer numeric type (Key numbers are sorted in ascending order) */
class CPriorityIntKeyGenerater {
public:
    /** @brief Disallow the copy constructor */
    CPriorityIntKeyGenerater(const CIntKeyGenerater& other) = delete;
    /** @brief Disallow the copy assignment operator */
    CPriorityIntKeyGenerater& operator=(const CIntKeyGenerater& other) = delete;
    /** @brief Disallow the move constructor */
    CPriorityIntKeyGenerater(CIntKeyGenerater&& other) = delete;
    /** @brief Disallow the move assignment operator */
    CPriorityIntKeyGenerater& operator=(CIntKeyGenerater&& other) = delete;

    /**
       @brief Constructor
       @param initKey Key initial value
    */
    CPriorityIntKeyGenerater(IntKey initKey = 0) : m_nextKey(initKey) {}

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

    /**
       @brief Generate a key that doesn't duplicate an already existing key and is greater than the key sent
       @param key Key sent
       @return A generated key
    */
    IntKey GenerateNotDuplicateKey(IntKey key);

    /**
       @brief Is there currently no key assignment?
       @return Result
    */
    bool IsNotKeyAssignment();

    /**
       @brief Register the key you sent as a used key
       @param key Key to be registered
    */
    void RegisterUsedKey(IntKey key);

private:
    /** @brief Value of a next key to be generated */
    IntKey m_nextKey;
    /** @brief Keys already released */
    std::set<IntKey> m_releasedKeys;
};

/** @brief String with int key */
class CStringWithIntKey {
public:
    /**
       @brief Constructor
       @param str String to be set
    */
    CStringWithIntKey(std::string str);

    /** @brief Get string with int key */
    std::string GetWithIntKey() const { return (m_intKey != 0) ? m_str + " (" + std::to_string(m_intKey) + ')' : m_str; }

    /** @brief Get unkeyed string */
    const std::string& GetUnKeyedString() const { return m_str; }

    /** @brief Get int key */
    IntKey GetIntKey() const { return m_intKey; }

    /** @brief Set int key */
    void SetIntKey(IntKey key) { m_intKey = key; }

private:
    /** @brief String */
    std::string m_str;
    /** @brief Int key */
    IntKey m_intKey;
};

#endif // !__INT_KEY_GENERATER_H__
