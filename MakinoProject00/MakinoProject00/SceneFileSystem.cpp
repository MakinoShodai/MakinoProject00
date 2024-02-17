#include "SceneFileSystem.h"
#include "Collider3D.h"
#include "UtilityForString.h"
#include "SwapChain.h"
#include "CommandManager.h"
#include "Shape.h"
#include "PointLight.h"

// Extension of scene file
const std::string SCENE_FILE_EXTENSION = ".scene";

// Common delimiter
const char COMMON_DELIMITER = '"';

// Delimiter after rendering pass name
const std::string DELIMITER_AFTER_RENDERPASS = "renderpass";
// Delimiter after hierarchy name
const std::string DELIMITER_AFTER_HIERARCHYNAME = "hierarchyname";
// Delimiter after prefab name
const std::string DELIMITER_AFTER_PREFABNAME = "prefabname";
// Delimiter after transform
const std::string DELIMITER_AFTER_TRANSFORM = "tr";

// Write delimiter
void WriteDelimiter(std::string* outputText, const std::string& delimiter) {
    *outputText += COMMON_DELIMITER;
    *outputText += delimiter;
    *outputText += COMMON_DELIMITER;
}

// Get to delimiter from string
bool GetToDelimiter(std::string* text, std::string* ret, std::string* delimiter) {
    size_t pos = text->find(COMMON_DELIMITER);
    if (pos != std::string::npos) {

        *ret = text->substr(0, pos);
        text->erase(0, pos + 1);

        pos = text->find(COMMON_DELIMITER);
        if (pos != std::string::npos) {
            *delimiter = text->substr(0, pos);
            text->erase(0, pos + 1);
            return true;
        }
    }

    return false;
}

// Get Vector3f from string
bool GetVector3fFromString(std::string* text, Vector3f* vec) {
    std::string delimiter;
    std::string tmp;

    for (uint8_t i = 0; i < 3; ++i) {
        if (false == (GetToDelimiter(text, &tmp, &delimiter) && delimiter == DELIMITER_AFTER_TRANSFORM)) {
            return false;
        }

        (*vec)[i] = std::stof(tmp);
    }
    return true;
}

// Constructor
CObjectHierarchyData::CObjectHierarchyData(CStringWithIntKey hierarchyName)
    : m_hierarchyName(std::move(hierarchyName))
    , m_prefabName()
    , m_objPtr(nullptr)
{
}

// Copy constructor
CObjectHierarchyData::CObjectHierarchyData(CStringWithIntKey hierarchyName, const CObjectHierarchyData& copy)
    : m_hierarchyName(std::move(hierarchyName))
    , m_prefabName(copy.m_prefabName)
    , m_objPtr(nullptr)
    , m_eularTransform(copy.m_eularTransform) {
    if (nullptr != copy.m_objPtr) {
        m_objPtr.Reset(ACRegistrarForGameObject::CreateGameObject(
            copy.m_prefabName, copy.m_objPtr->GetScene().Get(), 
            copy.m_objPtr->GetTransform()));
        m_objPtr->Prefab();
    }
}

// Destroy prefab
void CObjectHierarchyData::DestroyPrefab() {
    if (m_objPtr) {
        OnDestroyPrefab();
        m_objPtr.Reset();
    }
}

// Set prefab
void CObjectHierarchyData::SetPrefab(CScene* scene, const std::string& prefabName) {
    if (m_prefabName != prefabName) {
        if (scene != nullptr) {
            if (m_objPtr) {
                // Secure commands
                CCommandManager::GetMain().SecureCommands();
                OnDestroyPrefab();
            }

            m_objPtr.Reset(ACRegistrarForGameObject::CreateGameObject(prefabName, scene, Transformf(m_eularTransform.pos, m_eularTransform.scale, m_eularTransform.angle * Utl::DEG_2_RAD)));
            m_objPtr->Prefab();
        }
        m_prefabName = prefabName;
    }
}

