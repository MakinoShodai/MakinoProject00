/**
 * @file   ContactPointMap.h
 * @brief  This file handles map to store contact points of collider.
 * 
 * @author Shodai Makino
 * @date   2024/1/14
 */

#ifndef __CONTACT_POINT_MAP_H__
#define __CONTACT_POINT_MAP_H__

#include "BVOverlapPair.h"
#include "Singleton.h"

namespace Mkpe {
    /** @brief Map to store contact points */
    class CContactPointMap : public ACMainThreadSingleton<CContactPointMap> {
        // Friend declaration
        friend ACSingletonBase;

    public:
        /** @brief Destructor */
        ~CContactPointMap() = default;

        /** @brief Feature for thread-safe */
        class CThreadSafeFeature : public ACInnerClass<CContactPointMap> {
        public:
            // Friend declaration
            using ACInnerClass<CContactPointMap>::ACInnerClass;

            /**
               @brief Build and get reference to contact data with exclusion control
               @param pair Pair to add contact points
               @return Reference to contact data
               @details
               If the element is unbuilt, it is built and returned
            */
            CContactData& BuildContactData(const Dbvt::BVOverlapPair& pair);

            /**
               @brief Get contact data
               @param pair Pair to get contact points
               @return If the pair has a contact point, return it, otherwise return nullptr
               @details
               This function is thread-safe unless called at the same time as 'BuildContactData'
            */
            CContactData* GetContactData(const Dbvt::BVOverlapPair& pair);
        };

        /** @brief Get feature for thread-safe */
        inline static CThreadSafeFeature& GetAny() {
            static CThreadSafeFeature instance(GetProtected().Get());
            return instance;
        }

    protected:
        /**
           @brief Constructor
        */
        CContactPointMap() : ACMainThreadSingleton(0) {}

    private:
        /** @brief Map to associate pair and contact points */
        std::unordered_map<OverlapPairForKey, CContactData, OverlapPairKeyHash> m_map;
        /** @brief Mutex when building an element on the map */
        std::mutex m_emplaceMutex;
    };
} // namespace Mkpe

#endif // !__CONTACT_POINT_MAP_H__
