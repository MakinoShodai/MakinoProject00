#include "BVOverlapPair.h"

// Constructor of BVOverlapPair class
Mkpe::Dbvt::BVOverlapPair::BVOverlapPair(IDNodeAccessor* accesor, NodeID leafID1, NodeID leafID2)
    : leafID1((leafID1 < leafID2) ? leafID1 : leafID2)
    , leafID2((leafID1 < leafID2) ? leafID2 : leafID1) 
    , contactData(accesor->GetResource(leafID1)->leafData.thisWrapper, accesor->GetResource(leafID2)->leafData.thisWrapper)
{
    // Vouch the sent nodes is leaf node
    assert(leafID1 && leafID2);
}

// Returns the results of checking whether two colliders can overlap after approval each other as a contact opponent
bool Mkpe::Dbvt::BVOverlapPair::CanOverlapAfterApproval() {
    // Get wrappers and colliders
    CColliderWrapper* wrapperA = GetWrapperA();
    CColliderWrapper* wrapperB = GetWrapperB();
    CWeakPtr<ACCollider3D> colliderA = wrapperA->GetCollider();
    CWeakPtr<ACCollider3D> colliderB = wrapperB->GetCollider();

    // Approval each other as a contact opponent
    wrapperA->AddContactedOpponent(colliderB, this);
    wrapperB->AddContactedOpponent(colliderA, this);

    // Check if each other's tag settings are overlap
    bool isOverlapAtoB = colliderA->CheckTagOverlap(colliderB.Get());
    bool isOverlapBtoA = colliderB->CheckTagOverlap(colliderA.Get());

    // If no collision points are needed for each other, return true
    if (colliderA->IsOverlap() || colliderB->IsOverlap() // Is one of them Overlap?
        || (isOverlapAtoB && isOverlapBtoA)              // Are the tag settings Overlap each other?
        || (isOverlapAtoB && wrapperB->IsStatic())       // A to B is Overlap and B is Static?
        || (isOverlapBtoA && wrapperA->IsStatic())) {    // B to A is Overlap and A is Static?
        return true;
    }

    // If these conditions are not met, return false
    return false;
}

// Constructor of BVOverlapPairManager class
Mkpe::Dbvt::BVOverlapPairManager::BVOverlapPairManager(IDNodeAccessor* accesor)
    : m_prevSize(0)
    , m_idNode(accesor) {
}

// Merge the pairs from the current physics simulation, with the pairs from the previous physics simulation
void Mkpe::Dbvt::BVOverlapPairManager::MergePairs() {
    // The number of pairs to be removed
    int    removePairNum = 0;
    // #TODO : removePairNum
    // Lambda expression to invalidate pairs
    auto invalidPairFunc = [&](BVOverlapPair& pair) {
        // Invalidate the pair
        pair.leafID1 = NULL;
        pair.leafID2 = NULL;

        // Add the number of pairs to be removed
        removePairNum++;
    };

    // Find pairs that need to be removed according to the condition from all previous pairs
    for (int i = 0; i < m_prevSize; ++i) {
        BVOverlapPair& pair = m_pairs[i];

        Node* leaf1 = m_idNode->GetResource(pair.leafID1);
        Node* leaf2 = m_idNode->GetResource(pair.leafID2);

        // If the nodes doesn't exist
        if ((leaf1 == nullptr || leaf2 == nullptr)) {
            // Invalidate the pair
            invalidPairFunc(pair);
            // Exit processing
            continue;
        }

        // If the nodes needs to be discarded
        if (leaf1->leafData.discardPairType == PairDiscardType::Discard || leaf2->leafData.discardPairType == PairDiscardType::Discard) {
            // Invalidate the pair
            invalidPairFunc(pair);
            // Exit processing
            continue;
        }

        // If the nodes needs to be checked
        if (leaf1->leafData.discardPairType == PairDiscardType::Check || leaf2->leafData.discardPairType == PairDiscardType::Check) {
            // If the nodes doesn't contact
            if (!leaf1->bv.IsIntersect(leaf2->bv)) {
                // Invalidate the pair
                invalidPairFunc(pair);
            }
        }
    }

    // Find pairs that need to be deleted due to duplication
    {
        // Sort in descending order
        std::sort(m_pairs.begin(), m_pairs.end(), BVOverlapPairComparator());

        // Rotate array to invalidate duplicate pairs
        NodeID prevLeafID1 = NULL;
        NodeID prevLeafID2 = NULL;
        for (auto& it : m_pairs) {
            // If it has already decided to discard, do nothing
            if ((it.leafID1 == NULL)) { break; }

            // If one previous pair and this pair are the same, they are duplicates
            if (prevLeafID1 == it.leafID1 && prevLeafID2 == it.leafID2) {
                // Invalidate the pair
                invalidPairFunc(it);
            }

            // Update one previous pair
            prevLeafID1 = it.leafID1;
            prevLeafID2 = it.leafID2;
        }

        // Sort in descending order, invalid pairs are brought to the back of the list
        std::sort(m_pairs.begin(), m_pairs.end(), BVOverlapPairComparator());


        // Remove pairs to be invalidated
        if (m_pairs.size() > removePairNum) {
            m_pairs.erase(m_pairs.end() - removePairNum, m_pairs.end());
        }
        else {
            m_pairs.clear();
        }
    }
}