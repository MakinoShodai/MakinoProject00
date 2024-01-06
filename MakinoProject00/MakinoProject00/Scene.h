/**
 * @file   Scene.h
 * @brief  This file handles abstract class for scene.
 * 
 * @author Shodai Makino
 * @date   2023/12/1
 */

#ifndef __SCENE_H__
#define __SCENE_H__

#include "UniquePtr.h"
#include "GameObject.h"
#include "CameraRegistry.h"
#include "GraphicsLayerRegistry.h"
#include "GraphicsPipelineState.h"
#include "DynamicSbRegistry.h"
#include "IntKeyGenerater.h"

/** @brief Is this a child class of CGameObject? */
template <class T>
concept IsCGameObjectChild = std::is_base_of_v<CGameObject, T>;

/** @brief This is abstract class for scene */
class ACScene : public ACWeakPtrFromThis<ACScene> {
public:
    /**
       @brief Constructor
    */
    ACScene();

    /**
       @brief Destructor
    */
    virtual ~ACScene();

    /**
       @brief Processing when switching to this scene
       @details
       This function is called outside the main thread
    */
    virtual void Start() = 0;

    /**
       @brief Drawing process
    */
    virtual void Draw() = 0;

    /**
       @brief Updating process
    */
    void Update();

    /**
       @brief Processing at end of a frame
    */
    void EndFrameProcess();

    /**
       @brief Create a game object
       @return Weak pointer to the created game object
    */
    template<IsCGameObjectChild T, class... Args>
    CWeakPtr<T> CreateGameObject(Args&&... args);

    /**
       @brief Find a game object from the array of game objects
       @param name Name of game object to be found
       @return Weak pointer to the found game object
    */
    CWeakPtr<CGameObject> FindGameObject(const std::wstring& name);

    /**
       @brief Schedule destroying a game object from the array of game objects
       @param object game object to be destroyed
    */
    void DestroyGameObject(CWeakPtr<CGameObject> object);

    /**
       @brief Add a pointer to a graphics pipeline state
       @param pipelineState Pointer to a graphics pipeline state to be added
       @details
       This function is called from inside CGraphicsPipelineState, so there is no need to call it from anywhere else.
    */
    void AddGraphicsPipelineStatePointer(CGraphicsPipelineState* pipelineState) { if (pipelineState != nullptr) { m_gpsos.push_back(pipelineState); } }

    /** @brief Get weak pointer to camera registry */
    CWeakPtr<CCameraRegistry> GetCameraRegistry() { return m_cameraRegistry.GetWeakPtr(); }
    /** @brief Get weak pointer to camera registry */
    const CWeakPtr<CCameraRegistry> GetCameraRegistry() const { return m_cameraRegistry.GetWeakPtr(); }
    /** @brief Get registry class handles all structured buffer allocator per object */
    CDynamicSbRegistry* GetDynamicSbRegistry() { return &m_dynamicSbRegistry; }
    /** @brief Get pointer to the registry class handles graphics components array of each graphics layers */
    CGraphicsLayerRegistry* GetGraphicsLayerRegistry() { return &m_graphicsLayerRegistry; }
    /** @brief Get key generater class for mesh buffer */
    CIntKeyGenerater* GetMeshKeyGenerater() { return &m_meshKeyGenerater; }

private:
    /** @brief Dynamic array of game objects */
    std::vector<CUniquePtrWeakable<CGameObject>> m_gameObjects;
    /** @brief Game objects to be created in the future */
    std::vector<CUniquePtrWeakable<CGameObject>> m_gameObjectsToCreate;
    /** @brief Game objects to be destroyed in the future */
    std::vector<CWeakPtr<CGameObject>> m_gameObjectsToDestroy;
    /** @brief Dynamic array to pointers to graphics pipeline state object */
    std::vector<CGraphicsPipelineState*> m_gpsos;

    /** @brief Camera registry class */
    CUniquePtrWeakable<CCameraRegistry> m_cameraRegistry;
    /** @brief Registry class handles all structured buffer allocator per object */
    CDynamicSbRegistry m_dynamicSbRegistry;
    /** @brief Registry class handles graphics components array of each graphics layers */
    CGraphicsLayerRegistry m_graphicsLayerRegistry;
    /** @brief Key generater class for mesh buffer */
    CIntKeyGenerater m_meshKeyGenerater;
    /** @brief Finished first update process? */
    bool m_isFirstUpdated;
};

// Create a game object
template<IsCGameObjectChild T, class... Args>
CWeakPtr<T> ACScene::CreateGameObject(Args&&... args) {
    // Create game object to the array
    m_gameObjectsToCreate.emplace_back(CUniquePtrWeakable<T>::Make(this, std::forward<Args>(args)...));

    // Call prefab processing
    CWeakPtr<T> createdObject = m_gameObjectsToCreate.back().GetWeakPtr();
    createdObject->Prefab();

    return createdObject;
}

#endif // !__SCENE_H__
