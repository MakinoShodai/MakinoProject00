#include "Scene.h"
#include "CommandManager.h"
#include "StaticResourceRegistry.h"
#include "ApplicationClock.h"

// Initialize static member
ScenePhase CScene::ms_currentPhase = ScenePhase::Update;

// Constructor
CScene::CScene(CUniquePtr<ACRenderPassAsset> renderPass)
    : m_renderPass(std::move(renderPass))
    , m_physicsWorld(this)
    , m_cameraRegistry(CUniquePtrWeakable<CCameraRegistry>::Make())
    , m_lightRegistry(CUniquePtrWeakable<CLightRegistry>::Make())
    , m_isFirstUpdated(false)
    , m_currentUpdateMode(UpdateMode::Null)
    , m_scheduleUpdateMode(UpdateMode::Null)
{ }

// Processing when a scene is discarded
void CScene::OnDestroy() {
    // #NOTE : The object should be released first, since the destructor of the component must access m_meshKeyGenerater, etc.
    for (auto& it : m_gameObjects) {
        it->OnDestroy();
    }
    m_gameObjects.clear();
    for (auto& it : m_gameObjectsToCreate) {
        it->OnDestroy();
    }
    m_gameObjectsToCreate.clear();
    m_gameObjectsToDestroy.clear();
}

// Processing when switching to this scene
void CScene::Start() {
    m_renderPass->CallStart(WeakFromThis());
}

// Drawing process
void CScene::Draw() {
    m_renderPass->Draw();
}

// Update processing
void CScene::Update() {
    // Is update mode enabled?
    bool isUpdateMode = IsUpdateMode();

    // Create the game objects planned to be created
    if (!m_gameObjectsToCreate.empty()) {
        std::vector<CUniquePtrWeakable<CGameObject>> objects;
        objects.swap(m_gameObjectsToCreate);
        for (auto& it : objects) {
            it->Start();
            m_gameObjects.emplace_back(std::move(it));
        }
        objects.clear();
    }

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
        if (!it->IsActive() || (isUpdateMode && !it->CheckUpdateMode(m_currentUpdateMode))) { continue; }

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
        if (!it->IsActive() || (isUpdateMode && !it->CheckUpdateMode(m_currentUpdateMode))) { continue; }
        // Pre drawing processing
        it->PreDraw();
    }

    // Perform processing at the end of the update process from camera registry
    m_cameraRegistry->EndUpdate();
}

// Physics simulation preprocessing
void CScene::PrePhysicsSimulation() {
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
void CScene::PostPhysicsSimulation() {
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
void CScene::EndFrameProcess() {
    // Call end frame process of graphics pipeline state objects
    for (auto& it : m_gpsos) {
        it->EndFrameProcess();
    }

    // Call refresh process of dynamic structured buffer registry
    m_dynamicSbRegistry.FrameRefresh();

    // Set scheduled update mode to current update mode
    m_currentUpdateMode = m_scheduleUpdateMode;
}

// Transfer bit strings to observe the transform of all objects
void CScene::TransferObjectsTransformObserve() {
    for (auto& it : m_gameObjects) {
        it->TransferTransformObserve();
    }
}

// Set update mode
void CScene::SetUpdateMode(UpdateMode mode) {
    // If there is more than one update mode, do nothing.
    if (Utl::CountBits((UpdateModeType)mode) > 1) { return; }
    m_scheduleUpdateMode = mode;
}

// Create a game object
CWeakPtr<CGameObject> CScene::CreateGameObject(const Transformf& transform) {
    // Add created object to the array
    m_gameObjectsToCreate.emplace_back(CUniquePtrWeakable<CGameObject>::Make(this, transform));

    // Call prefab processing
    CWeakPtr<CGameObject> createdObject = m_gameObjectsToCreate.back().GetWeakPtr();
    createdObject->Prefab();

    return createdObject;
}

// Create a game object
CWeakPtr<CGameObject> CScene::CreateGameObject(const std::string& prefabName, const Transformf& transform) {
    // Create prefab
    CGameObject* obj = ACRegistrarForGameObject::CreateGameObject(prefabName, this, transform);
    if (obj == nullptr) {
        return nullptr;
    }

    // Add created object to the array
    m_gameObjectsToCreate.emplace_back(CUniquePtrWeakable<CGameObject>(obj));

    // Call prefab processing
    CWeakPtr<CGameObject> createdObject = m_gameObjectsToCreate.back().GetWeakPtr();
    createdObject->Prefab();

    return createdObject;
}

// Find a game object from the array of game objects
CWeakPtr<CGameObject> CScene::FindGameObject(const std::wstring& name) {
    // Existing game objects
    for (auto& it : m_gameObjects) {
        if (it == nullptr) { continue; }

        // Return if name matches
        const auto& objName = it->GetName();
        if (!objName.empty() && objName == name) {
            return it.GetWeakPtr();
        }
    }

    // Game objects to be created in the future
    for (auto& it : m_gameObjectsToCreate) {
        if (it == nullptr) { continue; }

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
void CScene::DestroyGameObject(CWeakPtr<CGameObject> object) {
    if (object == nullptr) { return; }
    m_gameObjectsToDestroy.push_back(object);
}

#ifdef _EDITOR
// Set rendering pass
void CScene::SetRenderPass(CUniquePtr<ACRenderPassAsset> renderPass) {
    m_renderPass = std::move(renderPass);
    if (m_renderPass) {
        m_renderPass->CallStart(WeakFromThis());
    }
    m_gpsos.clear();
}
#endif // _EDITOR

// Check if there are any objects to be destroyed, and if so, destroy them
void CScene::CheckObjectsDestroy() {
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
