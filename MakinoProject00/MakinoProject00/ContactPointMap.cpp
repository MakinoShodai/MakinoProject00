#include "ContactPointMap.h"

// Build and get reference to contact data with exclusion control
Mkpe::CContactData& Mkpe::CContactPointMap::CThreadSafeFeature::BuildContactData(const Dbvt::BVOverlapPair& pair) {
    // Acquire a lock
    std::lock_guard<std::mutex> lock(m_owner->m_emplaceMutex);

    // Add new element and return
    return m_owner->m_map.try_emplace(OverlapPairForKey(pair), pair.GetWrapperA()->WeakFromThis(), pair.GetWrapperB()->WeakFromThis()).first->second;
}

// Get contact data
Mkpe::CContactData* Mkpe::CContactPointMap::CThreadSafeFeature::GetContactData(const Dbvt::BVOverlapPair& pair) {
    auto it = m_owner->m_map.find(OverlapPairForKey(pair));
    return (it != m_owner->m_map.end()) ? &it->second : nullptr;
}
