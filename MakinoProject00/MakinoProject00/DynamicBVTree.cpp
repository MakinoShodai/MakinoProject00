#include "DynamicBVTree.h"
#include "BoudingVolume.h"
#include "PhysicsWorld.h"
#include "Scene.h"
#include "Shape.h"

// Amount of margin to expand bounding volume
const Vector3f BV_MARGIN = { 0.05f, 0.05f, 0.05f };

// Constructor
Mkpe::Dbvt::CDynamicBVTree::CDynamicBVTree()
    : m_idResourceFactory(NULL + 1)
    , m_bvPairManager(&m_idResourceFactory)
    , m_handoverID(std::nullopt)
    , m_bvMargin(BV_MARGIN)
    , m_isInitialize(false) {
}

// Initialize
void Mkpe::Dbvt::CDynamicBVTree::Initialize() {
    // Re-align tree
    RealignmentTree(TREETYPE_STATIC);
    RealignmentTree(TREETYPE_DYNAMIC);

    // Create pairs that potentially collide
    // #NOTE : Static vs. static doesn't create pairs
    NodeCollisionDetection(m_tree[TREETYPE_STATIC].rootNode,  m_tree[TREETYPE_DYNAMIC].rootNode);
    NodeCollisionDetection(m_tree[TREETYPE_DYNAMIC].rootNode, m_tree[TREETYPE_DYNAMIC].rootNode);

    // Update size of the array at previous physics simulation
    m_bvPairManager.UpdatePrevSize();

    // Finish initialize processing
    m_isInitialize = true;
}

// Re-align the tree from 0
void Mkpe::Dbvt::CDynamicBVTree::RealignmentTree(TREETYPE type) {
    // If there are no objects in the tree, do nothing
    if (m_tree[type].nodes.Empty()) { return; }

    // Variables for nodes that do not yet have a parent node
    std::vector<NodeCost> nonParentRemains;

    // Include all elements in the tree in an array with no parent node
    std::vector<WeakStableHandle<CUniquePtr<Node>>> internalNodeHandles;
    for (auto& it : m_tree[type].nodes) {
        // If it is internal node
        if (false == it->IsLeafNode()) {
            // Add its handle to the array
            internalNodeHandles.push_back(it->thisHandle);
            continue;
        }

        // Add it to remain nodes
        NodeCost nearBV(it.Get());
        nonParentRemains.push_back(nearBV);
    }

    // Erase internal nodes from array
    for (auto& handle : internalNodeHandles) {
        m_tree[type].nodes.Erase(*handle);
    }

    // Let each nearest node be computed and held
    NodeCost* minCostPair;
    minCostPair = AssignNearBV(nonParentRemains);

    // Repeat until there is one parentless node
    while (nonParentRemains.size() > 1) {
        // Create a bounding volume that contains two nodes that are a minimum cost combination
        BoundingVolume sumBV(minCostPair->thisNode->bv, minCostPair->leastNode->bv);

        // Create a internal node that contains two nodes that are a minimum cost combination
        WeakStableHandle<CUniquePtr<Node>> handle = 
            m_tree[type].nodes.PushBack(
                CUniquePtr<Node>::Make(minCostPair->thisNode, minCostPair->leastNode, sumBV));

        // Set own handle to new internal node
        Node* parentNode = m_tree[type].nodes[*handle].Get();
        parentNode->thisHandle = handle;

        // Set parent node to the two nodes
        minCostPair->thisNode->parentNode  = parentNode;
        minCostPair->leastNode->parentNode = parentNode;
        
        // Calculation when adding a new internal node
        minCostPair = CalculationAddingInternalNode(nonParentRemains, parentNode);
    }

    // Set root node
    m_tree[type].rootNode = nonParentRemains.back().thisNode;
}

