/**
 * @file   DynamicBVTree.h
 * @brief  This file handles dynamic bounding volume tree.
 * 
 * @author Shodai Makino
 * @date   2023/07/18
 */

#ifndef __DYNAMIC_BV_TREE_H__
#define __DYNAMIC_BV_TREE_H__

#include "Collider3D.h"
#include "RigidBody.h"
#include "BVNode.h"
#include "IDResource.h"
#include "BVOverlapPair.h"
#include "StableHandleVector.h"

namespace Mkpe {
    namespace Dbvt {
        /** @brief Dynamic bounding volume tree */
        class CDynamicBVTree {
        public:
            /**
               @brief Constructor
            */
            CDynamicBVTree();
            /**
               @brief Destructor
            */
            ~CDynamicBVTree() = default;

            /**
               @brief Initialize
            */
            void Initialize();

            /**
               @brief Re-align the tree from 0
               @param type Type of tree to be re-aligned
            */
            void RealignmentTree(TREETYPE type);
            /**
               @brief Add a new leaf node to the tree
               @param wrapperHandle Collider wrapper to be added
               @param specifyBV If bounding volume is not internally generated, specify it as an argument
            */
            void AddLeafNode(CColliderWrapper* wrapper, std::optional<BoundingVolume> specifyBV = std::nullopt);
            /**
               @brief Remove a leaf node from the tree
               @param wrapperHandle Collider wrapper to be removed
            */
            void RemoveLeafNode(CColliderWrapper* wrapper);
            /**
               @brief Reinsert a leaf node that already exists in the tree
               @param wrapper Collider wrapper to be reinserted
               @param specifyBV If bounding volume is not internally generated, specify it as an argument
               @return Reinserted new leaf node
               @attention 
               The reinsertion of a leaf node must be performed with this function, otherwise the ID transfer will not occur
               The leaf node sent to this function becomes invalid, and the returned leaf node is the new node
            */
            Node* ReInsertLeafNode(CColliderWrapper* wrapper, std::optional<BoundingVolume> specifyBV = std::nullopt);

            /**
               @brief Perform collision detection at node and node, creating potentially colliding pairs.
               @param nodeA first node
               @param nodeB second node
            */
            void NodeCollisionDetection(const Node* nodeA, const Node* nodeB);

            /** @brief Get amount of margin to add to the bounding volume of a dynamic object */
            const Vector3f& GetBVMargin() { return m_bvMargin; }
            /** @brief Set amount of margin to add to the bounding volume of a dynamic object */
            void SetBVMargin(const Vector3f& margin) { m_bvMargin = margin; }

            /** @brief Get root node in the specified tree type */
            const Node* GetRootNode(TREETYPE treetype) { return m_tree[treetype].rootNode; }

            /** @brief Get accessor class that links IDs and nodes */
            IDNodeAccessor* GetIDResouce() { return &m_idResourceFactory; }

            /** @brief Get the manager for potentially colliding pairs */
            BVOverlapPairManager* GetPairManager() { return &m_bvPairManager; }

        private:
            /**
               @brief Assign the closest BV to each BV
               @param remains Remaining Node queue
               @return Minimum cost pair
            */
            NodeCost* AssignNearBV(std::vector<NodeCost>& remains);
            /**
               @brief Calculation when adding a new internal node
               @param remains Remaining Node queue
               @param newNode New internal node
               @return Minimum cost pair
            */
            NodeCost* CalculationAddingInternalNode(std::vector<NodeCost>& remains, Node* newNode);
            /**
               @brief Perform branch rotation operation to minimize cost for all ancestor nodes of the specified node
               @param tree Tree to be operated
               @param node Specified node
            */
            void RotateTreeForAncestors(Tree& tree, Node* node);
            /**
               @brief Find the appropriate sibling to insert the specified node into the tree
               @param tree Tree to be operated
               @param newNode Specified new node
               @return Appropriate sibling
            */
            Node* FindGoodSibling(const Tree& tree, Node* newNode);

        private:
            /** @brief Manager for potentially colliding pairs */
            BVOverlapPairManager m_bvPairManager;
            /** @brief Transfer ID */
            std::optional<NodeID> m_handoverID;
            /** @brief Factory class that links IDs and nodes */
            IDNodeFactory m_idResourceFactory;
            /** @brief Various trees */
            Tree m_tree[TREETYPE_MAX];
            /** @brief Amount of margin to add to the bounding volume of a dynamic object */
            Vector3f m_bvMargin;
            /** @brief Have you completed initialization? */
            bool m_isInitialize;
        };
    } // namespace Dbvt
} // namespace Mkpe

#endif // !__DYNAMIC_BV_TREE_H__
