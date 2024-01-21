/**
 * @file   SimulationBody.h
 * @brief  This file handles rigid body class for computing.
 * 
 * @author Shodai Makino
 * @date   2023/07/22
 */

#ifndef __SIMULATIONBODY_H__
#define __SIMULATIONBODY_H__

#include "Collider3D.h"

// Forward declaration
class CRigidBody;

/** @brief How to apply force */
enum class ForceMode {
    /** @brief Continuous application of force */
    Force,
    /** @brief Instantaneous application of force */
    Impulse,
};

namespace Mkpe {
    /**
       @brief Rigid body class for computing
       @details
       This base class is not writable
    */
    class CSimulationBodyBase {
    public:
        /**
           @brief Constructor
           @param rb The rigid body that has this simulation body
        */
        CSimulationBodyBase(CRigidBody* rb);

        /**
           @brief Apply velocity for Solving velocity at contact
           @param linearVelocity Linear velocity to be applied
           @param angularVelocity Angular velocity to be applied
           @param impulseMagnitude Magnitude of impulse
        */
        virtual void SolveVelocityAtContact(const Vector3f& linearVelocity, const Vector3f& angularVelocity, float impulseMagnitude) {}

        /**
           @brief Apply velocity for Solving penetration at contact
           @param linearVelocity Linear velocity to be applied
           @param angularVelocity Angular velocity to be applied
           @param impulseMagnitude Magnitude of impulse
        */
        virtual void SolvePenetrationAtContact(const Vector3f& linearVelocity, const Vector3f& angularVelocity, float impulseMagnitude) {}

        /**
           @brief Add force to linear velocity
           @param force Force to be added
           @param mode How to apply force
        */
        virtual void AddForce(const Vector3f& force, ForceMode mode) {}

        /**
           @brief Add torque to angular velocity
           @param torque Torque to be added
           @param mode How to apply torque
        */
        virtual void AddTorque(const Vector3f& torque, ForceMode mode) {}

        /**
           @brief Clear all velocities updated every frame
        */
        virtual void ClearVelocitiesEveryFrame() {}

        /**
           @brief Clear all velocities updated per physics simulation
        */
        virtual void ClearVelocitiesPerSimulation() {}

        /**
           @brief Clear all velocities
        */
        virtual void ClearAllVelocities() {}

        /**
           @brief Clear pseudo velocities
        */
        virtual void ClearPseudoVelocities() {}

        /**
           @brief Merge pseudo velocity and regular velocity
        */
        virtual void MergeVelocities() {}

        /**
           @brief Decay velocity
        */
        virtual void DecayVelocity() {}

        /**
           @brief Apply velocity to transform
        */
        virtual void UpdateTransform() {}

        /**
           @brief Set gravity
           @param gravity Gravity to be set
        */
        virtual void SetGravity(float gravity) {}

        /**
           @brief Apply gravity to linear velocity
        */
        virtual void ApplyGravity() {}

        /**
           @brief Calculate the velocity of point p in the object
           @param p The point at which you want to calculate the speed
           @return Velocity of point p
        */
        virtual Vector3f GetVelocityOfPointInObject(const Vector3f& p) const { return Vector3f::Zero(); }

        /**
           @brief Initialize inertia tensor
           @param collider Collider associated with this simulation body
        */
        virtual void InitializeInertiaTensor(ACCollider3D* collider) {}

        /**
           @brief Processing of any scaling or rotation changes
           @param collider Collider associated with this simulation body
        */
        virtual void CheckScalingRotation(ACCollider3D* collider) {}

        /**
           @brief Compute pseudo velocity in current time per frame
           @param invTimeStep Inverse of lapsed time from previous time
        */
        virtual void ComputePseudoVelocityPerFrame(float invTimeStep) {}

        /**
           @brief Compute pseudo velocity in current time per simulation
           @param invTimeStep Inverse of lapsed time from previous time
        */
        virtual void ComputePseudoVelocityPerSimulation(float invTimeStep) {}

