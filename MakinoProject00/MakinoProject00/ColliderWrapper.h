/**
 * @file   ColliderWrapper.h
 * @brief  This file handles wrapper class that exists in a 1:1 relationship with a collider.
 * 
 * @author Shodai Makino
 * @date   2023/07/23
 */

#ifndef __COLLIDER_WRAPPER_H__
#define __COLLIDER_WRAPPER_H__

#include "Collider3D.h"
#include "RigidBody.h"
#include "BVNode.h"
#include "SimulationBody.h"
#include "ContactColliderList.h"

namespace Mkpe {
    /**
       @brief Wrapper class that exists in a 1:1 relationship with a collider
       @details
       Relates a collider to its rigid body. Accessible only from inside the physics engine
    */
    class CColliderWrapper : public ACWeakPtrFromThis<CColliderWrapper> {
    public:
        /**
           @brief Constructor
           @param collider Weak pointer to collider to be related to this wrapper
        */
        CColliderWrapper(CWeakPtr<ACCollider3D> collider);
        
        /**
           @brief Destructor
        */
        ~CColliderWrapper();

        /**
           @brief Add information of a contacted opponent by this collider
           @param opponent Collider of the contacted opponent
           @param pairInfo Pair information with the contacted opponent
        */
        inline void AddContactedOpponent(const CWeakPtr<ACCollider3D>& opponent, const Mkpe::Dbvt::BVOverlapPair* pairInfo) { if (!IsStatic()) { m_contactColliderList.AddOpponent(opponent, pairInfo); } }

        /** @brief Rebuild the lists of the contact colliders for each categories */
        inline void RebuildContactLists() { m_contactColliderList.RebuildList(); }
        /** @brief Clear lists of the contact colliders */
        inline void ClearContactLists() { m_contactColliderList.AllClear(); }


        /** @brief Is this object static? */
        inline bool IsStatic() const { return m_rigidBody == nullptr || m_rigidBody->IsStatic(); }

        /** @brief Get the rigid body material */
        inline const RigidBodyMaterial& GetMaterial() const { return (m_rigidBody == nullptr) ? ms_defaultRbMaterial : m_rigidBody->GetMaterial(); }


        /** @brief Get the center coordinates considering offset */
        inline Vector3f GetPos() { return m_collider->GetCenter(); }
        /** @brief Get the rotation of the collider */
        inline const Quaternionf& GetRotation() { return m_collider->GetTransform().rotation; }
        /** @brief Get the scale of the collider */
        inline const Vector3f& GetScale() { return m_collider->GetTransform().scale; }


        /** @brief Get the inverse mass of rigid body */
        inline float GetInvMass() const { return (!m_simulationBody) ? 0.0f : m_rigidBody->GetInvMass(); }
        /** @brief Get the mass of rigid body */
        inline float GetMass() const { return (!m_simulationBody) ? 0.0f : m_rigidBody->GetMass(); }


        /** @brief Get the maximum extent to which a vertex of collider is shifted by rotation */
        inline float GetMaxRotateScalar() { return m_collider->GetMaxRotateScalar(); }

        /** @brief Initialize inertia tensor */
        void InitializeInertiaTensor() { if (m_simulationBody) { GetSimulationBody()->InitializeInertiaTensor(m_collider.Get()); } }

        /** @brief Processing of any scaling or rotation changes */
        inline void CheckScalingRotation() { m_collider->CheckScalingRotation(); if (m_simulationBody) { GetSimulationBody()->CheckScalingRotation(m_collider.Get()); } }


        /** @brief Get the simulation body related to this wrapper */
        CSimulationBodyBase* GetSimulationBody() { return (m_simulationBody) ? m_simulationBody.Get() : &ms_staticSimulationBody; }
        /** @brief Set rigid body to be related to this wrapper */
        void SetRigidBody(CWeakPtr<CRigidBody> rb) { m_rigidBody = rb; }
        /** @brief Set simulation body to be related to this wrapper */
        void SetSimulationBody(CWeakPtr<CSimulationBody> simulationBody) { m_simulationBody = simulationBody; }


        /** @brief Get the collider related to this wrapper */
        CWeakPtr<ACCollider3D> GetCollider() { return m_collider; }

        /** @brief Set the tree type of this wrapper */
        void SetTreeType(Dbvt::TREETYPE type) { m_thisTreeType = type; }
        /** @brief Get the tree type of this wrapper */
        Dbvt::TREETYPE GetTreeType() { return m_thisTreeType; }


        /** @brief Set node with this wrapper in the tree */
        void SetNode(Dbvt::Node* node) { m_thisNode = node; }
        /** @brief Get node with this wrapper in the tree */
        Dbvt::Node* GetNode() const { return m_thisNode; }

        /** @brief Is the ID sent the same as my leaf node ID? */
        bool CheckMyLeafNodeID(Mkpe::Dbvt::NodeID id) const { return (m_thisNode) ? id == m_thisNode->nodeID : false; }


        /** @brief Is this collider and the gameobject that has this collider active? */
        bool IsActive() const { return m_collider->IsActiveOverall(); }


        /** @brief Get list class of the contact colliders for each category */
        const CContactColliderList& GetContactColliderList() const { return m_contactColliderList; }

    private:
        /** @brief Rigid body material of collider without rigid body */
        static const RigidBodyMaterial ms_defaultRbMaterial;
        /** @brief Temporary variable for collider without rigid body */
        static CSimulationBodyBase ms_staticSimulationBody;

        /** @brief Weak pointer to the simulation body of this wrapper */
        CWeakPtr<CSimulationBody> m_simulationBody;
        /** @brief Weak pointer to collider related to this wrapper */
        CWeakPtr<ACCollider3D> m_collider;
        /** @brief Weak pointer to rigid body related to this wrapper */
        CWeakPtr<CRigidBody> m_rigidBody;
        /** @brief Type of tree to which this wrapper belongs */
        Dbvt::TREETYPE m_thisTreeType;
        /** @brief Node with this wrapper in the tree */
        Dbvt::Node* m_thisNode;

        /** @brief List class of the contact colliders for each category */
        CContactColliderList m_contactColliderList;
    };
} // namespace Mkpe

#endif // !__COLLIDER_WRAPPER_H__
