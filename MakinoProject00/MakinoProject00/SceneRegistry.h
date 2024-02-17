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
#include "ImguiHelper.h"
#include "SceneFileSystem.h"

/** @brief Result for loading scene */
enum class SceneLoadResult {
    /** @brief Success */
    Success,
    /** @brief Failed to load scene file */
    FailedSceneFile,
    /** @brief Render path assets could not be found */
    FailedRenderPassAsset,
};

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
    */
    void Initialize();

    /**
       @brief Processing per frame
    */
    void ProcessPerFrame();

    /**
       @brief Set next transion scene and end of current scene
       @param sceneName Name of a next scene
    */
    void SetNextScene(const std::string& sceneName);

#ifdef _EDITOR
    /** @brief Display fps? */
    bool IsDisplayFPS() { return m_isDisplayFps; }
#endif // _EDITOR

    /** @brief Feature for thread-safe */
    class CThreadSafeFeature : public ACInnerClass<CSceneRegistry> {
    public:
        // Friend declaration
        using ACInnerClass<CSceneRegistry>::ACInnerClass;

#ifdef _EDITOR
        /** @brief Is it currently in editor mode? */
        bool IsEditorMode() { return m_owner->m_isEditorMode; }

        /** @brief Drawing colliders? */
        bool IsColliderDrawing() { return m_owner->m_isColliderDrawing; }
#endif
    };

    /** @brief Get feature for thread-safe */
    inline static CThreadSafeFeature& GetAny() {
        static CThreadSafeFeature instance(GetProtected().Get());
        return instance;
    }

protected:
    /**
       @brief Constructor
    */
    CSceneRegistry();

private:
    /**
       @brief Processing when scene loading is finished
       @param scene New scene
    */
    void OnLoadFinish(CScene* scene);

    /**
       @brief Execute initialization process for the next scene
       @return Return false if render path asset does not exist
    */
    bool NextSceneInitialization();

#ifdef _EDITOR
    /**
       @brief Hierarchy process
       @return Returns false if the hierarchy process is finished
    */
    bool HierarchyProcess();

    /**
       @brief Fix hierarchy name to avoid duplicates
       @param prevName Name before change
       @param name Name of game object to display in hierarchy
       @return fixed name
    */
    CStringWithIntKey FixHierarchyName(const CStringWithIntKey* prevName, std::string name);

    /**
       @brief Delete game object from hierarchy
       @param index Index of game object to be deleted
    */
    void DeleteFromHierarchy(size_t index);

    /**
       @brief Display popup to save scene
    */
    void DisplaySaveScenePopup();

    /**
       @brief Display popup to load scene
       @param isBackup Select backup file?
    */
    void DisplayLoadScenePopup(bool isBackup);

    /**
       @brief Display window for seeting
    */
    void DisplaySettingWindow();

    /**
       @brief Display object details
    */
    void DisplayObjectDetails();
#endif // _EDITOR

private:
    /** @brief Current scene */
    CSharedPtr<CScene> m_currentScene;
    /** @brief Next scene */
    CSharedPtr<CScene> m_nextScene;
    /** @brief Future object to load a next scene */
    Utl::Async::CMainThreadFuture<SceneLoadResult> m_sceneLoadFuture;
    
    /** @brief Game object data to display in hierarchy */
    std::vector<CObjectHierarchyData> m_hierarchyData;
    /** @brief Name of render pass asset to be used */
    std::string m_renderPassAssetName;

#ifdef _EDITOR
    /** @brief Is it currently in editor mode? */
    std::atomic<bool> m_isEditorMode;
    /** @brief Name of this scene */
    std::string m_name;
    /** @brief Name of scene to be load */
    std::string m_inputName;
    /** @brief Drawing colliders? */
    std::atomic<bool> m_isColliderDrawing;
    /** @brief Display fps? */
    bool m_isDisplayFps;

    /** @brief Name of game object to display in hierarchy already used */
    std::unordered_map<std::string, CPriorityIntKeyGenerater> m_hierarchyNameUsed;
    /** @brief Index of current selected object */
    size_t m_currentSelectedObjectIndex;
    /** @brief Is the settings window open now? */
    bool m_isOpenSettingWindow;
    /** @brief Is an object currently selected? */
    bool m_isObjectSelected;
    /** @brief Waiting for the message pop-up window? */
    bool m_isWaitForMessagePopup;
    /** @brief Variable to temporarily store the file name */
    std::vector<std::string> m_temporaryFilenames;
    /** @brief Popup name to open in next frame */
    const char* m_nextOpenPopupName;

    /** @brief Camera object for editor */
    CUniquePtrWeakable<CGameObject> m_editorCamera;
    /** @brief Camera component for editor */
    CWeakPtr<CCameraComponent> m_editorCameraComponent;
    /** @brief Gpso for drawing coordinate axis */
    CGraphicsPipelineState m_coordAxisGpso;
    /** @brief Coordinate axis object */
    CUniquePtrWeakable<CGraphicsObjectAsset> m_coordAxisObject;
#endif // _EDITOR
};

#endif // !__SCENE_REGISTRY_H__
