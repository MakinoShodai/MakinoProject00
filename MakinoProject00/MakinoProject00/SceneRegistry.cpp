#include "SceneRegistry.h"
#include "CommandManager.h"
#include "DynamicCbRegistry.h"
#include "StaticResourceRegistry.h"
#include "DescriptorHeapPool.h"

// Processing per frame
void CSceneRegistry::ProcessPerFrame() {
    // If current scene doesn't exists, do nothing
    if (!m_currentScene) { return; }

    // If a next scene is being loaded, wait for it to finish
    if (m_sceneLoadFuture.Valid()) {
        m_sceneLoadFuture.Get();

        // Wait for present is called
        CSwapChain::GetMain().WaitForPresent();
        // Wait for GPU
        CCommandManager::GetAny().WaitForGPU();

        // Have the current scene destroyed in a new thread
        CThreadPool::GetAny().EnqueueTask([](CSharedPtr<ACScene> scene) {
            scene.Reset();
        }, std::move(m_currentScene));

        // Move a next scene to the current scene
        m_currentScene = std::move(m_nextScene);
    }

    // Updating process
    m_currentScene->Update();

    // Drawing preparation processing
    CDescriptorHeapPool::GetMain().ReadyToDraw();

    // Drawing process
    m_currentScene->Draw();

    // Processing at end of a frame
    m_currentScene->EndFrameProcess();

    // Refresh CB allocater and static resource allocater
    CDynamicCbRegistry::GetMain().FrameRefresh();
    CStaticResourceRegistry::GetMain().FrameRefresh();

    // Swap and draw the backbuffer
    CSwapChain::GetMain().DrawAndSwap();
}
