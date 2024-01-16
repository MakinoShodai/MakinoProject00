#include "ContactColliderList.h"
#include "UtilityForException.h"
#include "ContactPoint.h"
#include "BVOverlapPair.h"

// Constructor for CContactPointWrapper
CContactPointWrapper::CContactPointWrapper(const Mkpe::ContactPoint* point, const CContactOpponent* opponent) 
    : m_point(point)
    , m_opponent(opponent)
{ }

// Get midpoint of contact point
Vector3f CContactPointWrapper::GetMidPoint() {
    if (m_point) {
        return m_point->pointWorldOnB + m_point->normalWorldOnB * (m_point->penetDepth * Utl::Inv::_2);
    }
    throw Utl::Error::CFatalError(L"Requesting a collision point for a wrapper that has no collision point");
}

// Get contact point of my collider
Vector3f CContactPointWrapper::GetMyPoint() {
    if (m_point) {
        if (m_opponent->IsNodeASelf()) {
            return m_point->pointWorldOnB + m_point->normalWorldOnB * m_point->penetDepth;
        }
        else {
            return m_point->pointWorldOnB;
        }
    }
    throw Utl::Error::CFatalError(L"Requesting a collision point for a wrapper that has no collision point");
}

// Get contact point of opponent collider
Vector3f CContactPointWrapper::GetOpponentPoint() {
    if (m_point) {
        if (m_opponent->IsNodeASelf()) {
            return m_point->pointWorldOnB;
        }
        else {
            return m_point->pointWorldOnB + m_point->normalWorldOnB * m_point->penetDepth;
        }
    }
    throw Utl::Error::CFatalError(L"Requesting a collision point for a wrapper that has no collision point");
}

// Constructor for CContactOpponent
CContactOpponent::CContactOpponent(CWeakPtr<ACCollider3D> opponent, const Mkpe::Dbvt::BVOverlapPair* pairInfo)
    : m_opponent(std::move(opponent))
    , m_pairInfo(pairInfo)
{ }

// Am I a node on the A side?
bool CContactOpponent::IsNodeASelf() const {
    return m_opponent->CheckMyLeafNodeID(m_pairInfo->leafID2);
}

// Get a contact point
CContactPointWrapper CContactOpponent::GetContactPoint(UINT index) const {
    return CContactPointWrapper(m_pairInfo->contactData.GetContactPoint(index), this);
}

// Construtor for CContactColliderList
Mkpe::CContactColliderList::CContactColliderList() 
    : m_currentFrameIndex(0) {
}

// Rebuild the lists of the contact colliders for each categories
void Mkpe::CContactColliderList::RebuildList() {
    // Compute index of previous frame contact list
    UINT prevIndex = (m_currentFrameIndex + 1) % 2;

    // Clear contact lists for each categories
    m_beginContactList.clear();
    m_hitContactList.clear();
    m_endContactList.clear();

    // Allocate to the contact list for during contact and the contact list for beginning of contact
    for (const auto& current : m_contactLists[m_currentFrameIndex]) {
        bool isFound = false;
        for (const auto& prev : m_contactLists[prevIndex]) {
            // If the collider that made contact with the current frame
            // also made contact with the previous frame, set the flag
            if (current == prev) {
                isFound = true;

                // Add to the contact list for during contact
                m_hitContactList.push_back(current);
                break;
            }
        }
        // If it's a collider that has begun to make contact with the current frame
        if (!isFound) {
            // Add to the contact list for beginning of contact
            m_beginContactList.push_back(current);
        }
    }

    // Allocate to the contact list for end of contact
    for (const auto& prev : m_contactLists[prevIndex]) {
        if (prev.GetOpponent() == nullptr) { continue; }

        auto it = std::find_if(m_contactLists[m_currentFrameIndex].begin(), m_contactLists[m_currentFrameIndex].end(),
            [&prev](const CContactOpponent& current) { return prev == current; });
        if (it == m_contactLists[m_currentFrameIndex].end()) {
            // Add to the contact list for end of contact
            m_endContactList.push_back(prev.GetOpponent());
        }
    }

    // Clear list of contact colliders in previous frame
    m_contactLists[prevIndex].clear();

    // Update index of current frame contact list
    m_currentFrameIndex = prevIndex;
}

// Clear the lists of all categories and the list of current and previous frames
void Mkpe::CContactColliderList::AllClear() {
    for (UINT i = 0; i < 2; ++i) {
        m_contactLists[i].clear();
    }
    m_beginContactList.clear();
    m_hitContactList.clear();
    m_endContactList.clear();
}