// Add a new leaf node to the tree
void Mkpe::Dbvt::CDynamicBVTree::AddLeafNode(CColliderWrapper* wrapper, std::optional<BoundingVolume> specifyBV) {
    // Get tree type based on type of rigid body
    TREETYPE treeType = (wrapper->IsStatic()) ? TREETYPE_STATIC : TREETYPE_DYNAMIC;

    // Use the specified bounding volume, if available
    BoundingVolume bv;
    if (specifyBV.has_value()) {
        bv = specifyBV.value();
    }
    // If none, create a bounding volume
    else {
        bv = wrapper->GetCollider()->GenerateBoudingVolume();

        // If tree type is dynamic, Expand bounding volume to some extent
        if (treeType == TREETYPE_DYNAMIC) {
            bv.Expand(m_bvMargin);
        }
    }

    // If there is a handover ID, carry out the handover. if not, don't carry out the handover
    NodeID id = (m_handoverID.has_value()) ? m_handoverID.value() : m_idResourceFactory.GenerateID();

    // Create a leaf node
    WeakStableHandle<CUniquePtr<Node>> leafHandle = m_tree[treeType].nodes.PushBack(CUniquePtr<Node>::Make(wrapper->WeakFromThis(), bv, id));
    Node* leaf = m_tree[treeType].nodes[*leafHandle].Get();
    
    // Set handle for leaf node, to the it
    leaf->thisHandle = leafHandle;

    // Associate an ID with the leaf node
    m_idResourceFactory.LinkIDResouce(id, leaf);

    // Set this node to the wrapper and the tree type
    wrapper->SetNode(leaf);
    wrapper->SetTreeType(treeType);

    // If before initialization, so exit function
    if (false == m_isInitialize) {
        return;
    }

    // If a root node doesn't yet exists
    if (m_tree[treeType].rootNode == nullptr) {
        // Make the new node the root node
        m_tree[treeType].rootNode = leaf;
        // Exit function
        return;
    }

    // Find compatible sibling node
    Node* sibling = FindGoodSibling(m_tree[treeType], leaf);

    // Create a bounding volume that contains two nodes
    BoundingVolume sumBV(leaf->bv, sibling->bv);

    // Create an internal node for new parent
    WeakStableHandle<CUniquePtr<Node>> newParentHandle = m_tree[treeType].nodes.PushBack(CUniquePtr<Node>::Make(leaf, sibling, sumBV));
    Node* newParent = m_tree[treeType].nodes[*newParentHandle].Get();

    // Set handle for new parent node, to the it
    newParent->thisHandle = newParentHandle;

    // If the sibling node isn't root node, swap a child node of the parent node
    if (sibling->parentNode != nullptr) {
        sibling->parentNode->internalData.ReplaceChild(sibling, newParent);
        newParent->parentNode = sibling->parentNode;
    }
    // If the sibling node is root node, change it to new internal node
    else {
        m_tree[treeType].rootNode = newParent;
    }

    // Set two parents
    leaf->parentNode = newParent;
    sibling->parentNode = newParent;

    // Perform branch rotation operation
    RotateTreeForAncestors(m_tree[treeType], leaf);
}

// Remove a leaf node from the tree
void Mkpe::Dbvt::CDynamicBVTree::RemoveLeafNode(CColliderWrapper* wrapper) {
    // If it's not leaf node, error
    Node* leaf = wrapper->GetNode();
    assert(leaf->IsLeafNode());

    // Get tree type based on the collider
    TREETYPE treeType = wrapper->GetTreeType();

    // If it's not set in the tree, do nothing
    if (treeType == TREETYPE_MAX) { return; }

    // If transfer ID not set, release the current ID.
    if (!m_handoverID.has_value()) {
        m_idResourceFactory.ReleaseID(leaf->nodeID);
    }

    // If it's a root node, initialize the tree
    if (leaf == m_tree[treeType].rootNode) {
        m_tree[treeType].nodes.Clear();
        m_tree[treeType].rootNode = nullptr;
    }
    // If it's not a root node
    else {
        // Get a parent node and a grandparent node
        Node* parent = leaf->parentNode;
        Node* grandparent = leaf->parentNode->parentNode;
        // Get a sibling node
        Node* sibling = parent->internalData.GetOtherChild(leaf);

        // If the grandparent node exists
        if (grandparent != nullptr) {
            // Replace the child node of the grandparent node, to the sibling node
            grandparent->internalData.ReplaceChild(parent, sibling);
            // Replace the parent node of the sibling node, to the grandparent node
            sibling->parentNode = grandparent;
        }
        // If the grandparent node doesn't exist, make the sibling node the root node
        else {
            m_tree[treeType].rootNode = sibling;
            sibling->parentNode = nullptr;
        }

        // Erase the leaf node and the parent node from the tree
        m_tree[treeType].nodes.Erase(*leaf->thisHandle);
        m_tree[treeType].nodes.Erase(*parent->thisHandle);

        // Perform branch rotation operation
        if (grandparent != nullptr) {
            RotateTreeForAncestors(m_tree[treeType], sibling);
        }
    }
}

