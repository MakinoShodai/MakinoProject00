/**
 * @file   SceneFileSystem.h
 * @brief  This file handles file system for scene.
 * 
 * @author Shodai Makino
 * @date   2024/1/25
 */

#ifndef __SCENE_FILE_SYSTEM_H__
#define __SCENE_FILE_SYSTEM_H__

#include "GameObject.h"
#include "IntKeyGenerater.h"

 // Directory of scene file
const std::string SCENE_FILE_DIR = "SceneData/";

/** @brief Game object data to display in hierarchy */
class CObjectHierarchyData {
public:
    /**
       @brief Constructor
       @param hierarchyName Name to display in hierarchy
    */
    CObjectHierarchyData(CStringWithIntKey hierarchyName);

    /**
       @brief Constructor
       @param hierarchyName Name to display in hierarchy
       @param copy Copy source
    */
    CObjectHierarchyData(CStringWithIntKey hierarchyName, const CObjectHierarchyData& copy);

    /** @brief Move constructor */
    CObjectHierarchyData(CObjectHierarchyData&& other) = default;
    /** @brief Move operator */
    CObjectHierarchyData& operator=(CObjectHierarchyData&& other) = default;

    /**
       @brief Destroy prefab
    */
    void DestroyPrefab();

    /**
       @brief Set prefab
       @param scene Owner scene
       @param prefabName Name of prefab to be used
    */
    void SetPrefab(CScene* scene, const std::string& prefabName);

    /**
       @brief Set collider drawing on/off
       @param isDrawing Drawing colliders?
    */
    void SetDrawingCollider(bool isDrawing);

    /**
       @brief Rebuild the object with the current prefab name
    */
    void RebuildCurrentPrefab(CScene* scene);

    /** @brief Set name to display in hierarchy */
    void SetHierarchyName(CStringWithIntKey name) { m_hierarchyName = std::move(name); }
    /** @brief Get name to display in hierarchy */
    const CStringWithIntKey& GetHierarchyName() const { return m_hierarchyName; }

    /** @brief Set prefab name (No prefab is created) */
    void SetPrefabName(const std::string& prefabName) { m_prefabName = prefabName; }
    /** @brief Get name prefab to be used */
    const std::string& GetPrefabName() const { return m_prefabName; }
    
    /** @brief Set position */
    void SetPos(const Vector3f& pos);
    /** @brief Get position */
    const Vector3f& GetPos() const { return m_eularTransform.pos; }

    /** @brief Set scale */
    void SetScale(const Vector3f& scale);
    /** @brief Get scale */
    const Vector3f& GetScale() const { return m_eularTransform.scale; }

    /** @brief Set eular angle */
    void SetEularAngle(const Vector3f& angle);
    /** @brief Get eular angle */
    const Vector3f& GetEularAngle() const { return m_eularTransform.angle; }

private:
    /**
       @brief Processing when Prefab is destroyed
    */
    void OnDestroyPrefab();

private:
    /** @brief Name to display in hierarchy */
    CStringWithIntKey m_hierarchyName;
    /** @brief Name of prefab to be used */
    std::string m_prefabName;
    /** @brief Unique pointer weakable to game object */
    CUniquePtrWeakable<CGameObject> m_objPtr;
    /** @brief Transform that has Eular angle */
    EularTransformf m_eularTransform;
};

namespace SceneFileSystem {
    /**
       @brief Load scene data from a scene file
       @param sceneName Name of scene to be loaded
       @param renderPassName Return variable for name of rendering pass asset that scene has
       @param objects Game object data that scene has
    */
    bool LoadSceneData(const std::string& sceneName, std::string* retRenderPassName, std::vector<CObjectHierarchyData>* objects);

    /**
       @brief Save scene data to a file
       @param sceneName Name of scene to be saved
       @param renderPassName Name of rendering pass asset that scene has
       @param objects Game object data that scene has
    */
    bool SaveSceneData(const std::string& sceneName, const std::string& renderPassName, const std::vector<CObjectHierarchyData>& objects);

    /** @brief Callback function to monitor prohibited characters */
    int ProhibitedCharacterCallback(ImGuiInputTextCallbackData* data);

} // namespace SceneFileSystem

#endif // !__SCENE_FILE_SYSTEM_H__
