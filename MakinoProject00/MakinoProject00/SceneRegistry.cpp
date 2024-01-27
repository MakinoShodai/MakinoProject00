#include "SceneRegistry.h"
#include "CommandManager.h"
#include "DynamicCbRegistry.h"
#include "StaticResourceRegistry.h"
#include "DescriptorHeapPool.h"
#include "ImguiHelper.h"
#include "DebugCamera.h"
#include "StandardRenderPass.h"
#include "ApplicationClock.h"
#include "UtilityForFile.h"
#include "UtilityForString.h"

const Colorf SCREEN_CLEAR_COLOR = Colorf(0.0f, 0.0f, 0.0f, 1.0f);

const char* POPUPNAME_SAVESCENE = "Save Scene";
const char* POPUPNAME_LOADSCENE = "Load Scene";
const char* POPUPNAME_LOADSCENE_BACKUP = "Load Scene Backup";

const std::string BACKUP_SCENEFILE_DIRECTORY = "working/";
const std::string BACKUP_SCENEFILE_FINALSTRING = "_backup";

// Flag for input text
ImGuiInputTextFlags INPUT_TEXT_FLAG = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CallbackCharFilter;
// Flag for popup window
ImGuiWindowFlags POPUP_WINDOW_FLAG = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

#ifdef _EDITOR
// Default name of game object to display in inspector
const std::string DEFAULT_OBJECT_INSPECTOR_NAME = "Game object";
#endif // _EDITOR

// Initialize
void CSceneRegistry::Initialize() {
#ifdef _EDITOR
    // Set standard rendering pass
    m_renderPassAssetName = STANDARD_RENDER_PASS_NAME;

    // Initialize scene and wait it
    NextSceneInitialization();

    // Replace the current scene with the next scene
    m_currentScene = m_nextScene;
    m_nextScene.Reset();

    OnLoadFinish(m_currentScene.Get());

    CImguiHelper::GetAny().AddWindowFunction([this]() -> bool {
        return this->InspectorProcess();
        });
#else
    // Load scene
    std::string sceneName = "DefaultScene";
    SetNextScene(sceneName);

    // Get result
    SceneLoadResult result = m_sceneLoadFuture.Get();
    if (result != SceneLoadResult::Success) {
        if (result == SceneLoadResult::FailedSceneFile) {
            throw Utl::Error::CFatalError(L"Scene file not found\nScene Name : " + Utl::Str::string2WString(sceneName));
        }
        else if (result == SceneLoadResult::FailedRenderPassAsset) {
            throw Utl::Error::CFatalError(L"The render path asset set in the scene file doesn't exist\n Render Pass Asset Name : " + Utl::Str::string2WString(m_renderPassAssetName));
        }
    }

    // Replace the current scene with the next scene
    m_currentScene = m_nextScene;
    m_nextScene.Reset();

#endif // _EDITOR
}

// Processing per frame
void CSceneRegistry::ProcessPerFrame() {
    // If current scene doesn't exists
    if (!m_currentScene) { 
#ifdef _EDITOR
        // Get current back buffer and clear it
        ACRenderTarget* rtv = CSwapChain::GetAny().GetBackBuffer();
        rtv->Clear(SCREEN_CLEAR_COLOR);
        rtv->Apply(nullptr);

        // Draw imgui
        CImguiHelper::GetMain().Draw();

        // Swap and draw the backbuffer
        CSwapChain::GetMain().DrawAndSwap();
#endif
        return; 
    }

    // If a next scene is being loaded, wait for it to finish
    if (m_sceneLoadFuture.Valid()) {
        m_sceneLoadFuture.Get();

        // Secure commands
        CCommandManager::GetMain().SecureCommands();

        // Have the current scene destroyed in a new thread
        CThreadPool::GetAny().EnqueueTask([](CSharedPtr<CScene> scene) {
            scene->OnDestroy();
            scene.Reset();
        }, m_currentScene);
        
        // Replace the current scene with the next scene
        m_currentScene = m_nextScene;
        m_nextScene.Reset();

        // Finish loading
        OnLoadFinish(m_currentScene.Get());
    }

    // Updating process
#ifdef _EDITOR
    if (false == m_isEditorMode) {
        m_currentScene->Update();
    }
    else {
        m_editorCamera->Update();
        m_editorCameraComponent->ApplyRotation();
    }
#else
    m_currentScene->Update();
#endif // _EDITOR

    // Drawing preparation processing
    CDescriptorHeapPool::GetMain().ReadyToDraw();

    try {
        // Drawing process
        m_currentScene->Draw();
    }
    catch (const Utl::Error::CStopDrawingSceneError& e) {
        OutputDebugString(e.WhatW().c_str());

        // Secure commands
        CCommandManager::GetMain().SecureCommands();

        // Get current back buffer and clear it
        ACRenderTarget* rtv = CSwapChain::GetAny().GetBackBuffer();
        rtv->Clear(SCREEN_CLEAR_COLOR);
        rtv->Apply(nullptr);
    }

    // Draw imgui
    CImguiHelper::GetMain().Draw();

    // Processing at end of a frame
    m_currentScene->EndFrameProcess();

    // Refresh CB allocater and static resource allocater
    CDynamicCbRegistry::GetMain().FrameRefresh();
    CStaticResourceRegistry::GetMain().FrameRefresh();

    // Swap and draw the backbuffer
    CSwapChain::GetMain().DrawAndSwap();
}