// Reinsert a leaf node that already exists in the tree
Mkpe::Dbvt::Node* Mkpe::Dbvt::CDynamicBVTree::ReInsertLeafNode(CColliderWrapper* wrapper, std::optional<BoundingVolume> specifyBV) {
    // Vouch the node of the wrapper is a leaf node
    Node* leaf = wrapper->GetNode();
    assert(leaf->IsLeafNode());
    
    // Set handover ID
    m_handoverID = leaf->nodeID;

    // Node operations
    RemoveLeafNode(wrapper);
    AddLeafNode(wrapper, specifyBV);

    // Initialize handover ID
    m_handoverID = std::nullopt;

    // Return the new node
    return wrapper->GetNode();
}

// Perform collision detection at node and node, creating potentially colliding pairs.
void Mkpe::Dbvt::CDynamicBVTree::NodeCollisionDetection(const Node* nodeA, const Node* nodeB) {
    // If the each node is nullptr, do nothing
    if (nodeA == nullptr || nodeB == nullptr) { return; }

    // If the node A and the node B are in contact
    if (nodeA->bv.IsIntersect(nodeB->bv)) {
        // Contact between the leaf nodes
        if (nodeA->IsLeafNode() && nodeB->IsLeafNode()) {
            // If the node A and the node B are the same, do nothing
            if (nodeA->nodeID == nodeB->nodeID) { return; }
            // Create a new pair
            m_bvPairManager.AddPair(nodeA->nodeID, nodeB->nodeID);
        }
        // Not between leaf nodes
        else {
            // If the node A is the only leaf node
            if (nodeA->IsLeafNode()) {
                NodeCollisionDetection(nodeA, nodeB->internalData.child1);
                NodeCollisionDetection(nodeA, nodeB->internalData.child2);
            }
            // If the node B is the only leaf node
            else if (nodeB->IsLeafNode()) {
                NodeCollisionDetection(nodeA->internalData.child1, nodeB);
                NodeCollisionDetection(nodeA->internalData.child2, nodeB);
            }
            // If both are internal nodes
            else {
                NodeCollisionDetection(nodeA->internalData.child1, nodeB->internalData.child1);
                NodeCollisionDetection(nodeA->internalData.child2, nodeB->internalData.child1);
                NodeCollisionDetection(nodeA->internalData.child1, nodeB->internalData.child2);
                NodeCollisionDetection(nodeA->internalData.child2, nodeB->internalData.child2);
            }
        }
    }
}

// Assign the closest BV to each BV
Mkpe::Dbvt::NodeCost* Mkpe::Dbvt::CDynamicBVTree::AssignNearBV(std::vector<NodeCost>& remains) {
    // Assign the first one to the least cost pair variable
    NodeCost* minCostPair = &remains[0];
    // Repeat for all nodes
    for (auto& it_A : remains) {
        for (auto& it_B : remains) {
            // If A and B are the same nodes, do nothing
            if (it_A.thisNode == it_B.thisNode) { continue; }

            // Calculate the const of a bounding volume contained the two nodes
            float cost = BoundingVolume(it_A.thisNode->bv, it_B.thisNode->bv).GetCost();
            // If the calculated cost is less than the retains cost, replace
            if (cost < it_A.cost) {
                it_A.leastNode = it_B.thisNode;
                it_A.cost = cost;
            }
        }

        // If it's least cost node, assign it to the variable for the least cost pair
        if (it_A.cost < minCostPair->cost) {
            minCostPair = &it_A;
        }
    }

    // Return the least cost pair
    return minCostPair;
}

