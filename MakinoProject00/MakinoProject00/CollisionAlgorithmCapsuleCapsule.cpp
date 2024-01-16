#include "CollisionAlgorithmCapsuleCapsule.h"
#include "UtilityForComplexCalculations.h"
#include "CapsuleCollider3D.h"
#include "SphereCollider3D.h"

// Capsule and capsule intersection judgment
bool IntersectCapsuleCapsule(Mkpe::Dbvt::BVOverlapPair* pair,
    const Vector3f& posA, const Vector3f& posB, 
    const Vector3f& directionA, const Vector3f& directionB,
    float halfLengthA, float halfLengthB,
    float radiusA, float radiusB
);

// Capsule and capsule collision algorithm
bool Mkpe::CollisionAlgorithm::CapsuleCapsule(Dbvt::BVOverlapPair* pair) {
    // Get the two wrappers
    CColliderWrapper* wrapperA = pair->GetWrapperA();
    CColliderWrapper* wrapperB = pair->GetWrapperB();

    // Cast the colliders of the two wrappers to capsule collider
    const CCapsuleCollider3D* capsuleA = static_cast<const CCapsuleCollider3D*>(wrapperA->GetCollider().Get());
    const CCapsuleCollider3D* capsuleB = static_cast<const CCapsuleCollider3D*>(wrapperB->GetCollider().Get());

    // Get positions of the two capsules
    Vector3f posA = wrapperA->GetPos();
    Vector3f posB = wrapperB->GetPos();
    
    // Get rotation matrices of the two capsules
    Matrix3x3f matA = wrapperA->GetRotation().GetMatrix();
    Matrix3x3f matB = wrapperB->GetRotation().GetMatrix();

    // Get the indices of the center axes of the two capsules
    UINT8 axisIndexA = capsuleA->GetAxisIndex();
    UINT8 axisIndexB = capsuleB->GetAxisIndex();

    // Get the direction vectors of the center axes of the two capsules
    Vector3f directionA = Vector3f(matA(axisIndexA, Utl::Math::_X), matA(axisIndexA, Utl::Math::_Y), matA(axisIndexA, Utl::Math::_Z));
    Vector3f directionB = Vector3f(matB(axisIndexB, Utl::Math::_X), matB(axisIndexB, Utl::Math::_Y), matB(axisIndexB, Utl::Math::_Z));

    // Get half lengths of the axes of the two capsules
    float halfLengthA = capsuleA->GetScalingLength() * Utl::Inv::_2;
    float halfLengthB = capsuleB->GetScalingLength() * Utl::Inv::_2;

    // Get radii of the two capsules
    float radiusA = capsuleA->GetScalingRadius();
    float radiusB = capsuleB->GetScalingRadius();

    // Intersection judgment
    return IntersectCapsuleCapsule(pair, posA, posB, directionA, directionB, halfLengthA, halfLengthB, radiusA, radiusB);
}

// Capsule and sphere collision algorithm
bool Mkpe::CollisionAlgorithm::CapsuleSphere(Dbvt::BVOverlapPair* pair) {
    // Get the two wrappers
    CColliderWrapper* wrapperA = pair->GetWrapperA();
    CColliderWrapper* wrapperB = pair->GetWrapperB();

    // wrapper A is a capsule?
    bool isCapsuleA = (wrapperA->GetCollider()->GetColliderType() == ColliderType::Capsule);

    // Get the two wrappers of each colliders
    CColliderWrapper* capsuleWrapper = isCapsuleA ? wrapperA : wrapperB;
    CColliderWrapper* sphereWrapper = isCapsuleA ? wrapperB : wrapperA;

    // Cast the colliders of the two wrappers to each collider
    const CCapsuleCollider3D* capsule = static_cast<const CCapsuleCollider3D*>(capsuleWrapper->GetCollider().Get());
    const CSphereCollider3D* sphere = static_cast<const CSphereCollider3D*>(sphereWrapper->GetCollider().Get());

    // Get positions of the two colliders
    Vector3f posA = wrapperA->GetPos();
    Vector3f posB = wrapperB->GetPos();

    // Get rotation matrices of the two colliders
    Matrix3x3f matA = wrapperA->GetRotation().GetMatrix();
    Matrix3x3f matB = wrapperB->GetRotation().GetMatrix();

    // Get the indices of the center axes of the two colliders
    UINT8 axisIndexA = (isCapsuleA) ? capsule->GetAxisIndex() : Utl::Math::_Y;
    UINT8 axisIndexB = (isCapsuleA) ? Utl::Math::_Y : capsule->GetAxisIndex();

    // Get the direction vectors of the center axes of the two colliders
    Vector3f directionA = Vector3f(matA(axisIndexA, Utl::Math::_X), matA(axisIndexA, Utl::Math::_Y), matA(axisIndexA, Utl::Math::_Z));
    Vector3f directionB = Vector3f(matB(axisIndexB, Utl::Math::_X), matB(axisIndexB, Utl::Math::_Y), matB(axisIndexB, Utl::Math::_Z));

    // Get half lengths of the axes of the two colliders
    float halfLengthA = (isCapsuleA) ? capsule->GetScalingLength() * Utl::Inv::_2 : 0.0f;
    float halfLengthB = (isCapsuleA) ? 0.0f : capsule->GetScalingLength() * Utl::Inv::_2;

    // Get radii of the two colliders
    float radiusA = (isCapsuleA) ? capsule->GetScalingRadius() : sphere->GetScalingRadius();
    float radiusB = (isCapsuleA) ? sphere->GetScalingRadius() : capsule->GetScalingRadius();

    // Intersection judgment by treating sphere as a capsule with the same height and radius
    return IntersectCapsuleCapsule(pair, posA, posB, directionA, directionB, halfLengthA, halfLengthB, radiusA, radiusB);
}

// Capsule and capsule intersection judgment
bool IntersectCapsuleCapsule(Mkpe::Dbvt::BVOverlapPair* pair,
    const Vector3f& posA, const Vector3f& posB, 
    const Vector3f& directionA, const Vector3f& directionB, 
    float halfLengthA, float halfLengthB, 
    float radiusA, float radiusB) {

    // Calculate a shortest distance from the two line segments of the sent capsules
    Vector3f vTieClosestPoint;
    Vector3f offsetA, offsetB;
    float length = Utl::Calc::CalculateDistanceSegments(posB - posA, directionA, directionB, halfLengthA, halfLengthB, &offsetA, &offsetB, &vTieClosestPoint);

    // Subtract the two radii from length of the distance vector
    float dist = length - (radiusA + radiusB);

    // If the distance is greater than the two radii, they are not in contact
    if (dist > 0) {
        return false;
    }

    // #NOTE : It's a collision when it comes to this point.

    // If the colliders overlap each other, contact points are not added
    if (pair->CanOverlapAfterApproval()) {
        return false;
    }

    // If the distance is very small
    Vector3f normalOnB;
    if (length <= FLT_EPSILON) {
        // Let the appropriate tangent vector be the normal
        normalOnB = Utl::Math::GetTangentVector(directionA);
    }
    else {
        float invLength = 1.0f / length;
        normalOnB = -vTieClosestPoint * invLength;
    }
    Vector3f pointWorldOnB = posB + offsetB + normalOnB * radiusB;

    // Add the contact point
    pair->contactData.AddContactPoint(normalOnB, pointWorldOnB, dist);

    return true;
}
