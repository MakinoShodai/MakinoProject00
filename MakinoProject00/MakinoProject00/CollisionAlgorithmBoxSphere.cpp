#include "CollisionAlgorithmBoxSphere.h"
#include "CapsuleCollider3D.h"
#include "SphereCollider3D.h"
#include "BoxCollider3D.h"
#include "ColliderWrapper.h"

// Sphere and box intersection judgment
bool IntersectSphereBox(const Vector3f& boxHalfSize, const Vector3f& boxPos, const Quaternionf& boxRotate, 
    const Vector3f& spherePos, float sphereRadius, bool isBoxA,
    Vector3f* normalB, Vector3f* worldPosB, float* depth);

// Recalculation if the sphere is contained in the box
float RecalculateIfSphereInBox(const Vector3f& boxHalfSize, const Vector3f& sphereRelativePos, Vector3f* closestPoint, Vector3f* normal);

// Calculate a vector that moves a point in the capsule onto its surface
Vector3f MoveInterPointOnCapsuleSurface(const Vector3f& pos, const Vector3f& dir, float halfLength, float radius, const Vector3f& interPoint);

// Box and sphere collision algorithm
bool Mkpe::CollisionAlgorithm::BoxSphere(Dbvt::BVOverlapPair* pair) {
    // Get the two wrappers
    CColliderWrapper* wrapperA = pair->GetWrapperA();
    CColliderWrapper* wrapperB = pair->GetWrapperB();

    // wrapper A is a box?
    bool isBoxA = (wrapperA->GetCollider()->GetColliderType() == ColliderType::Box);

    // Get the two wrappers of each colliders
    CColliderWrapper* boxWrapper = isBoxA ? wrapperA : wrapperB;
    CColliderWrapper* sphereWrapper = isBoxA ? wrapperB : wrapperA;

    // Cast the colliders of the two wrappers to each collider
    const CSphereCollider3D* sphere = static_cast<const CSphereCollider3D*>(sphereWrapper->GetCollider().Get());
    const CBoxCollider3D* box = static_cast<const CBoxCollider3D*>(boxWrapper->GetCollider().Get());

    // Get a half size of the box
    Vector3f boxHalfSize = box->GetScalingSize() * Utl::Inv::_2;

    // Intersection judgment
    Vector3f normalB;
    Vector3f pointB;
    float    depth;
    bool isIntersect = IntersectSphereBox(boxHalfSize, boxWrapper->GetPos(), boxWrapper->GetRotation(), sphereWrapper->GetPos(), sphere->GetScalingRadius(), isBoxA, &normalB, &pointB, &depth);

    // If two colliders intersect, add contact point
    if (isIntersect) {
        // If the colliders overlap each other, contact points are not added
        if (pair->CanOverlapAfterApproval()) {
            return false;
        }

        pair->contactData.AddContactPoint(normalB, pointB, depth);
    }

    // Return result
    return isIntersect;
}