// Calculation when adding a new internal node
Mkpe::Dbvt::NodeCost* Mkpe::Dbvt::CDynamicBVTree::CalculationAddingInternalNode(std::vector<NodeCost>& remains, Node* newNode) {
    // If the sent node is not internal node, error
    assert(!newNode->IsLeafNode());

    // Create container for elements comparison
    std::unordered_set<Node*> invalidatedNodes;
    invalidatedNodes.insert(newNode->internalData.child1);
    invalidatedNodes.insert(newNode->internalData.child2);

    // Set the nearest node of the new node, to the child node
    NodeCost newBV(newNode);
    newBV.leastNode = newNode->internalData.child1;
    remains.push_back(newBV);

    // Search the least cost pair
    NodeCost* minCostPair = nullptr;
    for (auto& it_A : remains) {
        do {
            // The two child nodes will be erased from the array in future, so do nothing
            if (invalidatedNodes.contains(it_A.thisNode)) { break; }

            // Search the node whose closest node is one of the two child nodes
            if (!invalidatedNodes.contains(it_A.leastNode)) { break; }

            // Initialize cost
            it_A.cost = Utl::Limit::FLAOT_HIGHEST;
            for (auto& it_B : remains) {
                if (it_A.thisNode == it_B.thisNode) { continue; }

                // The two child nodes will be erased from the array in future, so do nothing
                if (invalidatedNodes.contains(it_B.thisNode)) { continue; }

                // Calculate the cost of a bounding volume contained the two nodes
                float cost = BoundingVolume(it_A.thisNode->bv, it_B.thisNode->bv).GetCost();
                // If the calculated cost is less than the retains cost, replace
                if (cost < it_A.cost) {
                    it_A.leastNode = it_B.thisNode;
                    it_A.cost = cost;
                }
            }
        } while (false);

        // If it's least cost node, assign
        if (!invalidatedNodes.contains(it_A.thisNode)) {
            if (minCostPair == nullptr) {
                minCostPair = &it_A;
            }
            else if(it_A.cost < minCostPair->cost) {
                minCostPair = &it_A;
            }
        }
    }

    // Erase the two nodes from array
    remains.erase(
        std::remove_if(remains.begin(), remains.end(),
            [&](const NodeCost& node) {
                return invalidatedNodes.contains(node.thisNode);
            }),
        remains.end());

    // Return least cost pair
    return minCostPair;
}

