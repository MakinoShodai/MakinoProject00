/**
 * @file   GJKAlgorithm.h
 * @brief  This file handles GJK and EPA collision algorithm.
 *
 * @author Shodai Makino
 * @date   2023/09/10
 */

#ifndef __GJK_ALGORITHM_H__
#define __GJK_ALGORITHM_H__

#include "CollisionAlgorithm.h"

namespace Mkpe {
    namespace CollisionAlgorithm {
        /**
           @brief GJK(Gilbert-Johnson-Keerthi) collision algorithm
           @param pair Pairs for collision determination
           @return Added contact points?
        */
        bool GJKAlgorithm(Dbvt::BVOverlapPair* pair);
    } // namespace CollisionAlgorithm
} // namespace Mkpe

#endif // !__GJK_ALGORITHM_H__
