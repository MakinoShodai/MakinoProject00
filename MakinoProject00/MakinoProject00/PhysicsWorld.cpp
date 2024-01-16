#include "PhysicsWorld.h"
#include "CollisionAlgorithm.h"
#include "Scene.h"
#include "ApplicationClock.h"

// Initialize static member
ScenePhasesWrapper Mkpe::CPhysicsWorld::ms_phaseForPosCheck = {};
ScenePhasesWrapper Mkpe::CPhysicsWorld::ms_phaseForScaleCheck = {};
ScenePhasesWrapper Mkpe::CPhysicsWorld::ms_phaseForRotateCheck = {};
float Mkpe::CPhysicsWorld::ms_currentTimeStep = 0.0f;

// Constructor
Mkpe::CPhysicsWorld::CPhysicsWorld(ACScene* scene)
    : m_scene(scene)
    , m_solverDirector(this)
    , m_bvTree()
    , m_fixedTimeStep(CAppClock::GetMain().GetFrameTime())
    , m_maxSimulationOnFrame(5)
    , m_maxTimeStep(0.1f)
    , m_totalSurplusTime(0.0f) 
    , m_isFirstSimulationCurrentFrame(false)
{ }

// Initialization process at start frame
void Mkpe::CPhysicsWorld::StartInitialize() {
    // Initialize dynamic bounding volume tree
    m_bvTree.Initialize();
}

// Advance simulation time
void Mkpe::CPhysicsWorld::StepSimulation(float timeStep) {
    // Variable declarations
    int   currentFrameSimulationNum = 0;    // How many physics simulations should be performed in the current frame?
    float totalSimulationTime       = 0.0f; // Total time to perform the simulation

    // Add the timeStep to the excess time, and if it exceeds the time allowed for the physics simulation
    m_totalSurplusTime += timeStep;
    if (m_totalSurplusTime >= m_fixedTimeStep) {
        // Calculate how many physics simulations need to be performed and subtract just the time to perform the physics simulation from the excess time
        currentFrameSimulationNum = static_cast<int>(m_totalSurplusTime / m_fixedTimeStep);
        totalSimulationTime = currentFrameSimulationNum * m_fixedTimeStep;
        m_totalSurplusTime -= totalSimulationTime;
    }

    // If the number of simulations is more than one
    if (currentFrameSimulationNum) {
        // Correct so as not to exceed the upper limit of the number of simulations per frame
        int realSimulationNum = (currentFrameSimulationNum > m_maxSimulationOnFrame)
            ? m_maxSimulationOnFrame : currentFrameSimulationNum;

        // Calculate how much time to advance per physics simulation.
        ms_currentTimeStep = totalSimulationTime / realSimulationNum;
        ms_currentTimeStep = (ms_currentTimeStep > m_maxTimeStep) ? m_maxTimeStep : ms_currentTimeStep;
        float invTimeStep = 1.0f / ms_currentTimeStep;

        {
            // Set Update as the phase to check all the information that the transform has
            ms_phaseForPosCheck.SetPhases({ ScenePhase::Update });
            ms_phaseForRotateCheck.SetPhases({ ScenePhase::Update });
            ms_phaseForScaleCheck.SetPhases({ ScenePhase::Update });

            // Check transform has changes for scaling or rotation
            for (auto& it : m_exclusiveData.allWrappers) {
                it->CheckScalingRotation();
            }
        }

        {
            // Set Update and OnCollision as the phase to check the pos and the rotation
            ms_phaseForPosCheck.SetPhases({ ScenePhase::Update, ScenePhase::OnCollision });
            ms_phaseForRotateCheck.SetPhases({ ScenePhase::Update, ScenePhase::OnCollision });

            // Update values that need to be updated for all simulation bodies
            for (auto& it : m_exclusiveData.simulationBodies) {
                if (false == it->IsActive()) { continue; }
                // Required Processing
                it->ApplyGravity();
                it->ComputePseudoVelocityPerFrame(invTimeStep);
            }
        }

        // Perform physics simulation for the number of times calculated
        m_isFirstSimulationCurrentFrame = false;
        for (int i = 0; i < realSimulationNum; ++i) {
            PhysicsSimulation(invTimeStep);
        }

        // Clear velocities of all non static object
        for (auto& it : m_exclusiveData.simulationBodies) {
            it->ClearVelocitiesEveryFrame();
        }
    }

    ms_currentTimeStep = 0.0f;
}

