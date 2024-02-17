/**
 * @file   Scene.h
 * @brief  This file handles scene class.
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
#include "PhysicsWorld.h"
#include "ScenePhase.h"
#include "LightRegistry.h"
#include "RenderPassAsset.h"

/** @brief Scene class */
class CScene : public ACWeakPtrFromThis<CScene> {
public:
    /**
       @brief Constructor
       @param renderPass Rendering pass asset to be used
    */
    CScene(CUniquePtr<ACRenderPassAsset> renderPass);

    /**
       @brief Processing when a scene is discarded
    */
    void OnDestroy();

    /**
       @brief Processing when switching to this scene
       @details
       This function is called outside the main thread
    */
    void Start();

    /**
       @brief Drawing process
    */
    void Draw();

    /**
       @brief Updating process
    */
    void Update();

    /**
       @brief Physics simulation preprocessing
    */
    void PrePhysicsSimulation();

    /**
       @brief Physics simulation postprocessing
    */
    void PostPhysicsSimulation();

    /**
       @brief Processing at end of a frame
    */
    void EndFrameProcess();

    /**
       @brief Transfer bit strings to observe the transform of all objects
    */
    void TransferObjectsTransformObserve();

    /**
       @brief Set update mode
       @param mode Update mode to be set
       @details
       The update mode switches at the end of the frame.
       Only one update mode can be enabled at a time
    */
    void SetUpdateMode(UpdateMode mode);

    /**
       @brief Create a game object
       @param transform Transform of new game object
       @return Weak pointer to the created game object
    */
    CWeakPtr<CGameObject> CreateGameObject(const Transformf& transform);

    /**
       @brief Create a game object
       @param prefabName Prefab name of game object to be created
       @param transform Transform of new game object
       @return Weak pointer to the created game object
    */
    CWeakPtr<CGameObject> CreateGameObject(const std::string& prefabName, const Transformf& transform);

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

    /** @brief Get physics simulation environment class */
    Mkpe::CPhysicsWorld* GetPhysicsWorld() { return &m_physicsWorld; }
    /** @brief Get physics simulation environment class */
    const Mkpe::CPhysicsWorld* GetPhysicsWorld() const { return &m_physicsWorld; }

    /** @brief Get weak pointer to camera registry */
    CWeakPtr<CCameraRegistry> GetCameraRegistry() { return m_cameraRegistry.GetWeakPtr(); }
    /** @brief Get weak pointer to camera registry */
    CWeakPtr<const CCameraRegistry> GetCameraRegistry() const { return m_cameraRegistry.GetWeakPtr(); }

    /** @brief Get weak pointer to lights registry */
    CWeakPtr<CLightRegistry> GetLightRegistry() { return m_lightRegistry.GetWeakPtr(); }

    /** @brief Get registry class handles all structured buffer allocator per object */
    CDynamicSbRegistry* GetDynamicSbRegistry() { return &m_dynamicSbRegistry; }
    /** @brief Get pointer to the registry class handles graphics components array of each graphics layers */
    CGraphicsLayerRegistry* GetGraphicsLayerRegistry() { return &m_graphicsLayerRegistry; }
    /** @brief Get key generater class for mesh buffer */
    CIntKeyGenerater* GetMeshKeyGenerater() { return &m_meshKeyGenerater; }

    /** @brief Get current phase of the scene currently being processed */
    inline static ScenePhase GetCurrentScenePhase() { return ms_currentPhase; }

    /** @brief Is update mode enabled? */
    bool IsUpdateMode() { return (UpdateModeType)m_currentUpdateMode != 0; }
    /** @brief Logical conjunction with current update mode bits */
    bool CheckUpdateMode(UpdateMode mode) { return Utl::CheckEnumBit(m_currentUpdateMode & mode); }

#ifdef _EDITOR
    /**
       @brief Set rendering pass
       @param renderPass Rendering pass asset to be used
    */
    void SetRenderPass(CUniquePtr<ACRenderPassAsset> renderPass);
#endif // _EDITOR

private:
    /**
       @brief Check if there are any objects to be destroyed, and if so, destroy them
    */
    void CheckObjectsDestroy();

private:
    /** @brief Current phase of the scene currently being processed */
    static ScenePhase ms_currentPhase;

    /** @brief Rendering pass asset to be used */
    CUniquePtr<ACRenderPassAsset> m_renderPass;
    /** @brief Dynamic array of game objects */
    std::vector<CUniquePtrWeakable<CGameObject>> m_gameObjects;
    /** @brief Game objects to be created in the future */
    std::vector<CUniquePtrWeakable<CGameObject>> m_gameObjectsToCreate;
    /** @brief Game objects to be destroyed in the future */
    std::vector<CWeakPtr<CGameObject>> m_gameObjectsToDestroy;
    /** @brief Dynamic array to pointers to graphics pipeline state object */
    std::vector<CGraphicsPipelineState*> m_gpsos;

    /** @brief Physics simulation environment class */
    Mkpe::CPhysicsWorld m_physicsWorld;
    /** @brief Camera registry class */
    CUniquePtrWeakable<CCameraRegistry> m_cameraRegistry;
    /** @brief Registry class for lights */
    CUniquePtrWeakable<CLightRegistry> m_lightRegistry;
    /** @brief Registry class handles all structured buffer allocator per object */
    CDynamicSbRegistry m_dynamicSbRegistry;
    /** @brief Registry class handles graphics components array of each graphics layers */
    CGraphicsLayerRegistry m_graphicsLayerRegistry;
    /** @brief Key generater class for mesh buffer */
    CIntKeyGenerater m_meshKeyGenerater;
    /** @brief Finished first update process? */
    bool m_isFirstUpdated;
    /** @brief Current update mode of this scene */
    UpdateMode m_currentUpdateMode;
    /** @brief Switches to this update mode at the end of a scene */
    UpdateMode m_scheduleUpdateMode;
};

/** @brief Is this a child of CScene? */
template <class T>
concept IsCSceneChild = std::is_base_of_v<CScene, T>;


#endif // !__SCENE_H__