// Set next transion scene and end of current scene
void CSceneRegistry::SetNextScene(const std::string& sceneName) {
    // Cancel if scene is currently loading
    if (m_sceneLoadFuture.Valid()) { return; }

    m_sceneLoadFuture = CThreadPool::GetAny().EnqueueTask([this, sceneName]() {
        // Load scene file
        if (SceneFileSystem::LoadSceneData(sceneName, &this->m_renderPassAssetName, &this->m_inspectorData)) {
            // Initialize scene
            if (this->NextSceneInitialization()) {
                return SceneLoadResult::Success;
            }
            // Render path assets could not be found
            else {
                return SceneLoadResult::FailedRenderPassAsset;
            }
        }
        // Failed loading
        else {
            return SceneLoadResult::FailedSceneFile;
        }
        return SceneLoadResult::FailedSceneFile;
    });
}

// Constructor
CSceneRegistry::CSceneRegistry()
    : ACMainThreadSingleton(50)
#ifdef _EDITOR
    , m_isEditorMode(true)
    , m_renderPassAssetName()
    , m_name("Scene")
    , m_inputName()
    , m_inspectorData()
    , m_currentSelectedObjectIndex(0)
    , m_isObjectSelected(false)
    , m_isWaitForMessagePopup(false)
    , m_isOpenSettingWindow(false)
    , m_nextOpenPopupName(nullptr)
    , m_isColliderDrawing(true)
#endif // _EDITOR
{}

// Processing when scene loading is finished
void CSceneRegistry::OnLoadFinish(CScene* scene) {
#ifdef _EDITOR
    if (m_isEditorMode) {
        // Create editor camera
        m_editorCamera.Reset(new CGameObject(scene, Transformf(Vector3f(0.0f, 0.0f, -10.0f))));
        m_editorCameraComponent = m_editorCamera->AddComponent<CCameraComponent>(L"Inspector camera");
        m_editorCamera->AddComponent<CFreeCameraControl>();
        m_editorCamera->Start();

        CStaticResourceRegistry::GetMain().SceneStart(scene);
    }
#endif // _EDITOR
}

// Execute initialization process for the next scene
bool CSceneRegistry::NextSceneInitialization() {
    ACRenderPassAsset* renderPassAsset = ACRegistrarForRenderPassAsset::CreateRenderPassAsset(m_renderPassAssetName);
    if (renderPassAsset == nullptr) {
        return false;
    }

    m_nextScene = CSharedPtr<CScene>::Make(renderPassAsset);
    m_nextScene->Start();

    // Create all objects that exist in the inspector
    for (auto& it : m_inspectorData) {
        m_nextScene->CreateGameObject(it.GetPrefabName(), Transformf(it.GetPos(), it.GetScale(), it.GetEularAngle() * Utl::RAD_2_DEG));
    }

    return true;
}

