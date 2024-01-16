/**
 * @file   CollisionAlgorithm.h
 * @brief  This file handles all collision algorithms.
 * 
 * @author Shodai Makino
 * @date   2023/07/20
 */
#ifndef __COLLISION_ALGORITHM_H__
#define __COLLISION_ALGORITHM_H__

#include "Singleton.h"
#include "ColliderWrapper.h"
#include "BVOverlapPair.h"

namespace Mkpe {
    /**
       @brief Function type of collision algorithm
       @return Added contact points?
    */
    using CollisionAlgorithmFunc = bool(*)(Dbvt::BVOverlapPair*);

    /** @brief This class handles all collision algorithms */
    class CCollisionAlgorithm : public ACMainThreadSingleton<CCollisionAlgorithm> {
        // Friend declaration
        friend class ACSingletonBase<CCollisionAlgorithm>;

    public:
        /**
           @brief Destrcutor
        */
        ~CCollisionAlgorithm() = default;

        /**
           @brief Broad phase collision algorithm
           @param pair Pairs for collision determination
        */
        void BroadContact(Dbvt::BVOverlapPair* pair);

    private:
        /**
           @brief Constructor
        */
        CCollisionAlgorithm();

    private:
        /** @brief Variable that stores the contact judgment function between each shape */
        CollisionAlgorithmFunc m_algorithmFunc[(UINT)ColliderType::Max][(UINT)ColliderType::Max];
    };
} // namespace Mkpe

#endif // !__COLLISION_ALGORITHM_H__
