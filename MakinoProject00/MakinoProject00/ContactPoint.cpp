#include "ContactPoint.h"
#include "Collider3D.h"
#include "ColliderWrapper.h"

// Constant for determining by distance that it is almost the same point
// #NOTE : Use this constant to set a threshold for determining whether points are almost the same, 
//         considering the angular radius of the object
constexpr float SIMILAR_DISTANCE = 0.02f;

// Constructor
Mkpe::CContactData::CContactData(const CWeakPtr<CColliderWrapper>& wrapperA, const CWeakPtr<CColliderWrapper>& wrapperB)
    : m_wrapperA(wrapperA)
    , m_wrapperB(wrapperB)
    , m_nextPointIndex(0)
    , m_similarPointThreshold(-1.0f)
{}

// Add a contact point
void Mkpe::CContactData::AddContactPoint(const Vector3f& normalWorldOnB, const Vector3f& pointWorldOnB, float penetDepth) {
    // If no contact is made, do nothing
    if (!IsContact(penetDepth)) { return; }
    
    // Get rigid body materials of two bodies
    const RigidBodyMaterial& matA = m_wrapperA->GetMaterial();
    const RigidBodyMaterial& matB = m_wrapperB->GetMaterial();

    // If the equivalence value to determine that they are the same point has not been set, set it
    if (m_similarPointThreshold < 0) {
        m_similarPointThreshold = (std::min)(m_wrapperA->GetMaxRotateScalar() * SIMILAR_DISTANCE, m_wrapperB->GetMaxRotateScalar() * SIMILAR_DISTANCE);
    }

    // Calculate contact point on the m_wrapperA
    Vector3f worldA = pointWorldOnB + normalWorldOnB * penetDepth;
    // Calculate contact point in the local coordinate space
    Vector3f localA = m_wrapperA->GetRotation().GetConjugation() * (worldA - m_wrapperA->GetPos());
    Vector3f localB = m_wrapperB->GetRotation().GetConjugation() * (pointWorldOnB - m_wrapperB->GetPos());

    // Create new contact point
    CUniquePtr<ContactPoint> newPoint = CUniquePtr<ContactPoint>::Make(localA, localB, pointWorldOnB, normalWorldOnB, penetDepth);
    // Calculate each properties
    newPoint->combinedFriction    = CalculateCombinedFriction(matA, matB);
    newPoint->combinedRestitution = CalculateCombinedRestitution(matA, matB);

    // If there is an almost the same contact point in the array, replace it
    UINT similarIndex = FindSimilarPoint(*newPoint.Get());
    if (similarIndex > -1) {
        m_contactPoint[similarIndex].Swap(newPoint);
    }
    // If not replaced, add a new one to the array
    else {
        AddContactPointToArray(newPoint);
    }
}

// Clear all contact points
void Mkpe::CContactData::ClearContactPoint() {
    // If there is no point of contact, do nothing
    if (m_nextPointIndex <= 0) { return; }

    // Reset the array of the contact points
    for (UINT i = 0; i < m_nextPointIndex; ++i) {
        m_contactPoint[i].Reset();
    }

    // Initialize the next index
    m_nextPointIndex = 0;
}

// Is the depth sent in contact?
bool Mkpe::CContactData::IsContact(float penetDepth) {
    // Set to 0 until needed.
    if (penetDepth > 0.0f) { 
        return false; 
    } 
    return true;
}

// Add a contact point to the array of contact points
void Mkpe::CContactData::AddContactPointToArray(CUniquePtr<ContactPoint>& newPoint) {
    // If the next index to be added, exceeds the limit, find a contact point that can be discarded
    UINT insertIndex = m_nextPointIndex;
    if (insertIndex >= CONTACTPOINT_MAX) {
        insertIndex = FindDiscardPoint(*newPoint.Get());
    }
    // If it doesn't exceed the limit, add the next index
    else {
        m_nextPointIndex++;
    }

    assert(insertIndex < CONTACTPOINT_MAX);

    // Swap the ownership
    m_contactPoint[insertIndex].Swap(newPoint);
}

