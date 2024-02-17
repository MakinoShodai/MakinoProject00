/**
 * @file   CollisionAlgorithmCapsuleCapsule.h
 * @brief  This file handles collision algorithms for both 'capsule and capsule' and 'capsule and sphere'.
 *
 * @author Shodai Makino
 * @date   2023/07/29
 */

#ifndef __COLLISION_ALGORITHM_CAPSULE_CAPSULE_H__
#define __COLLISION_ALGORITHM_CAPSULE_CAPSULE_H__

#include "CollisionAlgorithm.h"

namespace Mkpe {
    namespace CollisionAlgorithm {
        /**
           @brief Capsule and capsule collision algorithm
           @param pair Pairs for collision determination
           @return Added contact points?
        */
        bool CapsuleCapsule(Dbvt::BVOverlapPair* pair);
        /**
           @brief Capsule and sphere collision algorithm
           @param pair Pairs for collision determination
           @return Added contact points?
        */
        bool CapsuleSphere(Dbvt::BVOverlapPair* pair);
    } // namespace CollisionAlgorithm
} // namespace Mkpe

#endif // !__COLLISION_ALGORITHM_BOX_SPHERE_H__