        /**
           @brief Update transform in previous time of physics simulation
        */
        virtual void UpdatePrevTransform() {}


        /** @brief Set aerodynamic drag for movement by force */
        virtual void SetLinearDrag(float drag) {}
        /** @brief Set aerodynamic drag for rotation by torque */
        virtual void SetAngularDrag(float drag) {}


        /** @brief Is this simulation body active? */
        virtual bool IsActive() const { return false; }


        /** @brief Get the linear velocity */
        const Vector3f& GetLinearVelocity() const { return m_linearVelocity; }
        /** @brief Get the angular velocity */
        const Vector3f& GetAngularVelocity() const { return m_angularVelocity; }

        /** @brief Amount of change in linear velocity caused by contact */
        const Vector3f& GetDeltaLinearVelocity() const { return m_deltaLinearVelocity; }
        /** @brief Amount of change in angular velocity caused by contact */
        const Vector3f& GetDeltaAngularVelocity() const { return m_deltaAngularVelocity; }
        /** @brief Linear velocity for solving penetration */
        const Vector3f& GetPenetrationLinearVelocity() const { return m_penetrationLinearVelocity; }
        /** @brief Angular velocity for solving penetration */
        const Vector3f& GetPenetrationAngularVelocity() const { return m_penetrationAngularVelocity; }

        /** @brief Get total linear velocity */
        const Vector3f& GetTotalLinearVelocity() const { return m_totalLinearVelocity; }
        /** @brief Get total angular velocity */
        const Vector3f& GetTotalAngularVelocity() const { return m_totalAngularVelocity; }

        /** @brief Get allowable vector for movement */
        const Vector3f& GetMoveAllowable() const;
        /** @brief Get allowable vector for rotation */
        const Vector3f& GetRotateAllowable() const;

        /** @brief Inverse of the inertia tensor */
        const Matrix3x3f& GetInvInertiaTensor() const { return m_invInertiaTensor; }

    protected:
        /**
           @brief Own rigid body
           @details Lifetime of rigid body is never longer than simulated body
        */
        CRigidBody* m_rb;

        /** @brief Linear velocity that is the sum of pseudo-linear velocity and linear velocity */
        Vector3f m_totalLinearVelocity;
        /** @brief Angular velocity that is the sum of pseudo-angular velocity and angular velocity */
        Vector3f m_totalAngularVelocity;

        /** @brief Linear velocity */
        Vector3f m_linearVelocity;
        /** @brief Angular velocity */
        Vector3f m_angularVelocity;

        /** @brief Amount of change in linear velocity caused by contact */
        Vector3f m_deltaLinearVelocity;
        /** @brief Amount of change in angular velocity caused by contact */
        Vector3f m_deltaAngularVelocity;
        /** @brief Linear velocity for solving penetration */
        Vector3f m_penetrationLinearVelocity;
        /** @brief Angular velocity for solving penetration */
        Vector3f m_penetrationAngularVelocity;

        /** @brief Direct coordinate movement converted to pseudo linear velocity for per frame */
        Vector3f m_pseudoLinearVelocityPerFrame;
        /** @brief Direct coordinate movement converted to pseudo angular velocity for per frame */
        Vector3f m_pseudoAngularVelocityPerFrame;
        /** @brief Direct coordinate movement converted to pseudo linear velocity for per simulation */
        Vector3f m_pseudoLinearVelocityPerSimulation;
        /** @brief Direct coordinate movement converted to pseudo angular velocity for per simulation */
        Vector3f m_pseudoAngularVelocityPerSimulation;

        /** @brief Gravity */
        float m_gravity;
        /** @brief Aerodynamic drag for movement by force */
        float m_linearDrag;
        /** @brief Aerodynamic drag for rotation by torque */
        float m_angularDrag;

        /** @brief Inverse of the inertia tensor */
        Matrix3x3f m_invInertiaTensor;
        /** @brief Local inverse of the inertia tensor. Abbreviated to 3-D vectors */
        Vector3f m_invInertiaLocal;