#ifdef _EDITOR
// Inspector process
bool CSceneRegistry::InspectorProcess() {
    // Window start
    ImGui::Begin("Inspector");

    // Open if pop-up reservation to be opened
    if (m_nextOpenPopupName != nullptr) {
        ImGui::OpenPopup(m_nextOpenPopupName);
        m_nextOpenPopupName = nullptr;
    }

    // Play game
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("Play")) {
        // Sequre commands
        CCommandManager::GetMain().SecureCommands();
        // Destroy prefabs
        for (auto& it : m_inspectorData) {
            it.DestroyPrefab();
        }
        // Destroy editor camera
        m_editorCamera->OnDestroy();
        m_editorCamera.Reset();
        m_editorCameraComponent = nullptr;
        
        // Finish editor mode
        m_isEditorMode = false;

        // Initialize next scene
        NextSceneInitialization();

        // Reset current scene
        m_currentScene.Reset();

        // Replace the current scene with the next scene
        m_currentScene = m_nextScene;
        m_nextScene.Reset();
        
        OnLoadFinish(m_currentScene.Get());

        // Window end
        ImGui::PopStyleColor();
        ImGui::End();
        return false;
    }
    ImGui::PopStyleColor();
    // Save scene
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.0f, 0.3f, 1.0f));
    if (ImGui::Button("Save")) {
        ImGui::OpenPopup(POPUPNAME_SAVESCENE);
        m_isWaitForMessagePopup = false;
    }
    ImGui::PopStyleColor();

    // Load scene
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    if (ImGui::Button("Load")) {
        ImGui::OpenPopup("Select Load Type");
    }
    ImGui::PopStyleColor();

    // Settings
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("Settings")) {
        m_isOpenSettingWindow = true;
    }
    ImGui::PopStyleColor();

    // Display popup to save scene
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.3f, 0.0f, 0.3f, 1.0f));
    DisplaySaveScenePopup();
    ImGui::PopStyleColor();

    // Select load type
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    if (ImGui::BeginPopupModal("Select Load Type")) {
        ImGui::Text("Please select the type of load!");
        // Load
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.3f, 1.0f));
        if (ImGui::Button("Load")) {
            ImGui::CloseCurrentPopup();
            m_nextOpenPopupName = POPUPNAME_LOADSCENE;
            m_inputName.clear();
            Utl::File::GetMatchExtensionInDirectory(&m_temporaryFilenames, { ".scene" }, SCENE_FILE_DIR, true);
        }
        ImGui::PopStyleColor();

        // Load for backup
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.3f, 0.0f, 1.0f));
        if (ImGui::Button("Load For Backup")) {
            ImGui::CloseCurrentPopup();
            m_nextOpenPopupName = POPUPNAME_LOADSCENE_BACKUP;
            m_inputName.clear();
            Utl::File::GetMatchExtensionInDirectory(&m_temporaryFilenames, { ".scene" }, SCENE_FILE_DIR + BACKUP_SCENEFILE_DIRECTORY, true);
        }
        ImGui::PopStyleColor();

        // Cancel
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleColor();

    // Display popup to load scene
    DisplayLoadScenePopup(false);
    DisplayLoadScenePopup(true);

    // Display setting window
    DisplaySettingWindow();

    // Scene name
    char sceneName[128];
    strncpy_s(sceneName, m_name.c_str(), sizeof(sceneName));
    sceneName[sizeof(sceneName) - 1] = '\0';
    ImGui::Text("Scene Name");
    ImGui::SameLine();
    ImGui::InputText("##Scene Name", sceneName, IM_ARRAYSIZE(sceneName), INPUT_TEXT_FLAG | ImGuiInputTextFlags_CharsNoBlank, SceneFileSystem::ProhibitedCharacterCallback);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        std::string afterName = sceneName;
        if (!afterName.empty()) {
            m_name = sceneName;
        }
    }

    // Render Pass Asset kind
    ImGui::Text("Render Pass Asset");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##Render Pass Asset", m_renderPassAssetName.c_str())) {
        // Display prefab class name
        const auto& map = ACRegistrarForRenderPassAsset::GetClassMap();
        for (const auto& it : map) {
            bool isSelected = (it.first == m_renderPassAssetName);
            // If changed
            if (ImGui::Selectable(it.first.c_str(), isSelected)) {
                if (!isSelected) {
                    // Change rendering pass
                    CCommandManager::GetMain().SecureCommands();
                    m_currentScene->SetRenderPass(CUniquePtr<ACRenderPassAsset>(it.second()));
                    // Set asset name
                    m_renderPassAssetName = it.first;
                }
            }
        }
        ImGui::EndCombo();
    }

    // Add object
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    if (ImGui::Button("Add")) {
        m_inspectorData.push_back(CObjectInspectorData(FixInspectorName(nullptr, DEFAULT_OBJECT_INSPECTOR_NAME)));
    }
    ImGui::PopStyleColor();
    
    // Delete object
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
    ImGui::SameLine();
    if (ImGui::Button("Delete") && m_isObjectSelected) {
        DeleteFromInspector(m_currentSelectedObjectIndex);
    }
    ImGui::PopStyleColor();
    
    // Duplicate object
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.0f, 1.0f));
    if (ImGui::Button("Duplicate") && m_isObjectSelected) {
        m_inspectorData.push_back(CObjectInspectorData(
            FixInspectorName(nullptr, m_inspectorData[m_currentSelectedObjectIndex].GetInspectorName().GetWithIntKey()), 
            m_inspectorData[m_currentSelectedObjectIndex]));
    }
    ImGui::PopStyleColor();

    // Up object
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::SameLine();
    if (ImGui::Button("^") && m_isObjectSelected && m_currentSelectedObjectIndex > 0) {
        std::swap(m_inspectorData[m_currentSelectedObjectIndex], m_inspectorData[m_currentSelectedObjectIndex - 1]);
        m_currentSelectedObjectIndex--;
    }

    // Down object
    ImGui::SameLine();
    if (ImGui::Button("v") && m_isObjectSelected && m_currentSelectedObjectIndex < m_inspectorData.size() - 1) {
        std::swap(m_inspectorData[m_currentSelectedObjectIndex], m_inspectorData[m_currentSelectedObjectIndex + 1]);
        m_currentSelectedObjectIndex++;
    }
    ImGui::PopStyleColor(2);

    // Display objects to inspector
    size_t inspectorSize = m_inspectorData.size();
    for (size_t i = 0; i < inspectorSize; ++i) {
        if (ImGui::Selectable(m_inspectorData[i].GetInspectorName().GetWithIntKey().c_str(), m_currentSelectedObjectIndex == i && m_isObjectSelected)) {
            m_currentSelectedObjectIndex = i;
            m_isObjectSelected = true;
        }
    }

    // Window end
    ImGui::End();

    // If an object is currently selected
    DisplayObjectDetails();

    return true;
}