// Perform branch rotation operation to minimize cost for all ancestor nodes of the specified node
void Mkpe::Dbvt::CDynamicBVTree::RotateTreeForAncestors(Tree& tree, Node* node) {
    while (!(node->parentNode == nullptr || node->parentNode->parentNode == nullptr)) {
        // Get the grandparent node
        Node* grandparent = node->parentNode->parentNode;

        // Get the two child nodes of the grand parent
        // #NOTE : A grandparent node is guaranteed to have two childrens because it can only be an internal node
        Node* parents[2];
        parents[0] = grandparent->internalData.child1;
        parents[1] = grandparent->internalData.child2;

        // Get 2x2 grandchildren nodes from grandparent node
        // #NOTE : Grandchildren node may not exist
        Node* childs[2][2];
        childs[0][0] = (!parents[0]->IsLeafNode()) ? parents[0]->internalData.child1 : nullptr;
        childs[0][1] = (!parents[0]->IsLeafNode()) ? parents[0]->internalData.child2 : nullptr;
        childs[1][0] = (!parents[1]->IsLeafNode()) ? parents[1]->internalData.child1 : nullptr;
        childs[1][1] = (!parents[1]->IsLeafNode()) ? parents[1]->internalData.child2 : nullptr;

        // Search two nodes that has the least cost combination
        Node* replaceNode[2] = { nullptr, nullptr };
        float minimumCost = parents[0]->bv.GetCost() + parents[1]->bv.GetCost();
        for (int i = 0; i < 2; ++i) {
            // If the parent node of it is leaf node, do nothing
            if (parents[i]->IsLeafNode()) { continue; }
            for (int j = 0; j < 2; ++j) {
                // Get the cost of the child node
                assert(childs[i][j] != nullptr && childs[i][(j + 1) % 2] != nullptr);
                float costA = childs[i][j]->bv.GetCost();
                
                // Calculate the cost of bounding volume if replaced
                // #NOTE : Calculate using the other child node and the parent node to be replaced
                float costB = BoundingVolume(
                    childs[i][(j + 1) % 2]->bv, 
                    parents[(i + 1) % 2]->bv).GetCost();

                // If the calculated cost is least cost, update variables
                if (costA + costB < minimumCost) {
                    replaceNode[0] = childs[i][j];
                    replaceNode[1] = parents[(i + 1) % 2];
                    minimumCost = costA + costB;
                }
            }
        }

        // If replacing is to be done
        if (replaceNode[0] != nullptr) {
            // Replace the child nodes of the parent nodes of the least cost nodes
            replaceNode[0]->parentNode->internalData.ReplaceChild(replaceNode[0], replaceNode[1]);
            replaceNode[1]->parentNode->internalData.ReplaceChild(replaceNode[1], replaceNode[0]);

            // Replace the parent nodes of the least cost nodes
            Node* workNode = replaceNode[0]->parentNode;
            replaceNode[0]->parentNode = replaceNode[1]->parentNode;
            replaceNode[1]->parentNode = workNode;

            // Reconfigure the bounding volumes of the parent nodes of the least cost nodes
            replaceNode[0]->parentNode->bv = BoundingVolume(
                replaceNode[0]->parentNode->internalData.child1->bv, 
                replaceNode[0]->parentNode->internalData.child2->bv);
            replaceNode[1]->parentNode->bv = BoundingVolume(
                replaceNode[1]->parentNode->internalData.child1->bv,
                replaceNode[1]->parentNode->internalData.child2->bv);
        }
        // Reconfigure the bounding volume of the parent node of the node to be operated
        node->parentNode->bv = BoundingVolume(
            node->parentNode->internalData.child1->bv,
            node->parentNode->internalData.child2->bv);

        // Advance to the parent node of the node to be operated
        node = node->parentNode;
    }

    // Reconfigure the bounding volume of the sent node
    if (node->parentNode != nullptr) {
        node->parentNode->bv = BoundingVolume(
            node->parentNode->internalData.child1->bv,
            node->parentNode->internalData.child2->bv);
    }

    // Reconfigure the bounding volume of the root node
    if (node->parentNode != tree.rootNode) {
        tree.rootNode->bv = BoundingVolume(
            tree.rootNode->internalData.child1->bv,
            tree.rootNode->internalData.child2->bv);
    }
}

// Find the appropriate sibling to insert the specified node into the tree
Mkpe::Dbvt::Node* Mkpe::Dbvt::CDynamicBVTree::FindGoodSibling(const Tree& tree, Node* newNode) {
    Node* node = tree.rootNode;
    
    // If it's a internal node, Repeat the search
    while (node->IsLeafNode() == false) {
        float proximity1 = node->internalData.child1->bv.GetProximity(newNode->bv);
        float proximity2 = node->internalData.child2->bv.GetProximity(newNode->bv);

        // Priority is given to the node that is closer to its node
        node = (proximity1 < proximity2) ? node->internalData.child1 : node->internalData.child2;
    }

    // Return the found node as a good sibling
    assert(node != nullptr);
    return node;
}