// Perform physics calculations
void Mkpe::CPhysicsWorld::PhysicsSimulation(float invTimeStep) {
    // Preprocessing of scene
    m_scene->PrePhysicsSimulation();

    {
        // Set PrePhysicsUpdate or OnCollision as the phase to check the scale
        ms_phaseForScaleCheck.SetPhases({ ScenePhase::PrePhysicsUpdate, ScenePhase::OnCollision });
        // Set PrePhysicsUpdate or OnCollision or PhysicsSimulation as the phase to check the rotation
        ms_phaseForRotateCheck.SetPhases({ ScenePhase::PrePhysicsUpdate, ScenePhase::Physics, ScenePhase::OnCollision });

        // Check transform has changes
        for (auto& it : m_exclusiveData.allWrappers) {
            it->CheckScalingRotation();
        }
    }

    {
        // It is first simulation
        if (m_isFirstSimulationCurrentFrame) {
            // Set PrePhysicsUpdate as the phase to check the position and the rotation
            ms_phaseForPosCheck.SetPhases({ ScenePhase::PrePhysicsUpdate });
            ms_phaseForRotateCheck.SetPhases({ ScenePhase::PrePhysicsUpdate });
        }
        // It is not first simulation
        else {
            // Set PrePhysicsUpdate as the phase to check the position and the rotation
            ms_phaseForPosCheck.SetPhases({ ScenePhase::PrePhysicsUpdate, ScenePhase::OnCollision });
            ms_phaseForRotateCheck.SetPhases({ ScenePhase::PrePhysicsUpdate, ScenePhase::OnCollision });
        }

        // Compute pseudo velocity per simulation and merge velocities
        for (auto& it : m_exclusiveData.simulationBodies) {
            // If it is active, compute pseudo velocity per simulation
            if (it->IsActive()) {
                it->ComputePseudoVelocityPerSimulation(invTimeStep);
            }
            // Merge velocities
            it->MergeVelocities();
        }
    }

    // Collision detection
    CollisionDetection();

    // Create constraint solvers
    m_solverDirector.CreateConstraint(invTimeStep);

    // Collision resolution by iterative solvers
    m_solverDirector.IterativeSolver();

    // Clear constraints
    m_solverDirector.ClearConstraint();

    // Update information on all non-static rigid bodies
    for (auto& it : m_exclusiveData.simulationBodies) {
        if (false == it->IsActive()) { continue; }
        // Update transform
        it->UpdateTransform();
        
        // Update velocities
        it->DecayVelocity();
        it->ClearVelocitiesPerSimulation();
        
        // Update prevTransform
        it->UpdatePrevTransform();
    }

    // Rebuild contact lists for all colliders
    for (auto& it : m_exclusiveData.allWrappers) {
        it->RebuildContactLists();
    }

    // Postprocessing of scene
    m_scene->PostPhysicsSimulation();

    // First simulation in current frame is finished
    m_isFirstSimulationCurrentFrame = true;
}

// Collision detection
void Mkpe::CPhysicsWorld::CollisionDetection() {
    BroadPhase();

    NarrowPhase();
}

// Broad phase. Bounding volumes are used to create potentially colliding pairs
void Mkpe::CPhysicsWorld::BroadPhase() {
    UpdateLeafNode();

    UpdateBVOverlapPair();
}

// Update leaf nodes in the bounding volume tree
void Mkpe::CPhysicsWorld::UpdateLeafNode() {
    for (auto& it : m_exclusiveData.allWrappers) {
        Dbvt::Node* node = it->GetNode();

        // Ignore colliders without nodes
        if (node == nullptr) { continue; }

        // Clear node discard type
        node->leafData.discardPairType = Dbvt::PairDiscardType::None;

        // Non-active collider flags the pair as destroyed and exits
        if (!it->IsActive()) {
            node->leafData.discardPairType = Dbvt::PairDiscardType::Discard;
            continue;
        }

        // Processing for static collider
        if (it->IsStatic()) {
            // Move a collider that has changed from a dynamic collider to a static collider
            // #NOTE : Conversely, when a static collider becomes dynamic, the tree moves only when it moves
            if (it->GetTreeType() == Dbvt::TREETYPE_DYNAMIC) {
                // Reinsert a node and receive a new node
                node = m_bvTree.ReInsertLeafNode(it.Get());
            }

            // Pair needs to be judged.
            node->leafData.discardPairType = Dbvt::PairDiscardType::Check;
        }
        // Processing for dynamic collider
        else {
            UpdateDynamicColliderNode(it.Get());
        }
    }
}

// Update leaf nodes of dynamic colliders in the bounding volume tree
void Mkpe::CPhysicsWorld::UpdateDynamicColliderNode(CColliderWrapper* wrapper) {
    // Variable declarations
    Dbvt::Node* node = wrapper->GetNode(); // Node of own collider
    bool        isNeedRejudgement = false; // Is rejudgement needed?

    // Generate bounding volume from collider
    BoundingVolume bv = wrapper->GetCollider()->GenerateBoudingVolume();

    // If it's in contact with the original bounding volume, we'll determine that it's moving normally.
    if (node->bv.IsIntersect(bv)) {
        // If it's slightly out of the original bounding volume, it realigns itself in the tree
        if (false == node->bv.IsContain(bv)) {
            // Expand BV to some extent
            // #NOTE : To avoid the need to realign the tree with a small amount of movement
            bv.Expand(m_bvTree.GetBVMargin());

            // Reinsert node
            node = m_bvTree.ReInsertLeafNode(wrapper, bv);

            // Need rejudgement
            isNeedRejudgement = true;
        }
    }
    // Determine that it moved instantaneously.
    else {
        // Reinsert node
        node = m_bvTree.ReInsertLeafNode(wrapper, bv);

        // Need rejudgement
        isNeedRejudgement = true;
    }

    // If rejudgement needed
    if (isNeedRejudgement) {
        // Recreate pairs for dynamic and static trees
        m_bvTree.NodeCollisionDetection(node, m_bvTree.GetRootNode(Dbvt::TREETYPE_DYNAMIC));
        m_bvTree.NodeCollisionDetection(node, m_bvTree.GetRootNode(Dbvt::TREETYPE_STATIC));

        // Discard old pairs
        node->leafData.discardPairType = Dbvt::PairDiscardType::Discard;
    }
}

// Update bounding volume overlap pairs
void Mkpe::CPhysicsWorld::UpdateBVOverlapPair() {
    // Merge previous and current pairs
    m_bvTree.GetPairManager()->MergePairs();

    // Update size of the array at previous physics simulation
    m_bvTree.GetPairManager()->UpdatePrevSize();
}

// Collision detection between actual geometries
void Mkpe::CPhysicsWorld::NarrowPhase() {
    auto* pairs = m_bvTree.GetPairManager()->GetPair();
    for (auto& it : *pairs) {
        CCollisionAlgorithm::GetMain().BroadContact(&it);
    }
}
