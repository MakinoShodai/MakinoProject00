/**
 * @file   BVOverlapPair.h
 * @brief  This file handles pair of bounding volumes in contact each other.
 * 
 * @author Shodai Makino
 * @date   2023/07/19
 */

#ifndef __BV_OVERLAP_PAIR_H__
#define __BV_OVERLAP_PAIR_H__

#include "BVNode.h"
#include "IDResource.h"
#include "ContactPoint.h"
#include "StableHandleVector.h"

namespace Mkpe {
    namespace Dbvt {
        /** @brief Pair of bounding volumes in contact each other */
        struct BVOverlapPair {
            /** @brief First leaf node */
            NodeID leafID1;
            /** @brief Second leaf node */
            NodeID leafID2;
            /** @brief Contact point information for this pair */
            CContactData contactData;

            /**
               @brief Constructor
               @param accesor Accessor class that links IDs and nodes
               @param leafID1 First leaf node
               @param leafID2 Second leaf node
            */
            BVOverlapPair(IDNodeAccessor* accesor, NodeID leafID1, NodeID leafID2);

            /**
               @brief Returns the results of checking whether two colliders can overlap after approval each other as a contact opponent
               @return Can two colliders overlap?
               @details
               Call if the collision detection between two colliders is true.
               If true is returned, must not create a contact point
            */
            bool CanOverlapAfterApproval();

            /** @brief Get collider wrapper of the first leaf node */
            inline CColliderWrapper* GetWrapperA() const { return contactData.GetWrapperA(); }
            /** @brief Get collider wrapper of the second leaf node */
            inline CColliderWrapper* GetWrapperB() const { return contactData.GetWrapperB(); }

        public:
            /** @brief Allow the move constructor */
            BVOverlapPair(BVOverlapPair&&) = default;
            /** @brief Allow the move operator */
            BVOverlapPair& operator=(BVOverlapPair&&) = default;
            /** @brief Disallow the copy constructor */
            BVOverlapPair(const BVOverlapPair&) = delete;
            /** @brief Disallow the copy operator */
            BVOverlapPair& operator=(const BVOverlapPair&) = delete;
        };

        /** @brief Comparison operator structure */
        struct BVOverlapPairComparator {
            /** Comparison operator */
            inline bool operator()(const BVOverlapPair& a, const BVOverlapPair& b);
        };

        /** @brief Dynamic array of, pairs of bounding volumes in contact each other */
        using BVOverlapPairVector = std::vector<BVOverlapPair>;

        /** @brief Structure that manages pairs of bounding volumes in contact each other */
        class BVOverlapPairManager {
        public:
            /**
               @brief Constructor
               @param accesor Accessor class that links IDs and nodes
            */
            BVOverlapPairManager(IDNodeAccessor* accesor);

            /**
               @brief Destructor
            */
            ~BVOverlapPairManager() = default;

            /**
               @brief Merge the pairs from the current physics simulation, with the pairs from the previous physics simulation
            */
            void MergePairs();

            /**
               @brief Add a new pair
               @param leafID1 Fist leaf node
               @param leafID2 Second leaf node
            */
            void AddPair(NodeID leafID1, NodeID leafID2) { m_pairs.push_back(BVOverlapPair(m_idNode, leafID1, leafID2)); }

            /**
               @brief Update size of the pairs dynamic array at previous physics simulation
            */
            void UpdatePrevSize() { m_prevSize = static_cast<int>(m_pairs.size()); }

            /** @brief Get pointer to the dynamic array of, pairs of bounding volumes in contact each other */
            BVOverlapPairVector* GetPair() { return &m_pairs; }

        private:
            /** @brief Accessor class that links IDs and nodes */
            IDNodeAccessor* m_idNode;
            /** @brief Dynamic array of, pairs of bounding volumes in contact each other */
            BVOverlapPairVector m_pairs;
            /** @brief Size of pairs dynamic array at previous physics simulation */
            int m_prevSize;
        };
    } // namespace Dbvt

    /** @brief Structure from which only feature as key of BVOverlapPair is extracted */
    struct OverlapPairForKey {
        /** @brief First leaf node */
        Dbvt::NodeID leafID1;
        /** @brief Second leaf node */
        Dbvt::NodeID leafID2;

        /**
           @brief Constructor
           @param pair Variable with the NodeID to be keyed
        */
        inline OverlapPairForKey(const Dbvt::BVOverlapPair& pair) : leafID1(pair.leafID1), leafID2(pair.leafID2) {}

        /** @brief Comparison operator */
        inline bool operator==(const OverlapPairForKey& v) const {
            return leafID1 == v.leafID1 && leafID2 == v.leafID2;
        }
    };

    /** @brief Structure to generate hash */
    struct OverlapPairKeyHash {
        /** @brief Generating hash operator */
        inline std::size_t operator()(const OverlapPairForKey& v) const noexcept;
    };
} // namespace Mkpe

// Comparison operator
inline bool Mkpe::Dbvt::BVOverlapPairComparator::operator()(const BVOverlapPair& a, const Dbvt::BVOverlapPair& b) {
    // Descending order comparison
    return
        a.leafID1 > b.leafID1 ||
        (a.leafID1 == b.leafID1 && a.leafID2 > b.leafID2);
}

// Generating hash operator
inline std::size_t Mkpe::OverlapPairKeyHash::operator()(const OverlapPairForKey& v) const noexcept {
    std::size_t seed = 0;
    Utl::HashCombine(&seed, v.leafID1);
    Utl::HashCombine(&seed, v.leafID2);
    return seed;
}

#endif // !__BV_OVERLAP_PAIR_H__
