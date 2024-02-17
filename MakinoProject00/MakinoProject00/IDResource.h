/**
 * @file   IDResource.h
 * @brief  This file handles class that links IDs and resources.
 * 
 * @author Shodai Makino
 * @date   2023/07/19
 */

#ifndef __ID_RESOURCE_H__
#define __ID_RESOURCE_H__

/** @brief Unique ID type */
using UniqueID = uint64_t;
/** @brief Index part of unique ID (lower bits) */
using UniqueID_Index = uint32_t;
/** @brief Version part of unique ID (upper bits) */
using UniqueID_Version = uint32_t;

/** @brief Number of bits in the index part of unique ID */
constexpr uint32_t UNIQUE_ID_INDEX_BITNUM = 32;
/** @brief Bit mask of the index part of unique ID */
constexpr uint32_t UNIQUE_ID_INDEX_BITMASK = 0xFFFFFFFF;

/** @brief Class that links IDs and resources with the feature for get resources */
template <class Resource>
class CIDResourceAccessor {
public:
    /**
       @brief Get the resource
       @param id ID for the resource
       @return Resource
    */
    inline Resource* GetResource(UniqueID id);

protected:
    /**
       @brief Constructor
       @param startIndex Start Index
    */
    CIDResourceAccessor(UniqueID_Index startIndex) : m_resourceArray(startIndex, nullptr), m_nextIndex(startIndex) {}
    
    /**
       @brief Destructor
    */
    ~CIDResourceAccessor() = default;

protected:
    /** @brief Dynamic array of resources */
    std::vector<Resource*> m_resourceArray;
    /** @brief Next index */
    UniqueID_Index m_nextIndex;
};

/** @brief Class that links IDs and resources with the feature for generating IDs and linking them to resources  */
template <class Resource>
class CIDResourceFactory : public CIDResourceAccessor<Resource> {
public:
    /**
       @brief Constructor
       @param startIndex Start index
    */
    CIDResourceFactory(UniqueID_Index startIndex = 0);

    /**
       @brief Destructor
    */
    ~CIDResourceFactory() = default;

    /**
       @brief Generate ID
       @return Generated ID
    */
    UniqueID GenerateID();

    /**
       @brief Link ID to resource
       @param id ID to be linked
       @param res Resource to be linked
    */
    void LinkIDResouce(UniqueID id, Resource* res);

    /**
       @brief Release ID
       @param id ID to be released
    */
    void ReleaseID(UniqueID id);

private:
    /** @brief Queue of released unique IDs */
    std::queue<UniqueID> m_freeID;
};

// Get the resource
template<class Resource>
inline Resource* CIDResourceAccessor<Resource>::GetResource(UniqueID id) {
    // Get the index part of ID
    UniqueID_Index index = static_cast<UniqueID_Index>(id);

    // If it's an ungenerated ID, return nullptr
    if (index >= m_nextIndex) { return nullptr; }

    // Return a resource
    return m_resourceArray[index];
}

// Factory class's constructor
template<class T>
CIDResourceFactory<T>::CIDResourceFactory(UniqueID_Index startIndex)
    : CIDResourceAccessor<T>(startIndex)
{ }

// Generate ID
template <class Resource>
UniqueID CIDResourceFactory<Resource>::GenerateID() {
    UniqueID uniqueID;
    // If a released IDs exists, use it
    if (!m_freeID.empty()) {
        // Get the oldest ID
        uniqueID = m_freeID.front();
        m_freeID.pop();

        // Extract the version part from the got ID and add the version
        UniqueID_Version version = static_cast<UniqueID_Version>(uniqueID >> UNIQUE_ID_INDEX_BITNUM);
        version++;

        // Merge the version part and the index part
        uniqueID = (static_cast<UniqueID>(version) << UNIQUE_ID_INDEX_BITNUM) | (uniqueID & UNIQUE_ID_INDEX_BITMASK);
    }
    else {
        // Assign the next unique ID
        uniqueID = static_cast<UniqueID>(this->m_nextIndex);

        // If the maximum index value is exceeded, an exception is handled
        if (this->m_nextIndex >= UNIQUE_ID_INDEX_BITMASK) {
            throw std::overflow_error("The index of the IDResource class has exceeded the limit of the index part.");
        }

        // Extend array by one
        this->m_resourceArray.emplace_back(nullptr);

        // Add the next index
        this->m_nextIndex++;
    }

    return uniqueID;
}

// Link ID to resource
template<class Resource>
inline void CIDResourceFactory<Resource>::LinkIDResouce(UniqueID id, Resource* res) {
    // Get the index part of ID
    UniqueID_Index index = static_cast<UniqueID_Index>(id);

    // If it's an ungenerated ID, do nothing
    if (index >= this->m_nextIndex) { return; }

    // Link a resource to ID
    this->m_resourceArray[index] = res;
}

// Release ID
template <class Resource>
void CIDResourceFactory<Resource>::ReleaseID(UniqueID id) {
    // Get the index part of ID
    UniqueID_Index index = static_cast<UniqueID_Index>(id);

    // If it's an ungenerated ID, do nothing
    if (index >= this->m_nextIndex) { return; }

    // Change the specified ID resource in the array to nullptr
    this->m_resourceArray[index] = nullptr;
    
    // Add the released IDs queue
    m_freeID.push(id);
}

#endif // !__ID_RESOURCE_H__
