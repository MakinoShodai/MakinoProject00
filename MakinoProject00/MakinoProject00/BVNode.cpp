#include "BVNode.h"

// Constructor (leaf node)
Mkpe::Dbvt::Node::Node(CWeakPtr<CColliderWrapper> wrapper, const BoundingVolume& bv, NodeID id)
    : thisHandle(nullptr)
    , nodeID(id)
    , bv(bv)
    , parentNode(nullptr)
    , leafData(std::move(wrapper)) {
}

// Constructor (internal node)
Mkpe::Dbvt::Node::Node(Node* child1, Node* child2, const BoundingVolume& bv)
    : thisHandle(nullptr)
    , nodeID(NULL)
    , bv(bv)
    , parentNode(nullptr)
    , internalData(child1, child2) {
}

// Replace a child node
void Mkpe::Dbvt::InternalData::ReplaceChild(Node* oldChild, Node* newChild) {
    // Find and replace children node matching oldChild
    if (child1 == oldChild) {
        child1 = newChild;
    }
    else if (child2 == oldChild) {
        child2 = newChild;
    }
}

// Get the other child node
Mkpe::Dbvt::Node* Mkpe::Dbvt::InternalData::GetOtherChild(Node* child) {
    // Vouch the sent node is a child node of the this node
    assert(child == child1 || child == child2);

    // Return the other node
    return (child == child1) ? child2 : child1;
}

// Constructor of NodeCost class
Mkpe::Dbvt::NodeCost::NodeCost(Dbvt::Node* thisNode, float cost)
    : cost(cost)
    , thisNode(thisNode)
    , leastNode(nullptr) 
{ }
