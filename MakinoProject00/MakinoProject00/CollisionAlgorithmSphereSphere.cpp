#include "CollisionAlgorithmSphereSphere.h"
#include "SphereCollider3D.h"

// Sphere and sphere collision algorithm
bool Mkpe::CollisionAlgorithm::SphereSphere(Dbvt::BVOverlapPair* pair) {
    // Get the two wrappers
    CColliderWrapper* wrapperA = pair->GetWrapperA();
    CColliderWrapper* wrapperB = pair->GetWrapperB();
    // Cast the colliders of the two wrappers to sphere collider
    const CSphereCollider3D* sphereA = static_cast<const CSphereCollider3D*>(wrapperA->GetCollider().Get());
    const CSphereCollider3D* sphereB = static_cast<const CSphereCollider3D*>(wrapperB->GetCollider().Get());
    // Get radii of the two spheres
    float radiusA = sphereA->GetScalingRadius();
    float radiusB = sphereB->GetScalingRadius();
    // Get positions of the two spheres
    Vector3f posA = wrapperA->GetPos();
    Vector3f posB = wrapperB->GetPos();

    // Calculate a distance vector from the positions of the two spheres
    Vector3f dist = posA - posB;
    // Calculate a lenhth of the distance vector
    float distLength = dist.Length();
    
    // Calculate a penetration depth
    float penetDepth = distLength - (radiusA + radiusB);
    // If there is no contact, return false
    if (!CContactData::IsContact(penetDepth)) {
        return false;
    }

    // #NOTE : It's a collision when it comes to this point.

    // If the colliders overlap each other, contact points are not added
    if (pair->CanOverlapAfterApproval()) {
        return false;
    }

    // Calculate the normal vector of the contact point on the sphere B
    Vector3f normalB(0.0f, 0.0f, 1.0f);
    if (distLength > Utl::FLOAT_IGNORE) {
        normalB = dist / distLength;
    }

    // Calculate the contact point on the sphere B
    Vector3f contactPosB = posB + radiusB * normalB;

    // Add the contact point
    pair->contactData.AddContactPoint(normalB, contactPosB, penetDepth);

    return true;
}
