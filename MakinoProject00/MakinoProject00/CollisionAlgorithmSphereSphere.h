/**
 * @file   CollisionAlgorithmSphereSphere.h
 * @brief  This file handles sphere and sphere collision algorithm.
 *
 * @author Shodai Makino
 * @date   2023/07/20
 */

#ifndef __COLLISION_ALGORITHM_SPHERE_SPHERE_H__
#define __COLLISION_ALGORITHM_SPHERE_SPHERE_H__

#include "CollisionAlgorithm.h"

namespace Mkpe {
    namespace CollisionAlgorithm {
        /**
           @brief Sphere and sphere collision algorithm
           @param pair Pairs for collision determination
           @return Added contact points?
        */
        bool SphereSphere(Dbvt::BVOverlapPair* pair);
    } // namespace CollisionAlgorithm
} // namespace Mkpe

#endif // !__COLLISION_ALGORITHM_SPHERE_SPHERE_H__
