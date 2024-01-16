/**
 * @file   ContactColliderList.h
 * @brief  This file handles the list of colliders contacted by the collider.
 * 
 * @author Shodai Makino
 * @date   2024/1/13
 */

#ifndef __CONTACT_COLLIDER_LIST_H__
#define __CONTACT_COLLIDER_LIST_H__

#include "Collider3D.h"

// Forward declaration
class CContactOpponent;
namespace Mkpe { namespace Dbvt { struct BVOverlapPair; } }
namespace Mkpe { struct ContactPoint; }

/** @brief Wrapped classes to make it easier for users to handle collision point */
class CContactPointWrapper {
public:
    /**
       @brief Constructor
       @param point Contact point
       @param opponent Information of contacted opponent by the collider
    */
    CContactPointWrapper(const Mkpe::ContactPoint* point, const CContactOpponent* opponent);

    /** @brief Does this class have a contact point? */
    bool IsHaving() { return (m_point != nullptr); }

    /**
       @brief Get midpoint of contact point
       @return Midpoint
    */
    Vector3f GetMidPoint();
    /**
       @brief Get contact point of my collider
       @return Contact point of my collider
    */
    Vector3f GetMyPoint();
    /**
       @brief Get contact point of opponent collider
       @return Contact point of opponent collider
    */
    Vector3f GetOpponentPoint();

private:
    /** @brief Contact point */
    const Mkpe::ContactPoint* m_point;
    /** @brief Information of contacted opponent by the collider */
    const CContactOpponent* m_opponent;
};

/**
   @brief Information of contacted opponent by the collider
   @details
   Lifetime of this class is 2 simulation steps.
   The second simulation step instance doesn't use pair information
*/
class CContactOpponent {
public:
    /**
       @brief Constructor
       @param opponent Collider of the contacted opponent
       @param pairInfo Pair information with the contacted opponent
    */
    CContactOpponent(CWeakPtr<ACCollider3D> opponent, const Mkpe::Dbvt::BVOverlapPair* pairInfo);

    /** @brief Copy constructor */
    CContactOpponent(const CContactOpponent& other) = default;

    /** @brief Destructor */
    ~CContactOpponent() = default;

    /**
       @brief Am I a node on the A side?
       @return Result
    */
    bool IsNodeASelf() const;

    /**
       @brief Get a contact point
       @param index Index of contact point to be get
       @return Class with wrapped a contact point
    */
    CContactPointWrapper GetContactPoint(UINT index) const;

    /** @brief Get reference to weak pointer to collider of the contacted opponent */
    const CWeakPtr<ACCollider3D>& GetOpponent() const { return m_opponent; }

    /** @brief Comparison operator */
    bool operator==(const CContactOpponent& other) const { return m_opponent == other.m_opponent; }
    /** @brief Comparison operator */
    bool operator!=(const CContactOpponent& other) const { return m_opponent != other.m_opponent; }

private:
    /** @brief Collider of the contacted opponent */
    CWeakPtr<ACCollider3D> m_opponent;
    /** @brief Pair information with the contacted opponent */
    const Mkpe::Dbvt::BVOverlapPair* m_pairInfo;
};

namespace Mkpe {
    /**
       @brief List class of the contact colliders for each categories
       @details
       # Category
       * Begin : Beginning of contact
       * Hit   : During contact
       * End   : End of contact
    */
    class CContactColliderList {
    public:
        /**
           @brief Constructor
        */
        CContactColliderList();

        /** @brief Destructor */
        ~CContactColliderList() = default;

        /**
           @brief Add information of a contacted opponent by the collider
           @param opponent Collider of the contacted opponent
           @param pairInfo Pair information with the contacted opponent
        */
        inline void AddOpponent(const CWeakPtr<ACCollider3D>& opponent, const Mkpe::Dbvt::BVOverlapPair* pairInfo) { m_contactLists[m_currentFrameIndex].emplace_back(opponent, pairInfo); }

        /**
           @brief Rebuild the lists of the contact colliders for each categories
        */
        void RebuildList();

        /**
           @brief Clear the lists of all categories and the list of current and previous frames
        */
        void AllClear();

        /** @brief Get list of information of the contact colliders for beginning of contact */
        const std::vector<CContactOpponent>& GetBeginList() const { return m_beginContactList; }
        /** @brief Get list of information of the contact colliders for during contact */
        const std::vector<CContactOpponent>& GetHitList() const { return m_hitContactList; }
        /** @brief Get list of weak pointer to the contact colliders for end of contact */
        const std::vector<CWeakPtr<ACCollider3D>>& GetEndList() const { return m_endContactList; }

    private:
        /** @brief Lists of information of the contact colliders for current and previous frames */
        std::vector<CContactOpponent> m_contactLists[2];
        /** @brief Index of current frame contact list */
        UINT m_currentFrameIndex;

        /** @brief List of information of the contact colliders for beginning of contact */
        std::vector<CContactOpponent> m_beginContactList;
        /** @brief List of information of the contact colliders for during contact */
        std::vector<CContactOpponent> m_hitContactList;
        /** @brief List of weak pointer to the contact colliders for end of contact */
        std::vector<CWeakPtr<ACCollider3D>> m_endContactList;
    };
} // namespace Mkpe

#endif // !__CONTACT_COLLIDER_LIST_H__
