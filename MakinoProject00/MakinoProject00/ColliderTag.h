/**
 * @file   ColliderTag.h
 * @brief  This file handles tag for collider.
 * 
 * @author Shodai Makino
 * @date   2024/1/15
 */

#ifndef __COLLIDER_TAG_H__
#define __COLLIDER_TAG_H__

#include "UtilityForBit.h"
#include "Singleton.h"

/** @brief Type of collider tag */
using ColliderTagType = uint32_t;
/** @brief The number of collider tag */
const UINT COLLIDER_TAG_BITS_MAX = 4;

/** @brief Tag of collider */
enum class ColliderTag : ColliderTagType {
    /** @brief Player */
    Player = 1 << 1,
    /** @brief Enemy */
    Enemy = 1 << 2,
    /** @brief Terrain */
    Terrain = 1 << 3,
    /** @brief Unset */
    Empty = 1 << COLLIDER_TAG_BITS_MAX,
};
ENUM_TO_BITFLAG(ColliderTag); // Support for bit operation

/** @brief Kind of collider tag configuration */
enum class ColliderTagConfigKind {
    /** @brief Colliding with each other */
    Collide,
    /** @brief Overlapping with each other */
    Overlap,
    /** @brief A to B collide, B to A overlap */
    OneSideCollideOverlap,
};

/** @brief Registry for handling collider colliding partner tags */
class CColliderTagRegistry : public ACAnyThreadSingleton<CColliderTagRegistry> {
    // Friend declaration
    friend class ACSingletonBase;

public:
    /** @brief Destructor */
    ~CColliderTagRegistry() = default;

    /**
       @brief Set configure
       @param kind Kind of configuration
       @param tagA tag A
       @param tagB tag B
    */
    void SetConfig(ColliderTagConfigKind kind, ColliderTag tagA, ColliderTag tagB);

    /** @brief Does tag A collide or overlap with tag B? */
    inline bool IsEither(ColliderTag tagA, ColliderTag tagB) { return Utl::CheckEnumBit(m_collideMasks[tagA] & tagB) || Utl::CheckEnumBit(m_overlapMasks[tagA] & tagB); }
    /** @brief Does tag A collide with tag B? */
    inline bool IsCollide(ColliderTag tagA, ColliderTag tagB) { return Utl::CheckEnumBit(m_collideMasks[tagA] & tagB); }
    /** @brief Does tag A overlap with tag B? */
    inline bool IsOverlap(ColliderTag tagA, ColliderTag tagB) { return Utl::CheckEnumBit(m_overlapMasks[tagA] & tagB); }

    /** @brief Finish writing the value */
    void FinishWrite() { m_isFinishWrite = true; }

protected:
    /**
       @brief Constructor
    */
    CColliderTagRegistry();

private:
    /**
       @brief Set tagA to tagB to collide
    */
    void SetCollideTag(ColliderTag tagA, ColliderTag tagB);

    /**
       @brief Set tagA to tagB to overlap
    */
    void SetOverlapTag(ColliderTag tagA, ColliderTag tagB);

private:
    /** @brief Masks map for colliding tag */
    std::unordered_map<ColliderTag, ColliderTag> m_collideMasks;
    /** @brief Masks map for overlaping tag */
    std::unordered_map<ColliderTag, ColliderTag> m_overlapMasks;
    /** @brief Finish writing values? */
    bool m_isFinishWrite;
};

#endif // !__COLLIDER_TAG_H__
