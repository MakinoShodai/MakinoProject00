/**
 * @file   SolverConstraint.h
 * @brief  This file handles constraint solver class.
 * 
 * @author Shodai Makino
 * @date   2023/07/22
 */

#ifndef __SOLVER_CONSTRAINT_H__
#define __SOLVER_CONSTRAINT_H__

#include "ColliderWrapper.h"
#include "StableHandleVector.h"

namespace Mkpe {
    /** @brief Constraint solver class */
    struct SolverConstraint {
        /** @brief Weak pointer to the first collider wrapper */
        CWeakPtr<CColliderWrapper> wrapperA;
        /** @brief Weak pointer to the second collider wrapper */
        CWeakPtr<CColliderWrapper> wrapperB;
        /** @brief Normal direction of linear velocity of the first collider */
        Vector3f linearNormalA;
        /** @brief Normal direction of linear velocity of the second collider */
        Vector3f linearNormalB;
        /** @brief Torque for the first collider */
        Vector3f torqueA;
        /** @brief Torque for the second collider */
        Vector3f torqueB;
        /** @brief Axis of rotation vector for the first collider */
        Vector3f angularAxisA;
        /** @brief Axis of rotation vector for the second collider */
        Vector3f angularAxisB;
        /** @brief Denominator of impulse */
        float denomOfImpulse;
        /** @brief Total value of impact for contact solving */
        float appliedImpulse;
        /** @brief Impulse required for velocity solving */
        float velocityImpulse;
        /** @brief Impulse required for penetration solving */
        float penetrationImpulse;
        /** @brief Friction */
        float friction;

        /** @brief Lower limit of friction */
        float lowerLimit;
        /** @brief Upper limit of friction */
        float upperLimit;

        /** @brief Index of the constraint solver to be synchronized with this constraint solver */
        int synchroSolverIndex;

        /**
           @brief Constructor
           @param wrapperA Weak pointer to the first collider wrapper
           @param wrapperB Weak pointer to the second collider wrapper
        */
        SolverConstraint(CWeakPtr<CColliderWrapper> wrapperA, CWeakPtr<CColliderWrapper> wrapperB);

        /** @brief Get the first collider wrapper */
        CColliderWrapper* GetWrapperA() { return wrapperA.Get(); }
        /** @brief Get the second collider wrapper */
        CColliderWrapper* GetWrapperB() { return wrapperB.Get(); }
    };
} // namespace Mkpe

#endif // !__SOLVER_CONSTRAINT_H__
