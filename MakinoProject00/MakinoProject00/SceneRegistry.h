/**
 * @file   SceneRegistry.h
 * @brief  This file handles registry class that manages all scene.
 * 
 * @author Shodai Makino
 * @date   2023/12/9
 */

#ifndef __SCENE_REGISTRY_H__
#define __SCENE_REGISTRY_H__

#include "Singleton.h"
#include "Scene.h"
#include "SharedPtr.h"
#include "ThreadPool.h"
#include "UtilityForAsync.h"

 /** @brief Is this a child of ACScene? */
template <class T>
concept IsACSceneChild = std::is_base_of_v<ACScene, T>;

/** @brief This registry class manages all scene */
class CSceneRegistry : public ACMainThreadSingleton<CSceneRegistry> {
    // Friend declaration
    friend class ACSingletonBase;

public:
    /**
       @brief Destructor
    */
    ~CSceneRegistry() override = default;

    /**
       @brief Initialize
       @tparam T First scene class
    */
    template<IsACSceneChild T>
    void Initialize();

    /**
       @brief Processing per frame
    */
    void ProcessPerFrame();

    /**
       @brief Set next transion scene and end of current scene
       @tparam T Next scene class
    */
    template<IsACSceneChild T>
    void SetNextScene();

protected:
    /**
       @brief Constructor
    */
    CSceneRegistry() : ACMainThreadSingleton(50) {}

private:
    /** @brief Current scene */
    CSharedPtr<ACScene> m_currentScene;
    /** @brief Next scene */
    CSharedPtr<ACScene> m_nextScene;
    /** @brief Future object to load a next scene */
    Utl::Async::CMainThreadFuture<void> m_sceneLoadFuture;
};

// Initialize
template<IsACSceneChild T>
void CSceneRegistry::Initialize() {
    // Start loading scene
    SetNextScene<T>();

    // Wait for the scene to load
    if (m_sceneLoadFuture.Valid()) {
        m_sceneLoadFuture.Get();

        // Move a next scene to the current scene
        m_currentScene = std::move(m_nextScene);
    }
}

// Set next transion scene and end of current scene
template<IsACSceneChild T>
void CSceneRegistry::SetNextScene() {
    // Cancel if scene is currently loading
    if (m_sceneLoadFuture.Valid()) { return; }

    // Create an instance of a next scene
    m_nextScene = CSharedPtr<T>::Make();

    // Load the next scene in a new thread
    m_sceneLoadFuture = CThreadPool::GetAny().EnqueueTask([](CSharedPtr<ACScene> nextScene) {
        nextScene->Start();
    }, m_nextScene);
}

#endif // !__SCENE_REGISTRY_H__