        /** @brief Position in previous time of physics simulation */
        Vector3f m_prevPos;
        /** @brief Rotation in previous time of physics simulation */
        Quaternionf m_prevRotation;
    };

    /**
       @brief Rigid body class for computing
       @details
       This derived class is writable
    */
    class CSimulationBody : public CSimulationBodyBase {
    public:
        /** @brief Constructor is inherited */
        using CSimulationBodyBase::CSimulationBodyBase;

        /**
           @brief Apply velocity for Solving velocity at contact
           @param linearVelocity Linear velocity to be applied
           @param angularVelocity Angular velocity to be applied
           @param impulseMagnitude Magnitude of impulse
        */
        void SolveVelocityAtContact(const Vector3f& linearVelocity, const Vector3f& angularVelocity, float impulseMagnitude) override;

        /**
           @brief Apply velocity for Solving penetration at contact
           @param linearVelocity Linear velocity to be applied
           @param angularVelocity Angular velocity to be applied
           @param impulseMagnitude Magnitude of impulse
        */
        void SolvePenetrationAtContact(const Vector3f& linearVelocity, const Vector3f& angularVelocity, float impulseMagnitude) override;

        /**
           @brief Add force to linear velocity
           @param force Force to be added
           @param mode How to apply force
        */
        void AddForce(const Vector3f& force, ForceMode mode) override;

        /**
           @brief Add torque to angular velocity
           @param torque Torque to be added
           @param mode How to apply torque
        */
        void AddTorque(const Vector3f& torque, ForceMode mode) override;

        /**
           @brief Clear all velocities updated every frame
        */
        void ClearVelocitiesEveryFrame() override;

        /**
           @brief Clear all velocities updated per physics simulation
        */
        void ClearVelocitiesPerSimulation() override;

        /**
           @brief Clear all velocities
        */
        void ClearAllVelocities() override;

        /**
           @brief Clear pseudo velocities
        */
        void ClearPseudoVelocities() override;

        /**
           @brief Merge pseudo velocity and regular velocity
        */
        void MergeVelocities() override;

        /**
           @brief Decay velocity
        */
        void DecayVelocity() override;

        /**
           @brief Apply velocity to transform
        */
        void UpdateTransform() override;

        /**
           @brief Set gravity
           @param gravity Gravity to be set
        */
        void SetGravity(float gravity) override { m_gravity = gravity; }

        /**
           @brief Apply gravity to linear velocity
        */
        void ApplyGravity() override;

        /**
           @brief Calculate the velocity of point p in the object
           @param p The point at which you want to calculate the speed
           @return Velocity of point p
        */
        Vector3f GetVelocityOfPointInObject(const Vector3f& p) const override;

        /**
           @brief Initialize inertia tensor
           @param collider Collider associated with this simulation body
        */
        void InitializeInertiaTensor(ACCollider3D* collider) override;

        /**
           @brief Processing of any scaling or rotation changes
           @param collider Collider associated with this simulation body
        */
        void CheckScalingRotation(ACCollider3D* collider) override;

        /**
           @brief Compute pseudo velocity in current time per frame
           @param invTimeStep Inverse of lapsed time from previous time
        */
        void ComputePseudoVelocityPerFrame(float invTimeStep) override;

        /**
           @brief Compute pseudo velocity in current time per simulation
           @param invTimeStep Inverse of lapsed time from previous time
        */
        void ComputePseudoVelocityPerSimulation(float invTimeStep) override;

        /**
           @brief Update transform in previous time of physics simulation
        */
        void UpdatePrevTransform() override;

        /** @brief Set aerodynamic drag for movement by force */
        void SetLinearDrag(float drag) override { m_linearDrag = drag; }
        /** @brief Set aerodynamic drag for rotation by torque */
        void SetAngularDrag(float drag) override { m_angularDrag = drag; }

        /** @brief Is this simulation body active? */
        bool IsActive() const override;
    };
} // namespace Mkpe

#endif // !__SIMULATIONBODY_H__
