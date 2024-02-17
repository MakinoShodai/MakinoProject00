/**
 * @file   PhysicsWorld.h
 * @brief  This file handles physics simulation environment class.
 * 
 * @author Shodai Makino
 * @date   2023/07/18
 */

#ifndef __PHYSICS_WORLD_H__
#define __PHYSICS_WORLD_H__

#include "Collider3D.h"
#include "RigidBody.h"
#include "DynamicBVTree.h"
#include "ColliderWrapper.h"
#include "ConstraintSolverDirector.h"
#include "ScenePhase.h"

// Forward declaration
class CScene;

namespace Mkpe {
    /**
       @brief Structure exclusive to the physics engine
       @attention
       Don't access from outside the physics engine
    */
    struct PhysicsEngineExclusiveData {
        /** @brief Dynamic array of computational rigid bodies that only non-static rigid bodies have */
        StableHandleVector<CUniquePtrWeakable<CSimulationBody>> simulationBodies;
        /** @brief Dynamic array of all collider wrappers */
        StableHandleVector<CUniquePtrWeakable<CColliderWrapper>> allWrappers;
        /** @brief Dynamic array of all rigid bodies */
        StableHandleVector<CWeakPtr<CRigidBody>> allRigidBodies;
    };

    /** @brief Physics simulation environment class */
    class CPhysicsWorld {
    public:
        /**
           @brief Constructor
           @param scene Scene that has this class
        */
        CPhysicsWorld(CScene* scene);
        /**
           @brief Destructor
        */
        ~CPhysicsWorld() = default;

        /**
           @brief Initialization process at start frame
           @details
           Must be called after Start processing of all objects
        */
        void StartInitialize();

        /**
           @brief Advance simulation time
           @param timeStep Elapsed time from previous frame
        */
        void StepSimulation(float timeStep);

        /**
           @brief Reassign fixed time step based on current frame time of the application
        */
        void ReassignFixedTimeStep();

        /** @brief Get data exclusive to the physics engine */
        PhysicsEngineExclusiveData* GetExclusiveData() { return &m_exclusiveData; }
        /** @brief Get data exclusive to the physics engine */
        const PhysicsEngineExclusiveData* GetExclusiveData() const { return &m_exclusiveData; }
        /** @brief Get bounding volume tree */
        Dbvt::CDynamicBVTree* GetBVTree() { return &m_bvTree; }

        /** @brief Check the transform has changed for position */
        inline static bool CheckTransformObserveForPos(CGameObject* gameObj) { return ms_phaseForPosCheck.CheckTransformObserve(gameObj, TransformObserve::Pos); }
        /** @brief Check the transform has changed for scale */
        inline static bool CheckTransformObserveForScale(CGameObject* gameObj) { return ms_phaseForScaleCheck.CheckTransformObserve(gameObj, TransformObserve::Scale); }
        /** @brief Check the transform has changed for rotation */
        inline static bool CheckTransformObserveForRotate(CGameObject* gameObj) { return ms_phaseForRotateCheck.CheckTransformObserve(gameObj, TransformObserve::Rotate); }

        /**
           @brief Get current time step 
           @details
           Returns the correct timeStep if the current Phase is PrePhysicsUpdate, Physics, or OnCollision, otherwise returns 0
        */
        inline static float GetTimeStep() { return ms_currentTimeStep; }

    private:
        /**
           @brief Perform physics calculations
           @param invTimeStep Inverse of elapsed time from previous frame
        */
        void PhysicsSimulation(float invTimeStep);

        /**
           @brief Collision detection
        */
        void CollisionDetection();

        /**
           @brief Broad phase. Bounding volumes are used to create potentially colliding pairs
        */
        void BroadPhase();
        /**
           @brief Update leaf nodes in the bounding volume tree
        */
        void UpdateLeafNode();
        /**
           @brief Update leaf nodes of dynamic colliders in the bounding volume tree
           @param wrapper Collider to be updated
        */
        void UpdateDynamicColliderNode(CColliderWrapper* wrapper);
        /**
           @brief Update bounding volume overlap pairs
        */
        void UpdateBVOverlapPair();

        /**
           @brief Collision detection between actual geometries
        */
        void NarrowPhase();

    private:
        /** @brief Scene phases for position check. Holds the phase that the physical engine currently wants to check */
        static ScenePhasesWrapper ms_phaseForPosCheck;
        /** @brief Scene phases for scale check. Holds the phase that the physical engine currently wants to check */
        static ScenePhasesWrapper ms_phaseForScaleCheck;
        /** @brief Scene phases for rotate check. Holds the phase that the physical engine currently wants to check */
        static ScenePhasesWrapper ms_phaseForRotateCheck;
        /** @brief Time step of current physics simulation */
        static float ms_currentTimeStep;

        /** @brief Scene that has this class */
        CScene* m_scene;
        /** @brief Data exclusive to the physics engine */
        PhysicsEngineExclusiveData m_exclusiveData;
        /** @brief Class that perform constraint solving */
        CConstraintSolverDirector m_solverDirector;
        /** @brief Dynamic bounding volume tree */
        Dbvt::CDynamicBVTree m_bvTree;
        /** @brief How many physics simulations are allowed in per frame? */
        int   m_maxSimulationOnFrame;
        /** @brief Advance the physical simulation for each this fixed time step */
        float m_fixedTimeStep;
        /** @brief What is the maximum amount of time that can proceed in a single physics simulation? */
        float m_maxTimeStep;
        /** @brief Total elapsed time for the excess not reflected in the physics simulation */
        float m_totalSurplusTime;
        /** @brief Is the first simulation of the current frame not finished? */
        bool m_isFirstSimulationCurrentFrame;
    };
} // namespace Mkpe

#endif // !__PHYSICS_WORLD_H__
