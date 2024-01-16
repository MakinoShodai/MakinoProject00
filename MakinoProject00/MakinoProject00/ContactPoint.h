/**
 * @file   ContactPoint.h
 * @brief  This file handles contact point information between rigid bodies..
 * 
 * @author Shodai Makino
 * @date   2023/07/20
 */

#ifndef __CONTACT_POINT_H__
#define __CONTACT_POINT_H__

#include "SolverConstraint.h"
#include "StableHandleVector.h"

/** @brief The maximum number of contact points a colliding pair can have */
constexpr int CONTACTPOINT_MAX = 4;

namespace Mkpe {
    /** @brief Contact point structure */
    struct ContactPoint {
        /** @brief Contact point on bodyA in local coordinate space */
        Vector3f pointLocalOnA;
        /** @brief Contact point on bodyB in local coordinate space */
        Vector3f pointLocalOnB;
        /** @brief Contact point on bodyB in world coordinate space */
        Vector3f pointWorldOnB;
        /** @brief Normal on contact point on bodyB in world coordinate space */
        Vector3f normalWorldOnB;
        /** @brief Depth of penetration */
        float    penetDepth;
        /** @brief The combined coefficient of friction of two rigid bodies */
        float    combinedFriction;
        /** @brief The combined coefficient of restitution of two rigid bodies */
        float    combinedRestitution;
        
        /**
           @brief Constructor
           @param pointLocalOnA Contact point on bodyA in local coordinate space
           @param pointLocalOnB Contact point on bodyB in local coordinate space
           @param pointWorldOnB Contact point on bodyB in world coordinate space
           @param normalWorldOnB Normal on contact point on bodyB in world coordinate space
           @param penetDepth Depth of penetration
        */
        ContactPoint(const Vector3f& pointLocalOnA, const Vector3f& pointLocalOnB, const Vector3f& pointWorldOnB, const Vector3f& normalWorldOnB, float penetDepth)
            : pointLocalOnA(pointLocalOnA)
            , pointLocalOnB(pointLocalOnB)
            , pointWorldOnB(pointWorldOnB)
            , normalWorldOnB(normalWorldOnB)
            , penetDepth(penetDepth)
            , combinedFriction(0.0f)
            , combinedRestitution(0.0f)
        { }
    };

    /** @brief Contact point information for one collision pair */
    class CContactData {
    public:
        /** @brief Move is possible */
        CContactData(CContactData&&) = default;
        /** @brief Move is possible */
        CContactData& operator=(CContactData&&) = default;
        /** @brief Copy is not possible */
        CContactData(const CContactData&) = delete;
        /** @brief Copy is not possible */
        CContactData& operator=(const CContactData&) = delete;

        /**
           @brief Constructor
           @param wrapperA Weak pointer to first collider wrapper
           @param wrapperB Weak pointer to second collider wrapper
        */
        CContactData(const CWeakPtr<CColliderWrapper>& wrapperA, const CWeakPtr<CColliderWrapper>& wrapperB);
        
        /**
           @brief Destructor
        */
        ~CContactData() = default;

        /**
           @brief Add a contact point
           @param normalWorldOnB Normal on contact point on bodyB in world coordinate space
           @param pointWorldOnB Contact point on bodyB in world coordinate space
           @param penetDepth Depth of penetration
        */
        void AddContactPoint(const Vector3f& normalWorldOnB, const Vector3f& pointWorldOnB, float penetDepth);

        /**
           @brief Clear all contact points
        */
        void ClearContactPoint();

        /**
           @brief Is the depth sent in contact?
           @param penetDepth Depth to check for contact
           @return Is it treated as a contact?
        */
        static bool IsContact(float penetDepth);

        /** @brief Get the first collider wrapper */
        CColliderWrapper* GetWrapperA() const { return m_wrapperA.Get(); }
        /** @brief Get the second collider wrapper */
        CColliderWrapper* GetWrapperB() const { return m_wrapperB.Get(); }

        /** @brief Get the size of the array of contact points */
        int GetContactPointSize() { return m_nextPointIndex; }

        /** @brief Get the array of contact points */
        CUniquePtr<ContactPoint>* GetContactPointArray() { return m_contactPoint; }

        /** @brief Get a contact point */
        const ContactPoint* GetContactPoint(UINT index) const { return (index < m_nextPointIndex) ? m_contactPoint[index].Get() : nullptr; }

    private:
        /**
           @brief Add a contact point to the array of contact points
           @param newPoint Contact point to be added
        */
        void AddContactPointToArray(CUniquePtr<ContactPoint>& newPoint);

        /**
           @brief Find and return the most discardable a contact point in the array
           @param newPoint New contact point to be added.
           @return The most discardable a contact point in the array
        */
        UINT FindDiscardPoint(const ContactPoint& newPoint);

        /**
           @brief Find a contact point matching the specified argument and returns it as a discard target
           @param newPoint Contact point of search source
           @return If a matching contact point is found, its index is returned. If not found, return -1
        */
        UINT FindSimilarPoint(const ContactPoint& newPoint);

        /** @brief Get the combined coefficient of friction of two rigid bodies */
        float CalculateCombinedFriction(const RigidBodyMaterial& matA, const RigidBodyMaterial& matB);
        /** @brief Get the combined coefficient of restitution of two rigid bodies */
        float CalculateCombinedRestitution(const RigidBodyMaterial& matA, const RigidBodyMaterial& matB);

    private:
        /** @brief Weak pointer to first collider wrapper */
        CWeakPtr<CColliderWrapper> m_wrapperA;
        /** @brief Weak pointer to second collider wrapper */
        CWeakPtr<CColliderWrapper> m_wrapperB;
        /** @brief Array of all contacts points */
        CUniquePtr<ContactPoint> m_contactPoint[CONTACTPOINT_MAX];
        /** @brief Threshold for determining that they are the same point */
        float m_similarPointThreshold;
        /** @brief Element number of the next contact point to be stored */
        UINT m_nextPointIndex;
    };
} // namespace Mkpe

#endif // !__CONTACT_POINT_H__
