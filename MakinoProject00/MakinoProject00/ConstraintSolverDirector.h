/**
 * @file   ConstraintSolverDirector.h
 * @brief  This file create and resolve constraints for contact resolution.
 * 
 * @author Shodai Makino
 * @date   2023/07/23
 */

#ifndef __CONSTRAINT_SOLVER_DIRECTOR_H__
#define __CONSTRAINT_SOLVER_DIRECTOR_H__

#include "SolverConstraint.h"
#include "ContactPoint.h"

namespace Mkpe {
    // Forward declaration
    class CPhysicsWorld;

    /** @brief Function for contact solving operations of simulation body */
    using SimulationBodySolveFunc = void (CSimulationBodyBase::*)(const Vector3f&, const Vector3f&, float);
    /** @brief Function to get velocity of simulation body */
    using SimulationBodyGetVelocityFunc = const Vector3f& (CSimulationBodyBase::*)() const;

    /** @brief Class that create and resolve constraints for contact resolution */
    class CConstraintSolverDirector {
    public:
        /**
           @brief Constructor
           @param physicsWorld Physics simulation environment class owning this class
        */
        CConstraintSolverDirector(CPhysicsWorld* physicsWorld);

        /**
           @brief Create constraints in the current frame
           @param invTimeStep Inverse of elapsed time from previous frame
        */
        void CreateConstraint(float invTimeStep);

        /**
           @brief Contact solving with an iterative solver
        */
        void IterativeSolver();

        /**
           @brief Clear all constraints
        */
        void ClearConstraint();

    private:
        /**
           @brief Create contact constraint
           @param solver Constraint class of the destination to be created
           @param cp Contact point information of two rigid bodies
           @param wrapperA First collider wrapper
           @param wrapperB Second collider wrapper
           @param bodyA First rigid body
           @param bodyB Second rigid body
           @param relativePosA Relative position of contact point from the first collider
           @param relativePosB Relative position of contact point from the second collider
           @param isMoveA Does A move with this constraint?
           @param isMoveB Does B move with this constraint?
           @param invTimeStep Inverse of elapsed time from previous frame
        */
        void CreateContactConstraint(SolverConstraint* solver, ContactPoint* cp,
            CColliderWrapper* wrapperA, CColliderWrapper* wrapperB,
            CSimulationBodyBase* bodyA, CSimulationBodyBase* bodyB,
            const Vector3f& relativePosA, const Vector3f& relativePosB,
            bool isMoveA, bool isMoveB, float invTimeStep);

        /**
           @brief Create friction constraint of First rigid body
           @param frictionDirA Direction of friction vector
           @param solver Constraint class of the destination to be created
           @param cp Contact point information of two rigid bodies
           @param wrapperA First collider wrapper
           @param wrapperB Second collider wrapper
           @param bodyA First rigid body
           @param bodyB Second rigid body
           @param relativePosA Relative position of contact point from the first collider
           @param relativePosB Relative position of contact point from the second collider
           @param isMoveA Does A move with this constraint?
           @param isMoveB Does B move with this constraint?
           @param invTimeStep Inverse of elapsed time from previous frame
        */
        void CreateFrictionConstraint(const Vector3f& frictionDirA, 
            SolverConstraint* solver, ContactPoint* cp,
            CColliderWrapper* wrapperA, CColliderWrapper* wrapperB,
            CSimulationBodyBase* bodyA, CSimulationBodyBase* bodyB,
            const Vector3f& relativePosA, const Vector3f& relativePosB, 
            bool isMoveA, bool isMoveB, float invTimeStep);

        /**
           @brief Calculate denominator of impulse and return relative velocity in the direction of repulsion
           @param dirA Linear movement direction of A
           @param dirB Linear movement direction of B
           @param solver Constraint class of the destination to be created
           @param cp Contact point information of two rigid bodies
           @param wrapperA First collider wrapper
           @param wrapperB Second collider wrapper
           @param bodyA First rigid body
           @param bodyB Second rigid body
           @param relativePosA Relative position of contact point from the first collider
           @param relativePosB Relative position of contact point from the second collider
           @param isMoveA Does A move with this constraint?
           @param isMoveB Does B move with this constraint?
           @return Relative velocity in the direction of repulsion
        */
        float CalculateDenomOfImpulse(const Vector3f& dirA, const Vector3f& dirB, SolverConstraint* solver, ContactPoint* cp,
            CColliderWrapper* wrapperA, CColliderWrapper* wrapperB,
            CSimulationBodyBase* bodyA, CSimulationBodyBase* bodyB,
            const Vector3f& relativePosA, const Vector3f& relativePosB,
            bool isMoveA, bool isMoveB);

        /**
           @brief Penetration solving with an iterative solver
        */
        void IterativeSolverPenetrationImpulse();

        /**
           @brief Solve constraints only once
           @return The maximum value of position error
        */
        float SingleIterationConstraintSolver();

        /**
           @brief Solve penetration impulse
           @param solver Constraint to be solved
           @return The maximum value of position error
        */
        float SolverPenetrationImpulse(SolverConstraint* solver);
        
        /**
           @brief Solve velocity impulse for contact solver
           @param solver Constraint to be solved
           @return The maximum value of position error
        */
        float SolverContactVelocityImpulse(SolverConstraint* solver);

        /**
           @brief Solve friction impulse for contact solver
           @param solver Constraint to be solved
           @return The maximum value of position error
        */
        float SolverFrictionVelocityImpulse(SolverConstraint* solver);

        /**
           @brief Solve impulse
           @param solver Constraint to be solved
           @param impulse Impulse to be applied
           @param linearFunc Function to get linear velocity of simulation body
           @param angularFunc Function to get angular velocity of simulation body
           @param solveFunc Function for contact solving operations of simulation body
           @param isUpperLimit Use upper limit for impulse?
           @return The maximum value of position error
        */
        float SolverImpulse(SolverConstraint* solver, float impulse,
            SimulationBodyGetVelocityFunc linearFunc, 
            SimulationBodyGetVelocityFunc angularFunc,
            SimulationBodySolveFunc solveFunc,
            bool isUpperLimit = false);

    private:
        /** @brief contact constraints */
        std::vector<SolverConstraint> m_contactConstraints;
        /** @brief friction constraints */
        std::vector<SolverConstraint> m_frictionConstraints;
        /** @brief Physics simulation environment class owning this class */
        CPhysicsWorld* m_physicsWorld;
        /** @brief Maximum number of iterative solver steps */
        int m_iterativeSolverMax;
    };
} // namespace Mkpe

#endif // !__CONSTRAINT_SOLVER_DIRECTOR_H__