// Fix inspector name to avoid duplicates
CStringWithIntKey CSceneRegistry::FixInspectorName(const CStringWithIntKey* prevName, std::string name) {
    // Remove trailing spaces
    size_t pos = name.find_last_not_of(' ');
    if (std::string::npos != pos) {
        name = name.substr(0, pos + 1);
    }
    CStringWithIntKey nameWithKey(name);

    if (prevName != nullptr) {
        if (prevName->GetWithIntKey() == name) {
            return *prevName;
        }
        m_inspectorNameUsed[prevName->GetUnKeyedString()].ReleaseKey(prevName->GetIntKey());
    }

    IntKey key = nameWithKey.GetIntKey();
    if (key > 0) {
        key = m_inspectorNameUsed[nameWithKey.GetUnKeyedString()].GenerateNotDuplicateKey(key);
        nameWithKey.SetIntKey(key);
        return nameWithKey;
    }

    // Avoid name duplicates
    key = m_inspectorNameUsed[nameWithKey.GetUnKeyedString()].GenerateKey();
    nameWithKey.SetIntKey(key);

    return nameWithKey;
}

// Delete game object from inspector
void CSceneRegistry::DeleteFromInspector(size_t index) {
    // Erase previous name from map
    auto it = m_inspectorNameUsed.find(m_inspectorData[index].GetInspectorName().GetUnKeyedString());
    if (it != m_inspectorNameUsed.end()) {
        if (it->second.IsNotKeyAssignment()) {
            m_inspectorNameUsed.erase(it);
        }
        else {
            it->second.ReleaseKey(m_inspectorData[index].GetInspectorName().GetIntKey());
        }
    }

    // Sequre commands
    CCommandManager::GetMain().SecureCommands();

    // Erase
    m_inspectorData[index].DestroyPrefab();
    m_inspectorData.erase(m_inspectorData.begin() + index);
    if (index >= m_inspectorData.size()) {
        m_isObjectSelected = false;
        m_currentSelectedObjectIndex = 0;
    }
}