// Find and return the most discardable a contact point in the array
UINT Mkpe::CContactData::FindDiscardPoint(const ContactPoint& newPoint) {
    // Vouch to be filled the array of contact points
    assert(m_nextPointIndex >= CONTACTPOINT_MAX);
    
    // Variable declarations
    float maxDepth      = newPoint.penetDepth; // Maximum value of depth
    int   maxDepthIndex = -1;                  // Index of maximum value of depth
    // Search contact point with most penetration depth
    for (UINT i = 0; i < CONTACTPOINT_MAX; ++i) {
        if (m_contactPoint[i]->penetDepth < maxDepth) {
            maxDepth = m_contactPoint[i]->penetDepth;
            maxDepthIndex = (int)i;
        }
    }

    // Lambda formula to calculate area
    auto calclateArea = [&](int ban, int a, int b, int c) -> float {
        // If index of the maximum value of depth, is same as ban, nothing is done
        if (ban == maxDepthIndex) { return 0.0f; }

        // Create two vectors consisting of 4 points
        Vector3f v1 = newPoint.pointLocalOnA - m_contactPoint[a]->pointLocalOnA;
        Vector3f v2 = m_contactPoint[b]->pointLocalOnA - m_contactPoint[c]->pointLocalOnA;
        // Calculate cross product, return it
        // #NOTE : it is equal to area of parrallelogram consisting of v1, v2
        Vector3f cross = v1.Cross(v2);
        return cross.Length();
    };

    // variables to store each area
    float area[CONTACTPOINT_MAX] = { 0.0f, 0.0f, 0.0f, 0.0f };
    
    // Calculate each area
    area[0] = calclateArea(0, 1, 3, 2);
    area[1] = calclateArea(1, 0, 3, 2);
    area[2] = calclateArea(2, 0, 3, 1);
    area[3] = calclateArea(3, 0, 2, 1);

    // Calculate the index to represent the maximum value of area
    UINT maxAreaIndex = 0;
    for (UINT i = 1; i < CONTACTPOINT_MAX; ++i) {
        if (area[maxAreaIndex] < area[i]) {
            maxAreaIndex = i;
        }
    }

    return maxAreaIndex;
}

// Find a contact point matching the specified argument and returns it as a discard target
UINT Mkpe::CContactData::FindSimilarPoint(const ContactPoint& newPoint) {
    // Get the squared value for determining whether contact points to represent almost the same point
    float shortestDistance = m_similarPointThreshold * m_similarPointThreshold;

    // Index of contact point determined to be almost the same point
    UINT similarPointIndex = -1;
    // Repeat for all contact points you currently have
    for (UINT i = 0; i < m_nextPointIndex; ++i) {
        // Calculate the distance vector between two contact points with rigid body A, and get its length
        Vector3f vDistanceA = m_contactPoint[i]->pointLocalOnA - newPoint.pointLocalOnA;
        const float distanceA = vDistanceA.LengthSq();

        // Calculate the distance vector between two contact points with rigid body B, and get its length
        Vector3f vDistanceB = m_contactPoint[i]->pointLocalOnB - newPoint.pointLocalOnB;
        const float distanceB = vDistanceB.LengthSq();
        
        // Search for the shortest distance
        if (distanceA < shortestDistance || distanceB < shortestDistance) {
            shortestDistance = (std::min)(distanceA, distanceB);
            similarPointIndex = i;
        }
    }
    return similarPointIndex;
}

// Get the combined coefficient of friction of two rigid bodies
float Mkpe::CContactData::CalculateCombinedFriction(const RigidBodyMaterial& matA, const RigidBodyMaterial& matB) {
    return std::sqrtf(matA.friction * matB.friction);
}

// Get the combined coefficient of restitution of two rigid bodies
float Mkpe::CContactData::CalculateCombinedRestitution(const RigidBodyMaterial& matA, const RigidBodyMaterial& matB) {
    return std::sqrtf(matA.restitution * matB.restitution);
}
