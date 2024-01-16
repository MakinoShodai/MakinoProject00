/**
 * @file   CollisionAlgorithmBoxBox.h
 * @brief  This file handles box and box collision algorithm.
 * 
 * @author Shodai Makino
 * @date   2023/08/02
 */

#ifndef __COLLISION_ALGORITHM_BOX_BOX_H__
#define __COLLISION_ALGORITHM_BOX_BOX_H__

#include "CollisionAlgorithm.h"

namespace Mkpe {
    namespace CollisionAlgorithm {
        /**
           @brief Box and box collision algorithm
           @param pair Pairs for collision determination
           @return Added contact points?
        */
        bool BoxBox(Dbvt::BVOverlapPair* pair);
    } // namespace CollisionAlgorithm
} // namespace Mkpe

#endif // !__COLLISION_ALGORITHM_BOX_BOX_H__