// Box and capsule collision algorithm
bool Mkpe::CollisionAlgorithm::BoxCapsule(Dbvt::BVOverlapPair* pair) {
    // #NOTE : It is an incomplete collision judgment that considers the capsule as two Spheres and calculates the contact point for them.
    //         Currently not used as the GJK algorithm is implemented.

    // Get the two wrappers
    CColliderWrapper* wrapperA = pair->GetWrapperA();
    CColliderWrapper* wrapperB = pair->GetWrapperB();

    // wrapper A is a box?
    bool isBoxA = (wrapperA->GetCollider()->GetColliderType() == ColliderType::Box);

    // Get the two wrappers of each colliders
    CColliderWrapper* boxWrapper = isBoxA ? wrapperA : wrapperB;
    CColliderWrapper* capsuleWrapper = isBoxA ? wrapperB : wrapperA;

    // Cast the colliders of the two wrappers to each collider
    const CCapsuleCollider3D* capsule = static_cast<const CCapsuleCollider3D*>(capsuleWrapper->GetCollider().Get());
    const CBoxCollider3D* box = static_cast<const CBoxCollider3D*>(boxWrapper->GetCollider().Get());

    // Get the position of the capsule
    Vector3f capsulePos = capsuleWrapper->GetPos();

    // Get the radius of the capsule
    float capsuleRadius = capsule->GetScalingRadius();

    // Get the half length of the center axis of the capsule
    float capsuleLengthHalf = capsule->GetScalingLength() * Utl::Inv::_2;

    // Get the half size of the box
    Vector3f boxHalfSize = box->GetScalingSize() * Utl::Inv::_2;

    // Variable declarations
    Vector3f normalB;             // The normal vector of the contact point on the collider B
    Vector3f pointB;              // The contact point on the collider B
    float    depth = 0.0f;        // Penetration depth
    bool     isIntersect = false; // Did the two colliders intersect?

    // If the length of the capsule is 0, treat it as just a Sphere
    if (Utl::IsFloatZero(capsuleLengthHalf)) {
        isIntersect = IntersectSphereBox(boxHalfSize, boxWrapper->GetPos(), boxWrapper->GetRotation(), capsulePos, capsuleRadius, isBoxA, &normalB, &pointB, &depth);
    }
    // Treat as 2 Spheres since the length of the capsule is not zero
    else {
        // Variable declarations
        Vector3f normalB1; // The normal vector 1 of the contact point on the collider B
        Vector3f pointB1;  // The contact point 1 on the collider B
        float    depth1;   // Penetration depth 1
        Vector3f normalB2; // The normal vector 2 of the contact point on the collider B
        Vector3f pointB2;  // The contact point 2 on the collider B
        float    depth2;   // Penetration depth 2

        // Get rotation matrix of the capsule
        Matrix3x3f capsuleMat = capsuleWrapper->GetRotation().GetMatrix();

        // Get the index of the center axis of the capsule
        UINT8 axisIndex = capsule->GetAxisIndex();

        // Get the direction vector of the center axis of the capsule
        Vector3f direction = Vector3f(capsuleMat(axisIndex, Utl::Math::_X), capsuleMat(axisIndex, Utl::Math::_Y), capsuleMat(axisIndex, Utl::Math::_Z));

        // Get positions of the two hemispheres of the capsule
        Vector3f capsulePos1 = capsulePos + direction * capsuleLengthHalf;
        Vector3f capsulePos2 = capsulePos - direction * capsuleLengthHalf;

        // Intersection judgment
        bool isIntersect1 = IntersectSphereBox(boxHalfSize, boxWrapper->GetPos(), boxWrapper->GetRotation(), capsulePos1, capsuleRadius, isBoxA, &normalB1, &pointB1, &depth1);
        bool isIntersect2 = IntersectSphereBox(boxHalfSize, boxWrapper->GetPos(), boxWrapper->GetRotation(), capsulePos2, capsuleRadius, isBoxA, &normalB2, &pointB2, &depth2);
        // Processing for the first intersection judgment
        if (isIntersect1) {
            // Get the normal vector of the contact point on the box
            Vector3f normalBox = (isBoxA) ? -normalB1 : normalB1;

            // If the normal direction of the contact point on the box is outside the capsule, it is assumed that there is no collision
            float dot = direction.Dot(normalBox);
            if (dot < 0.0f) {
                // Get the contact point on the capsule
                Vector3f contactPointCapsule = (isBoxA) ? pointB1 : pointB1 + normalB1 * depth1;
                // Calculate a vector that moves a point in the capsule onto its surface
                Vector3f vMove = MoveInterPointOnCapsuleSurface(capsulePos, direction, capsuleLengthHalf, capsuleRadius, contactPointCapsule);

                // If Box is A, move the contact point
                if (isBoxA) {
                    pointB1 += vMove;
                }
                
                // Change the depth by the amount of movement
                depth1 += normalBox.Dot(vMove);
                // If the collision is still valid, the two colliders are in contact
                if (depth1 < depth) {
                    depth = depth1;
                    normalB = normalB1;
                    pointB = pointB1;
                    isIntersect = true;
                }
            }
        }
        // Processing for the second intersection judgment
        if (isIntersect2) {
            // Get the normal vector of the contact point on the box
            Vector3f normalBox = (isBoxA) ? -normalB2 : normalB2;

            // If the normal direction of the contact point on the box is outside the capsule, it is assumed that there is no collision
            float dot = -direction.Dot(normalBox);
            if (dot < 0.0f) {
                // Get the contact point on the capsule
                Vector3f contactPointCapsule = (isBoxA) ? pointB2 : pointB2 + normalB2 * depth2;
                // Calculate a vector that moves a point in the capsule onto its surface
                Vector3f vMove = MoveInterPointOnCapsuleSurface(capsulePos, direction, capsuleLengthHalf, capsuleRadius, contactPointCapsule);

                // If Box is A, move the contact point
                if (isBoxA) {
                    normalB2 += vMove;
                }

                // Change the depth by the amount of movement
                depth2 += normalBox.Dot(vMove);
                // If the first intersection is already valid and the depth is nearly equal, 
                // the point between the two collision points is the intersection point
                if (isIntersect && Utl::IsEqual(depth, depth2)) {
                    Vector3f vPoint2_1 = pointB2 - pointB;
                    pointB += vPoint2_1 * 0.5f;
                }
                // If the collision is still valid, the two colliders are in contact
                else if (depth2 < depth) {
                    depth = depth2;
                    normalB = normalB2;
                    pointB = pointB2;
                    isIntersect = true;
                }
            }
        }

    }

    // If two colliders intersect, add contact point
    if (isIntersect) {
        // If the colliders overlap each other, contact points are not added
        if (pair->CanOverlapAfterApproval()) {
            return false;
        }

        pair->contactData.AddContactPoint(normalB, pointB, depth);
    }

    return isIntersect;
}

