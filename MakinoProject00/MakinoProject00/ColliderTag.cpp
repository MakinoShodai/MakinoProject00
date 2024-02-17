#include "ColliderTag.h"
#include "UtilityForException.h"

// Set configure
void CColliderTagRegistry::SetConfig(ColliderTagConfigKind kind, ColliderTag tagA, ColliderTag tagB) {
    if (m_isFinishWrite) { return; }

    switch (kind) {
    case ColliderTagConfigKind::Collide:
        SetCollideTag(tagA, tagB);
        if (tagA != tagB) {
            SetCollideTag(tagB, tagA);
        }
        break;
    case ColliderTagConfigKind::Overlap:
        SetOverlapTag(tagA, tagB);
        if (tagA != tagB) {
            SetOverlapTag(tagB, tagA);
        }
        break;
    case ColliderTagConfigKind::OneSideCollideOverlap:
        if (tagA == tagB) {
            throw Utl::Error::CFatalError(L"Trying to use Collide and Overlap for the same tags");
        }
        SetCollideTag(tagA, tagB);
        SetOverlapTag(tagB, tagA);
        break;
    }
}

// Set tagA to tagB to collide
void CColliderTagRegistry::SetCollideTag(ColliderTag tagA, ColliderTag tagB) {
    if (m_isFinishWrite) { return; }
    m_collideMasks[tagA] |= tagB;

    // Check for duplication
    if (Utl::CheckEnumBit(m_overlapMasks[tagA] & tagB)) {
        throw Utl::Error::CFatalError(L"This tag is already set in overlap");
    }
}

// Set tagA to tagB to overlap
void CColliderTagRegistry::SetOverlapTag(ColliderTag tagA, ColliderTag tagB) {
    if (m_isFinishWrite) { return; }
    m_overlapMasks[tagA] |= tagB;

    // Check for duplication
    if (Utl::CheckEnumBit(m_collideMasks[tagA] & tagB)) {
        throw Utl::Error::CFatalError(L"This tag is already set in collide");
    }
}

// Constructor
CColliderTagRegistry::CColliderTagRegistry() 
    : ACAnyThreadSingleton(0)
    , m_isFinishWrite(false) {

    // Construct the value elements that all ColliderTag's have
    for (ColliderTagType i = 0; i < (ColliderTagType)COLLIDER_TAG_BITS_MAX; ++i) {
        ColliderTag tag = static_cast<ColliderTag>(1 << i);
        m_collideMasks[tag];
        m_overlapMasks[tag];
    }
}
