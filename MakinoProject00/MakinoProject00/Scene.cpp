#include "Scene.h"
#include "CommandManager.h"
#include "StaticResourceRegistry.h"
#include "ApplicationClock.h"

// Initialize static member
ScenePhase ACScene::ms_currentPhase = ScenePhase::Update;

// Constructor
ACScene::ACScene() 
    : m_physicsWorld(this)
    , m_cameraRegistry(CUniquePtrWeakable<CCameraRegistry>::Make())
    , m_isFirstUpdated(false)
{ }

// Destructor
ACScene::~ACScene() {
    // #NOTE : The object should be released first, since the destructor of the component must access m_meshKeyGenerater, etc.
    m_gameObjects.clear();
    m_gameObjectsToCreate.clear();
    m_gameObjectsToDestroy.clear();
}

// Update processing
void ACScene::Update() {
    // Create the game objects planned to be created
    for (auto& it : m_gameObjectsToCreate) {
        it->Start();
        m_gameObjects.emplace_back(std::move(it));
    }
    m_gameObjectsToCreate.clear();

    // First frame processing
    if (m_isFirstUpdated == false) {
        // Raise the flag
        m_isFirstUpdated = true;
        // Send this scene to the static resource registry
        CStaticResourceRegistry::GetMain().SceneStart(this);

        // Initialize physics simulation environment class
        m_physicsWorld.StartInitialize();
    }

    // Phase to Update and transfer the bit strings to observe the transforms
    ms_currentPhase = ScenePhase::Update;
    TransferObjectsTransformObserve();

    // Update the existing game objects
    for (auto& it : m_gameObjects) {
        // Active check
        if (!it->IsActive()) { continue; }
        // Update processing
        it->Update();
    }

    // Destroy the game objects planned to be destroyed
    CheckObjectsDestroy();

    // If the phase remains Update, there will be problems with processing in the physics engine, so move to PrePhysicsUpdate
    ms_currentPhase = ScenePhase::PrePhysicsUpdate;

    // Advance simulation time
    m_physicsWorld.StepSimulation(CAppClock::GetMain().GetDeltaTime());

    // Pre drawing process by the existing game objects
    for (auto& it : m_gameObjects) {
        // Active check
        if (!it->IsActive()) { continue; }
        // Pre drawing processing
        it->PreDraw();
    }

    // Perform processing at the end of the update process from camera registry
    m_cameraRegistry->EndUpdate();
}

// Physics simulation preprocessing
void ACScene::PrePhysicsSimulation() {
    // Phase to PrePhysicsUpdate and transfer the bit strings to observe the transforms
    ms_currentPhase = ScenePhase::PrePhysicsUpdate;
    TransferObjectsTransformObserve();

    // Call processing for collider collisions
    for (auto& it : m_gameObjects) {
        // Active check
        if (!it->IsActive()) { continue; }
        it->PrePhysicsUpdate();
    }

    // Destroy the game objects planned to be destroyed
    CheckObjectsDestroy();

    // Phase to Physics and transfer the bit strings to observe the transforms
    ms_currentPhase = ScenePhase::Physics;
    TransferObjectsTransformObserve();
}

// Physics simulation postprocessing
void ACScene::PostPhysicsSimulation() {
    // Phase to OnCollision and transfer the bit strings to observe the transforms
    ms_currentPhase = ScenePhase::OnCollision;
    TransferObjectsTransformObserve();

    // Call processing for collider collisions
    for (auto& it : m_gameObjects) {
        // Active check
        if (!it->IsActive()) { continue; }
        it->OnCollision();
    }

    // Destroy the game objects planned to be destroyed
    CheckObjectsDestroy();
}

// Processing at end of a frame
void ACScene::EndFrameProcess() {
    // Call end frame process of graphics pipeline state objects
    for (auto& it : m_gpsos) {
        it->EndFrameProcess();
    }

    // Call refresh process of dynamic structured buffer registry
    m_dynamicSbRegistry.FrameRefresh();
}

// Transfer bit strings to observe the transform of all objects
void ACScene::TransferObjectsTransformObserve() {
    for (auto& it : m_gameObjects) {
        it->TransferTransformObserve();
    }
}

// Find a game object from the array of game objects
CWeakPtr<CGameObject> ACScene::FindGameObject(const std::wstring& name) {
    // Existing game objects
    for (auto& it : m_gameObjects) {
        // Return if name matches
        const auto& objName = it->GetName();
        if (!objName.empty() && objName == name) {
            return it.GetWeakPtr();
        }
    }

    // Game objects to be created in the future
    for (auto& it : m_gameObjectsToCreate) {
        // Return if name matches
        const auto& objName = it->GetName();
        if (!objName.empty() && objName == name) {
            return it.GetWeakPtr();
        }
    }

    // return nullptr
    return CWeakPtr<CGameObject>(nullptr);
}

// Schedule destroying a game object from the array of game objects
void ACScene::DestroyGameObject(CWeakPtr<CGameObject> object) {
    if (object == nullptr) { return; }
    m_gameObjectsToDestroy.push_back(object);
}

// Check if there are any objects to be destroyed, and if so, destroy them
void ACScene::CheckObjectsDestroy() {
    for (auto& objectToDestroy : m_gameObjectsToDestroy) {
        // Instance check
        CGameObject* instance = objectToDestroy.Get();
        if (instance == nullptr) { continue; }

        // Find and destroy matching the object
        for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); ++it) {
            if (it->Get() == instance) {
                // Call processing at instance destruction from components
                instance->OnDestroy();

                // Erase game object from array
                m_gameObjects.erase(it);
                break;
            }
        }
    }
    m_gameObjectsToDestroy.clear();
}
