/**
 * @file   BVNode.h
 * @brief  This file handles information needed for the dynamic bouding volume tree.
 * 
 * @author Shodai Makino
 * @date   2023/07/18
 */

#ifndef __BV_NODE_H__
#define __BV_NODE_H__

#include "BoudingVolume.h"
#include "IDResource.h"
#include "StableHandleVector.h"

namespace Mkpe {
    // Forward declaration
    class CColliderWrapper;

    namespace Dbvt {
        // Forward declaration
        struct Node;

        /** @brief Node ID. internal node ID is 0 */
        using NodeID = UniqueID;
        // Accessor class that links IDs and nodes
        using IDNodeAccessor = CIDResourceAccessor<Node>;
        // Factory class that links IDs and nodes
        using IDNodeFactory = CIDResourceFactory<Node>;

        /** @brief Type of bounding volume tree */
        enum TREETYPE {
            /** @brief Tree type for static objects */
            TREETYPE_STATIC = 0,
            /** @brief Tree type for dynamic objects */
            TREETYPE_DYNAMIC,
            /** @brief Maximum number of tree type */
            TREETYPE_MAX
        };

        /** @brief Types of pair discard methods */
        enum class PairDiscardType {
            /** @brief No discard */
            None,
            /** @brief Contact determination and decide whether to discard. */
            Check,
            /** @brief Discard */
            Discard,
        };

        /**
           @brief Leaf node data
           @details
           Lifetime of this class is the same as CColliderWrapper
        */
        struct LeafData {
            /** @brief Weak pointer to own collider wrapper */
            CWeakPtr<CColliderWrapper> thisWrapper;
            /** @brief Discard the pair containing itself? */
            PairDiscardType discardPairType;

            /**
               @brief Constructor
               @param wrapper Weak pointer to own collider wrapper
            */
            LeafData(CWeakPtr<CColliderWrapper> wrapper) {
                this->thisWrapper = std::move(wrapper);
                discardPairType = PairDiscardType::None;
            }

            /** @brief Destructor */
            ~LeafData() {}
        };

        /** @brief Internal node data */
        struct InternalData {
            /** @brief Pointer to the first child node */
            Node* child1;
            /** @brief Pointer to the second child node */
            Node* child2;

            /**
               @brief Constructor
               @param child1 Pointer to the first child node
               @param child2 Pointer to the second child node
            */
            InternalData(Node* child1, Node* child2) {
                this->child1 = child1;
                this->child2 = child2;
            }

            /** @brief Destructor */
            ~InternalData() = default;

            /**
               @brief Replace a child node
               @param oldChild Old child node
               @param newChild New child nodes
            */
            void ReplaceChild(Node* oldChild, Node* newChild);

            /**
               @brief Get the other child node
               @param child Child node in this node
               @return Other child node
            */
            Node* GetOtherChild(Node* child);
        };

        /** @brief Node structure */
        struct Node {
            /** @brief Weak pointer to handle for this node */
            WeakStableHandle<CUniquePtr<Node>> thisHandle;
            /** @brief Node ID. internal node ID is 0 */
            const NodeID nodeID;
            /** @brief Bounding volume */
            BoundingVolume bv;
            /** @brief Pointer to the parent node */
            Node* parentNode;

            union {
                /** @brief Leaf node data */
                LeafData leafData;
                /** @brief Internal node data */
                InternalData internalData;
            };

            /**
               @brief Constructor for leaf node
               @param wrapper Weak pointer to own collider wrapper
               @param bv Bounding volume
               @param id Node ID. internal node ID is 0
            */
            Node(CWeakPtr<CColliderWrapper> wrapper, const BoundingVolume& bv, NodeID id);

            /**
               @brief Constcuctor for internal node
               @param child1 Pointer to the first child node
               @param child2 Pointer to the second child node
               @param bv Bounding volume
            */
            Node(Node* child1, Node* child2, const BoundingVolume& bv);

            /** @brief Destructor */
            ~Node() {
                if (IsLeafNode()) {
                    leafData.~LeafData();
                }
                else {
                    internalData.~InternalData();
                }
            }

            /**
               @brief Is this leaf node?
               @return returns true if this node is a leaf node
            */
            bool IsLeafNode() const { return nodeID != NULL; }
        };

        /** @brief Tree structure */
        struct Tree {
            /** @brief Dynamic array of each nodes */
            StableHandleVector<CUniquePtr<Node>> nodes;
            /** @brief Pointer to root node */
            Node* rootNode;
        };

        /** @brief Structure that manages cost of node */
        struct NodeCost {
            /** @brief Cost of bounding volume of own node */
            float cost;
            /** @brief Own node */
            Node* thisNode;
            /** @brief The node that results in the lowest node when contained with this node */
            Node* leastNode;

            /**
               @brief Constructor
               @param thisNode Own node
               @param cost Cost of bounding volume of own node
            */
            NodeCost(Node* thisNode, float cost = (std::numeric_limits<float>::max)());
        };
    } // namespace Dbvt
} // namespace Mkpe

#endif // !__BV_NODE_H__