// Display popup to save scene
void CSceneRegistry::DisplaySaveScenePopup() {
    // Select save type for scene
    if (ImGui::BeginPopupModal(POPUPNAME_SAVESCENE, NULL, POPUP_WINDOW_FLAG)) {
        ImGui::Text("Please select the type of save!");
        // Save
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.3f, 1.0f));
        if (ImGui::Button("Save")) {
            if (SceneFileSystem::SaveSceneData(m_name, m_renderPassAssetName, m_inspectorData)) {
                ImGui::OpenPopup("Save Success");
            }
            else {
                ImGui::OpenPopup("Save Failed");
            }
            m_isWaitForMessagePopup = true;
        }
        ImGui::PopStyleColor();

        // Save for backup
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.3f, 0.0f, 1.0f));
        if (ImGui::Button("Save For Backup")) {
            int maxNumber = -1;

            std::vector<std::string> backupFiles;
            Utl::File::GetMatchExtensionInDirectory(&backupFiles, { ".scene" }, SCENE_FILE_DIR + BACKUP_SCENEFILE_DIRECTORY, true);
            for (const auto& it : backupFiles) {
                size_t pos = it.rfind(BACKUP_SCENEFILE_FINALSTRING);
                if (pos != std::string::npos) {
                    std::string backupSceneName = it.substr(0, pos);
                    if (backupSceneName == m_name) {
                        std::string numInString = it.substr(pos + BACKUP_SCENEFILE_FINALSTRING.size());
                        maxNumber = (std::max)(maxNumber, std::stoi(numInString));
                    }
                }
            }

            std::string saveFilename = BACKUP_SCENEFILE_DIRECTORY + m_name + BACKUP_SCENEFILE_FINALSTRING + std::to_string(maxNumber + 1);
            if (SceneFileSystem::SaveSceneData(saveFilename, m_renderPassAssetName, m_inspectorData)) {
                ImGui::OpenPopup("Save Success");
            }
            else {
                ImGui::OpenPopup("Save Failed");
            }
            m_isWaitForMessagePopup = true;
        }
        ImGui::PopStyleColor();

        // Cancel
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        // Save scene message window (Success)
        if (ImGui::BeginPopup("Save Success", POPUP_WINDOW_FLAG)) {
            ImGui::Text("Scene saved successfully!");
            // Close window
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        // Save scene message window (Failed)
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        if (ImGui::BeginPopup("Save Failed", POPUP_WINDOW_FLAG)) {
            ImGui::Text("Scene save failed...");
            // Close window
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleColor();

        // Wait for message popup window
        bool isMessagePopupOpen = ImGui::IsPopupOpen("Save Success") || ImGui::IsPopupOpen("Save Failed");
        if (!isMessagePopupOpen && m_isWaitForMessagePopup) {
            ImGui::CloseCurrentPopup();
            m_isWaitForMessagePopup = false;
        }

        ImGui::EndPopup();
    }
}