// Sphere and box intersection judgment
bool IntersectSphereBox(const Vector3f& boxHalfSize, const Vector3f& boxPos, const Quaternionf& boxRotate,
    const Vector3f& spherePos, float sphereRadius, bool isBoxA,
    Vector3f* normalB, Vector3f* worldPosB, float* depth) {

    // Get the sphere's potision in the box's coordinate system 
    Vector3f sphereRelativePos = boxRotate.GetConjugation() * (spherePos - boxPos);

    // Find the position closest to the position of the sphere inside the box
    Vector3f closestPoint;
    closestPoint.x() = (std::min)(boxHalfSize.x(), sphereRelativePos.x());
    closestPoint.x() = (std::max)(-boxHalfSize.x(), closestPoint.x());
    closestPoint.y() = (std::min)(boxHalfSize.y(), sphereRelativePos.y());
    closestPoint.y() = (std::max)(-boxHalfSize.y(), closestPoint.y());
    closestPoint.z() = (std::min)(boxHalfSize.z(), sphereRelativePos.z());
    closestPoint.z() = (std::max)(-boxHalfSize.z(), closestPoint.z());

    // Calculate the shortest distance from the sphere to the box
    // #NOTE : This would also be the normal direction
    Vector3f normalOnBox = sphereRelativePos - closestPoint;
    float length2 = normalOnBox.LengthSq();
    // If the distance is greater than the radius, there is no contact
    if (length2 > sphereRadius * sphereRadius) {
        return false;
    }

    // Calculate penetration depth
    if (length2 <= FLT_EPSILON * FLT_EPSILON) {
        *depth = -RecalculateIfSphereInBox(boxHalfSize, sphereRelativePos, &closestPoint, &normalOnBox);
    }
    else {
        *depth = normalOnBox.Length();

        // Normalize the normal vector
        normalOnBox /= *depth;
    }
    // Subtract sphere's radius from penetration depth
    *depth -= sphereRadius;

    // Convert a contact point in a box's coordinate system to world coordinates
    closestPoint = (boxRotate * closestPoint) + boxPos;
    // Convert a normal vector in a box's coordinate system to world coordinates
    normalOnBox = boxRotate * normalOnBox;

    // If box is collider A
    if (isBoxA) {
        // Calculate a contact point on the sphere
        Vector3f pointOnSphere = closestPoint + normalOnBox * *depth;
        // Set information with sphere as B
        *normalB = -normalOnBox;
        *worldPosB = pointOnSphere;
    }
    // If box is collider B
    else {
        *normalB = normalOnBox;
        *worldPosB = closestPoint;
    }

    return true;
}

// Recalculation if the sphere is contained in the box
float RecalculateIfSphereInBox(const Vector3f& boxHalfSize, const Vector3f& sphereRelativePos, Vector3f* closestPoint, Vector3f* normal) {
    // Initially, assign information on the +x surface
    float minDist = boxHalfSize.x() - sphereRelativePos.x(); // shortest distance (as penetration depth)
    float sign = 1.0f; // surface sign
    UINT8 axis = 0;    // Which axis

    // -x surface
    float surfaceDist = boxHalfSize.x() + sphereRelativePos.x();
    if (surfaceDist < minDist) {
        minDist = surfaceDist;
        sign = -1.0f;
        axis = 0;
    }

    // +y surface
    surfaceDist = boxHalfSize.y() - sphereRelativePos.y();
    if (surfaceDist < minDist) {
        minDist = surfaceDist;
        sign = 1.0f;
        axis = 1;
    }

    // -y surface
    surfaceDist = boxHalfSize.y() + sphereRelativePos.y();
    if (surfaceDist < minDist) {
        minDist = surfaceDist;
        sign = -1.0f;
        axis = 1;
    }

    // +z surface
    surfaceDist = boxHalfSize.z() - sphereRelativePos.z();
    if (surfaceDist < minDist) {
        minDist = surfaceDist;
        sign = 1.0f;
        axis = 2;
    }

    // -z surface
    surfaceDist = boxHalfSize.z() + sphereRelativePos.z();
    if (surfaceDist < minDist) {
        minDist = surfaceDist;
        sign = -1.0f;
        axis = 2;
    }

    // Update the position closest to the position of the sphere inside the box
    (*closestPoint)[axis] = boxHalfSize[axis] * sign;

    // Calculate normal vector of the surface
    *normal = Vector3f::Zero();
    (*normal)[axis] = sign;

    // Return the penetration depth
    return minDist;
}

// Calculate a vector that moves a point in the capsule onto its surface
Vector3f MoveInterPointOnCapsuleSurface(const Vector3f& pos, const Vector3f& dir, float halfLength, float radius, const Vector3f& interPoint) {
    // Project vector from collision point to center point in center axis direction
    Vector3f v = interPoint - pos;
    float t = dir.Dot(v);

    // Correct so as not to exceed the length of the capsule
    t = Utl::Clamp(t, -halfLength, halfLength);

    // Calculate the point on the center axis of the capsule closest to the contact point
    Vector3f closestPoint = pos + t * dir;

    // Calculate and normalize vector to surface direction
    Vector3f vDirSurface = interPoint - closestPoint;
    vDirSurface.Normalize();

    // The point on the surface is the point moved from the center point to the surface direction by the radius
    Vector3f ret = closestPoint + vDirSurface * radius;

    // Calculate the vector from the contact point to a point on the surface
    ret -= interPoint;
    return ret;
}