// Set collider drawing on/off
void CObjectHierarchyData::SetDrawingCollider(bool isDrawing) {
    if (m_objPtr) {
        std::vector<CWeakPtr<CDebugColliderShape>> shapes = m_objPtr->GetComponents<CDebugColliderShape>();
        for (auto& it : shapes) {
            it->SetIsActive(isDrawing);
        }
    }
}

// Rebuild the object with the current prefab name
void CObjectHierarchyData::RebuildCurrentPrefab(CScene* scene) {
    if (!m_prefabName.empty() && scene != nullptr && !m_objPtr) {
        CGameObject* obj = ACRegistrarForGameObject::CreateGameObject(m_prefabName, scene,
            Transformf(m_eularTransform.pos, m_eularTransform.scale, m_eularTransform.angle * Utl::DEG_2_RAD));
        if (obj != nullptr) {
            m_objPtr.Reset(obj);
            m_objPtr->Prefab();
            std::vector<CWeakPtr<ACCollider3D>> colliders = m_objPtr->GetComponents<ACCollider3D>();
            for (auto& it : colliders) {
                it->ScalingRotationUpdate();
            }
        }
        else {
            m_prefabName.clear();
        }
    }
}

// Set position
void CObjectHierarchyData::SetPos(const Vector3f& pos) {
    m_eularTransform.pos = pos;
    if (m_objPtr) {
        m_objPtr->SetPos(pos);
    }
}

// Set scale
void CObjectHierarchyData::SetScale(const Vector3f& scale) {
    m_eularTransform.scale = scale;
    if (m_objPtr) {
        m_objPtr->SetScale(scale);

        std::vector<CWeakPtr<ACCollider3D>> colliders = m_objPtr->GetComponents<ACCollider3D>();
        for (auto& it : colliders) {
            it->ScalingRotationUpdate();
        }
        m_objPtr->TransferTransformObserve();
    }
}

// Set eular angle
void CObjectHierarchyData::SetEularAngle(const Vector3f& angle) {
    m_eularTransform.angle = angle;
    if (m_objPtr) {
        m_objPtr->SetRotation(Quaternionf(angle * Utl::DEG_2_RAD));

        std::vector<CWeakPtr<ACCollider3D>> colliders = m_objPtr->GetComponents<ACCollider3D>();
        for (auto& it : colliders) {
            it->ScalingRotationUpdate();
        }
    }
}

// Processing when Prefab is destroyed
void CObjectHierarchyData::OnDestroyPrefab() {
    if (m_objPtr) {
        std::vector<CWeakPtr<ACGraphicsComponent>> graphicsComponents = m_objPtr->GetComponents<ACGraphicsComponent>();
        for (auto& it : graphicsComponents) {
            it->OnDestroy();
        }
        std::vector<CWeakPtr<CPointLightComponent>> pointLights = m_objPtr->GetComponents<CPointLightComponent>();
        for (auto& it : pointLights) {
            it->OnDisable();
        }
    }
}