// Display popup for select to load scene
void CSceneRegistry::DisplayLoadScenePopup(bool isBackup) {
    // Input to load scene name
    const char* popupName = (isBackup) ? POPUPNAME_LOADSCENE_BACKUP : POPUPNAME_LOADSCENE;
    if (ImGui::BeginPopupModal(popupName, NULL, POPUP_WINDOW_FLAG)) {
        // Calculate width for the combo box
        float combo_width = 0.0f;
        for (const auto& it : m_temporaryFilenames) {
            ImVec2 text_size = ImGui::CalcTextSize(it.c_str());
            combo_width = (std::max)(combo_width, text_size.x);
        }

        // Ask for scene name select
        ImGui::Text("Select Load Scene");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(combo_width + ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.x * 2);
        if (ImGui::BeginCombo("##Load Scene Name", m_inputName.c_str())) {
            // Display prefab class name
            for (const auto& it : m_temporaryFilenames) {
                bool isSelected = (it == m_inputName);
                // If changed
                if (ImGui::Selectable(it.c_str(), isSelected)) {
                    m_inputName = it;
                }
            }
            ImGui::EndCombo();
        }

        // Confirmation of input
        if (ImGui::Button("OK")) {
            std::string loadName = (isBackup) ? BACKUP_SCENEFILE_DIRECTORY + m_inputName : m_inputName;
            if (SceneFileSystem::LoadSceneData(loadName, &m_renderPassAssetName, &m_inspectorData)) {
                // Secure commands
                CCommandManager::GetMain().SecureCommands();

                ImGui::CloseCurrentPopup();
                m_isObjectSelected = false;
                m_inspectorNameUsed.clear();

                // Extract scene name
                m_name = m_inputName;
                if (isBackup) {
                    size_t pos = m_inputName.rfind(BACKUP_SCENEFILE_FINALSTRING);
                    if (pos != std::string::npos) {
                        m_name = m_inputName.substr(0, pos);
                    }
                }

                // If render path asset does not exist, change to standard
                if (false == ACRegistrarForRenderPassAsset::GetClassMap().contains(m_renderPassAssetName)) {
                    m_renderPassAssetName = STANDARD_RENDER_PASS_NAME;
                }
                m_currentScene->SetRenderPass(ACRegistrarForRenderPassAsset::CreateRenderPassAsset(m_renderPassAssetName));

                // Rebuild game object
                for (auto& it : m_inspectorData) {
                    it.RebuildCurrentPrefab(m_currentScene.Get());
                    m_inspectorNameUsed[it.GetInspectorName().GetUnKeyedString()].RegisterUsedKey(it.GetInspectorName().GetIntKey());
                }
            }
            else {
                ImGui::OpenPopup("Failed Load");
            }
        }
        // Close window
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        // Popup for failed message
        if (ImGui::BeginPopup("Failed Load")) {
            ImGui::Text("Couldn't open file with matching scene name");
            if (ImGui::Button("OK")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleColor();

        ImGui::EndPopup();
    }
}

// Display window for seeting
void CSceneRegistry::DisplaySettingWindow() {
    if (m_isOpenSettingWindow) {
        ImGui::Begin("Settings");

        // Drawing collider
        bool isDrawing = m_isColliderDrawing;
        ImGui::Checkbox("Drawing Collider", &isDrawing);
        if (isDrawing != m_isColliderDrawing) {
            m_isColliderDrawing = isDrawing;
            for (auto& it : m_inspectorData) {
                it.SetDrawingCollider(m_isColliderDrawing);
            }
        }

        // Close
        if (ImGui::Button("Close")) {
            m_isOpenSettingWindow = false;
        }

        ImGui::End();
    }
}

// Display object details
void CSceneRegistry::DisplayObjectDetails() {
    if (m_isObjectSelected) {
        ImGui::Begin("Details");

        // Copy name
        char inspectorName[256];
        strncpy_s(inspectorName, m_inspectorData[m_currentSelectedObjectIndex].GetInspectorName().GetWithIntKey().c_str(), sizeof(inspectorName));
        inspectorName[sizeof(inspectorName) - 1] = '\0';

        // Inspector name
        ImGui::Text("Inspector Name");
        ImGui::SameLine();
        ImGui::InputText("##Inspector Name", inspectorName, IM_ARRAYSIZE(inspectorName), INPUT_TEXT_FLAG, SceneFileSystem::ProhibitedCharacterCallback);
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            std::string afterName = inspectorName;

            if (!afterName.empty()) {
                m_inspectorData[m_currentSelectedObjectIndex].SetInspectorName(
                    FixInspectorName(&m_inspectorData[m_currentSelectedObjectIndex].GetInspectorName(), inspectorName));
            }
        }

        // Prefab kind
        ImGui::Text("Prefab");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Prefab", m_inspectorData[m_currentSelectedObjectIndex].GetPrefabName().c_str())) {
            // Display prefab class name
            const auto& map = ACRegistrarForGameObject::GetClassMap();
            for (const auto& it : map) {
                bool isSelected = (it.first == m_inspectorData[m_currentSelectedObjectIndex].GetPrefabName());
                // If changed
                if (ImGui::Selectable(it.first.c_str(), isSelected)) {
                    m_inspectorData[m_currentSelectedObjectIndex].SetPrefab(m_currentScene.Get(), it.first);
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::TreeNode("Transform")) {
            // Position
            ImGui::Text("Position");
            ImGui::SameLine();
            Vector3f pos = m_inspectorData[m_currentSelectedObjectIndex].GetPos();
            if (ImGui::InputFloat3("##Position", pos.GetArrayPtr(), "%.2f")) {
                m_inspectorData[m_currentSelectedObjectIndex].SetPos(pos);
            }

            // Scale
            ImGui::Text("Scale");
            ImGui::SameLine();
            Vector3f scale = m_inspectorData[m_currentSelectedObjectIndex].GetScale();
            if (ImGui::InputFloat3("##Scale", scale.GetArrayPtr(), "%.2f")) {
                m_inspectorData[m_currentSelectedObjectIndex].SetScale(scale);
            }

            // Eular angle
            ImGui::Text("Angle");
            ImGui::SameLine();
            Vector3f eularAngle = m_inspectorData[m_currentSelectedObjectIndex].GetEularAngle();
            if (ImGui::InputFloat3("##Angle", eularAngle.GetArrayPtr(), "%.2f")) {
                m_inspectorData[m_currentSelectedObjectIndex].SetEularAngle(eularAngle);
            }

            ImGui::TreePop();
        }

        ImGui::End();
    }
}
#endif // _EDITOR
