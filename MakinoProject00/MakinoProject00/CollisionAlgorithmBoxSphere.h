/**
 * @file   CollisionAlgorithmBoxSphere.h
 * @brief  This file handles collision algorithms for both 'box and sphere' and 'box and capsule'.
 *
 * @author Shodai Makino
 * @date   2023/07/27
 */

#ifndef __COLLISION_ALGORITHM_BOX_SPHERE_H__
#define __COLLISION_ALGORITHM_BOX_SPHERE_H__

#include "CollisionAlgorithm.h"

namespace Mkpe {
    namespace CollisionAlgorithm {
        /**
           @brief Box and sphere collision algorithm
           @param pair Pairs for collision determination
           @return Added contact points?
        */
        bool BoxSphere(Dbvt::BVOverlapPair* pair);
        /**
           @brief Box and capsule collision algorithm
           @param pair Pairs for collision determination
           @return Added contact points?
        */
        bool BoxCapsule(Dbvt::BVOverlapPair* pair);
    } // namespace CollisionAlgorithm
} // namespace Mkpe

#endif // !__COLLISION_ALGORITHM_BOX_SPHERE_H__