// Load scene data from a scene file
bool SceneFileSystem::LoadSceneData(const std::string& sceneName, std::string* retRenderPassName, std::vector<CObjectHierarchyData>* objects) {
    std::ifstream file(SCENE_FILE_DIR + sceneName + SCENE_FILE_EXTENSION, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    // Copy file data to a std::string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string text = buffer.str();

    // Close file
    file.close();

#ifdef _EDITOR
    // Secure commands
    CCommandManager::GetMain().SecureCommands();

    // Clear objects
    for (auto& it : *objects) {
        it.DestroyPrefab();
    }
#endif // _EDITOR
    objects->clear();

    std::string retDelimiter;

    // Get rendering pass name
    if (false == (GetToDelimiter(&text, retRenderPassName, &retDelimiter) && retDelimiter == DELIMITER_AFTER_RENDERPASS)) {
        return false;
    }

    std::string tmp;
    while (GetToDelimiter(&text, &tmp, &retDelimiter) && retDelimiter == DELIMITER_AFTER_HIERARCHYNAME) {
        // Set hierarchy name
        objects->emplace_back(tmp);

        // Read prefab name
        CObjectHierarchyData& object = objects->back();
        if (false == (GetToDelimiter(&text, &tmp, &retDelimiter) && retDelimiter == DELIMITER_AFTER_PREFABNAME)) {
            break;
        }
        object.SetPrefabName(tmp);
        
        // Read position
        Vector3f vec;
        if (GetVector3fFromString(&text, &vec) == false) {
            break;
        }
        object.SetPos(vec);
        // Read scale
        if (GetVector3fFromString(&text, &vec) == false) {
            break;
        }
        object.SetScale(vec);
        // Read angle
        if (GetVector3fFromString(&text, &vec) == false) {
            break;
        }
        object.SetEularAngle(vec);
    }

    return true;
}

// Save scene data to a file
bool SceneFileSystem::SaveSceneData(const std::string& sceneName, const std::string& renderPassName, 
    const std::vector<CObjectHierarchyData>& objects) {
    // Write rendering pass name
    std::string outputText = renderPassName;
    WriteDelimiter(&outputText, DELIMITER_AFTER_RENDERPASS);

    // Write objects
    for (const auto& it : objects) {
        // Write hierarchy name
        outputText += it.GetHierarchyName().GetWithIntKey();
        WriteDelimiter(&outputText, DELIMITER_AFTER_HIERARCHYNAME);

        // Write prefab name
        outputText += it.GetPrefabName();
        WriteDelimiter(&outputText, DELIMITER_AFTER_PREFABNAME);

        // Write transform
        {
            // Write position x
            outputText += std::to_string(it.GetPos().x());
            WriteDelimiter(&outputText, DELIMITER_AFTER_TRANSFORM);
            // Write position y
            outputText += std::to_string(it.GetPos().y());
            WriteDelimiter(&outputText, DELIMITER_AFTER_TRANSFORM);
            // Write position z
            outputText += std::to_string(it.GetPos().z());
            WriteDelimiter(&outputText, DELIMITER_AFTER_TRANSFORM);


            // Write scale x
            outputText += std::to_string(it.GetScale().x());
            WriteDelimiter(&outputText, DELIMITER_AFTER_TRANSFORM);
            // Write scale y
            outputText += std::to_string(it.GetScale().y());
            WriteDelimiter(&outputText, DELIMITER_AFTER_TRANSFORM);
            // Write scale z
            outputText += std::to_string(it.GetScale().z());
            WriteDelimiter(&outputText, DELIMITER_AFTER_TRANSFORM);


            // Write angle x
            outputText += std::to_string(it.GetEularAngle().x());
            WriteDelimiter(&outputText, DELIMITER_AFTER_TRANSFORM);
            // Write angle y
            outputText += std::to_string(it.GetEularAngle().y());
            WriteDelimiter(&outputText, DELIMITER_AFTER_TRANSFORM);
            // Write angle z
            outputText += std::to_string(it.GetEularAngle().z());
            WriteDelimiter(&outputText, DELIMITER_AFTER_TRANSFORM);
        }
    }
    
    std::string filePath = SCENE_FILE_DIR + sceneName + SCENE_FILE_EXTENSION;

    // Get directory path from the file path
    std::filesystem::path dirpath = std::filesystem::path(filePath).parent_path();
    // If directory doesn't exists, create it
    if (!std::filesystem::exists(dirpath)) {
        if (!std::filesystem::create_directories(dirpath)) {
            return false;
        }
    }

    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(outputText.c_str(), static_cast<std::streamsize>(outputText.length()));
    file.close();
    return true;
}

// Callback function to monitor prohibited characters
int SceneFileSystem::ProhibitedCharacterCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter) {
        // Prohibited character
        if (data->EventChar == COMMON_DELIMITER)
            return 1;
    }
    return 0;
}
