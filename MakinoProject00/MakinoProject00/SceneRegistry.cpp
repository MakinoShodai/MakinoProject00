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
#include "ShaderRegistry.h"
#include "ModelRegistry.h"
#include "AssetNameDefine.h"
#include "DynamicCbWorldMat.h"
#include "StaticCbVPMat.h"
#include "DynamicCbColor.h"
#include "DynamicCbTexCoordParam.h"
#include "SkyDome.h"
#include "DebugCamera.h"

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
// Standard distance of coordinate axis object
const float AXIS_STANDARD_DISTANCE = 1000.0f;
// Default name of game object to display in hierarchy
const std::string DEFAULT_OBJECT_HIERARCHY_NAME = "Game object";
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
        return this->HierarchyProcess();
        });

    // Add needed object at startup
    m_hierarchyData.emplace_back(FixHierarchyName(nullptr, "Diretional light"));
    m_hierarchyData.back().SetPrefab(m_currentScene.Get(), PREFAB_NAME_DIR_LIGHT);
    m_hierarchyData.emplace_back(FixHierarchyName(nullptr, "Sky dome"));
    m_hierarchyData.back().SetPrefab(m_currentScene.Get(), PREFAB_NAME_SKYDOME);
    m_hierarchyData.emplace_back(FixHierarchyName(nullptr, "Debug camera"));
    m_hierarchyData.back().SetPrefab(m_currentScene.Get(), PREFAB_NAME_DEBUGCAMERA);
    m_hierarchyData.back().SetPos(Vector3f(0.0f, 1.0f, -10.0f));

    // Create gpso for drawing coordinate axis
    Gpso::GPSOSetting setting;
    setting.vs = CShaderRegistry::GetAny().GetVS(VertexShaderType::Standard3D);
    setting.ps = CShaderRegistry::GetAny().GetPS(PixelShaderType::StandardTex);
    setting.rtvFormats.push_back(std::make_pair(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Gpso::BlendStateSetting::Alpha));
    setting.rasterizerState.cullMode = D3D12_CULL_MODE_BACK;
    setting.depth.type = Gpso::DepthTestType::None;
    m_coordAxisGpso.Create(nullptr, L"Coordinate Allow GPSO", setting, {}, {});

    // Create graphics object asset for coordinate axis
    m_coordAxisObject = CUniquePtrWeakable<CGraphicsObjectAsset>::Make();
    const auto& model = CModelRegistry::GetAny().GetModel(ModelName::COORD_AXIS);
    m_coordAxisObject->SetMeshBuff(model.GetOpacityMesh(0)->meshbuffer.GetWeakPtr());
    m_coordAxisObject->AddSRV(CTexture(Utl::Dx::ShaderString(Utl::Dx::ShaderType::Pixel, "mainTex"), model.GetMaterial(0)->GetTex()));
    m_coordAxisGpso.AddGraphicsObjectAsset(m_coordAxisObject.GetWeakPtr());
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

        // Set current descriptor heap
        CDescriptorHeapPool::GetMain().SetCurrentDescriptorHeapToCommand();
    }

#ifdef _EDITOR
    CCameraComponent* camera = m_currentScene->GetCameraRegistry()->GetCameraPriority().Get();
    if (m_isEditorMode && m_isObjectSelected && camera) {
        m_coordAxisObject->SetIsActive(true);

        // Get needed constant buffer allocator classes
        CDynamicCbWorldMat* worldMatCb = CDynamicCbRegistry::GetAny().GetCbAllocator<CDynamicCbWorldMat>();
        CDynamicCbColor* colorCb = CDynamicCbRegistry::GetAny().GetCbAllocator<CDynamicCbColor>();
        CDynamicCbTexCoordParam* texCoordCb = CDynamicCbRegistry::GetAny().GetCbAllocator<CDynamicCbTexCoordParam>();
        CStaticCbVP* viewProjCb = CStaticResourceRegistry::GetAny().GetStaticResource<CStaticCbVP>();

        // Add constant buffer view
        CObjectHierarchyData& object = m_hierarchyData[m_currentSelectedObjectIndex];
        Vector3f scale = Vector3f::Ones() * ((camera->GetTransform().pos - object.GetPos()).Length() / AXIS_STANDARD_DISTANCE);
        m_coordAxisObject->AddCBV(Utl::Dx::ShaderString(Utl::Dx::ShaderType::Vertex, worldMatCb->GetName()), worldMatCb->AllocateDataNoComponent(object.GetPos(), scale, Utl::DEG_2_RAD * object.GetEularAngle()));
        m_coordAxisObject->AddCBV(Utl::Dx::ShaderString(Utl::Dx::ShaderType::Vertex, viewProjCb->GetName()), viewProjCb->GetAllocatedDataHandle());
        m_coordAxisObject->AddCBV(Utl::Dx::ShaderString(Utl::Dx::ShaderType::Vertex, texCoordCb->GetName()), texCoordCb->AllocateDataNoComponent());
        m_coordAxisObject->AddCBV(Utl::Dx::ShaderString(Utl::Dx::ShaderType::Pixel, colorCb->GetName()), colorCb->AllocateDataNoComponent(Colorf::Ones()));
    }
    else {
        m_coordAxisObject->SetIsActive(false);
    }
    m_coordAxisGpso.SetCommand();
    m_coordAxisGpso.EndFrameProcess();
#endif // _EDITOR

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
        if (SceneFileSystem::LoadSceneData(sceneName, &this->m_renderPassAssetName, &this->m_hierarchyData)) {
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
    , m_hierarchyData()
    , m_currentSelectedObjectIndex(0)
    , m_isObjectSelected(false)
    , m_isWaitForMessagePopup(false)
    , m_isOpenSettingWindow(false)
    , m_nextOpenPopupName(nullptr)
    , m_isColliderDrawing(true)
    , m_isDisplayFps(true)
#endif // _EDITOR
{}

// Processing when scene loading is finished
void CSceneRegistry::OnLoadFinish(CScene* scene) {
#ifdef _EDITOR
    if (m_isEditorMode) {
        // Create editor camera
        m_editorCamera.Reset(new CGameObject(scene, Transformf(Vector3f(0.0f, 1.0f, -10.0f))));
        m_editorCameraComponent = m_editorCamera->AddComponent<CCameraComponent>(L"Editor camera");
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

    // Create all objects that exist in the hierarchy
    for (auto& it : m_hierarchyData) {
        m_nextScene->CreateGameObject(it.GetPrefabName(), Transformf(it.GetPos(), it.GetScale(), it.GetEularAngle() * Utl::DEG_2_RAD));
    }

    return true;
}

#ifdef _EDITOR
// Hierarchy process
bool CSceneRegistry::HierarchyProcess() {
    // Window start
    ImGui::Begin("Hierarchy");

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
        for (auto& it : m_hierarchyData) {
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
        m_hierarchyData.push_back(CObjectHierarchyData(FixHierarchyName(nullptr, DEFAULT_OBJECT_HIERARCHY_NAME)));
    }
    ImGui::PopStyleColor();
    
    // Delete object
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
    ImGui::SameLine();
    if (ImGui::Button("Delete") && m_isObjectSelected) {
        DeleteFromHierarchy(m_currentSelectedObjectIndex);
    }
    ImGui::PopStyleColor();
    
    // Duplicate object
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.0f, 1.0f));
    if (ImGui::Button("Duplicate") && m_isObjectSelected) {
        m_hierarchyData.push_back(CObjectHierarchyData(
            FixHierarchyName(nullptr, m_hierarchyData[m_currentSelectedObjectIndex].GetHierarchyName().GetWithIntKey()), 
            m_hierarchyData[m_currentSelectedObjectIndex]));
    }
    ImGui::PopStyleColor();

    // Up object
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::SameLine();
    if (ImGui::Button("^") && m_isObjectSelected && m_currentSelectedObjectIndex > 0) {
        std::swap(m_hierarchyData[m_currentSelectedObjectIndex], m_hierarchyData[m_currentSelectedObjectIndex - 1]);
        m_currentSelectedObjectIndex--;
    }

    // Down object
    ImGui::SameLine();
    if (ImGui::Button("v") && m_isObjectSelected && m_currentSelectedObjectIndex < m_hierarchyData.size() - 1) {
        std::swap(m_hierarchyData[m_currentSelectedObjectIndex], m_hierarchyData[m_currentSelectedObjectIndex + 1]);
        m_currentSelectedObjectIndex++;
    }
    ImGui::PopStyleColor(2);

    // Display objects to hierarchy
    size_t hierarchySize = m_hierarchyData.size();
    for (size_t i = 0; i < hierarchySize; ++i) {
        if (ImGui::Selectable(m_hierarchyData[i].GetHierarchyName().GetWithIntKey().c_str(), m_currentSelectedObjectIndex == i && m_isObjectSelected)) {
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

// Fix hierarchy name to avoid duplicates
CStringWithIntKey CSceneRegistry::FixHierarchyName(const CStringWithIntKey* prevName, std::string name) {
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
        m_hierarchyNameUsed[prevName->GetUnKeyedString()].ReleaseKey(prevName->GetIntKey());
    }

    IntKey key = nameWithKey.GetIntKey();
    if (key > 0) {
        key = m_hierarchyNameUsed[nameWithKey.GetUnKeyedString()].GenerateNotDuplicateKey(key);
        nameWithKey.SetIntKey(key);
        return nameWithKey;
    }

    // Avoid name duplicates
    key = m_hierarchyNameUsed[nameWithKey.GetUnKeyedString()].GenerateKey();
    nameWithKey.SetIntKey(key);

    return nameWithKey;
}

// Delete game object from hierarchy
void CSceneRegistry::DeleteFromHierarchy(size_t index) {
    // Erase previous name from map
    auto it = m_hierarchyNameUsed.find(m_hierarchyData[index].GetHierarchyName().GetUnKeyedString());
    if (it != m_hierarchyNameUsed.end()) {
        if (it->second.IsNotKeyAssignment()) {
            m_hierarchyNameUsed.erase(it);
        }
        else {
            it->second.ReleaseKey(m_hierarchyData[index].GetHierarchyName().GetIntKey());
        }
    }

    // Sequre commands
    CCommandManager::GetMain().SecureCommands();

    // Erase
    m_hierarchyData[index].DestroyPrefab();
    m_hierarchyData.erase(m_hierarchyData.begin() + index);
    if (index >= m_hierarchyData.size()) {
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
            if (SceneFileSystem::SaveSceneData(m_name, m_renderPassAssetName, m_hierarchyData)) {
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
            if (SceneFileSystem::SaveSceneData(saveFilename, m_renderPassAssetName, m_hierarchyData)) {
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
            if (SceneFileSystem::LoadSceneData(loadName, &m_renderPassAssetName, &m_hierarchyData)) {
                // Secure commands
                CCommandManager::GetMain().SecureCommands();

                ImGui::CloseCurrentPopup();
                m_isObjectSelected = false;
                m_hierarchyNameUsed.clear();

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
                for (auto& it : m_hierarchyData) {
                    it.RebuildCurrentPrefab(m_currentScene.Get());
                    m_hierarchyNameUsed[it.GetHierarchyName().GetUnKeyedString()].RegisterUsedKey(it.GetHierarchyName().GetIntKey());
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
        bool tmp = m_isColliderDrawing;
        ImGui::Checkbox("Drawing Collider", &tmp);
        if (tmp != m_isColliderDrawing) {
            m_isColliderDrawing = tmp;
            for (auto& it : m_hierarchyData) {
                it.SetDrawingCollider(m_isColliderDrawing);
            }
        }

        // Display fps
        tmp = m_isDisplayFps;
        ImGui::Checkbox("Display FPS", &tmp);
        if (tmp != m_isDisplayFps) {
            m_isDisplayFps = tmp;
        }

        // Input FPS
        ImGui::Text("FPS");
        ImGui::SameLine();
        float fps = 1.0f / CAppClock::GetAny().GetFrameTime();
        ImGui::InputFloat("##FPS", &fps, 1.0f, 1.0f, "%.1f");
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            if (fps > FLT_EPSILON) {
                CAppClock::GetMain().SetFPS(fps);
                m_currentScene->GetPhysicsWorld()->ReassignFixedTimeStep();
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
        char hierarchyName[256];
        strncpy_s(hierarchyName, m_hierarchyData[m_currentSelectedObjectIndex].GetHierarchyName().GetWithIntKey().c_str(), sizeof(hierarchyName));
        hierarchyName[sizeof(hierarchyName) - 1] = '\0';

        // Hierarchy name
        ImGui::Text("Hierarchy Name");
        ImGui::SameLine();
        ImGui::InputText("##Hierarchy Name", hierarchyName, IM_ARRAYSIZE(hierarchyName), INPUT_TEXT_FLAG, SceneFileSystem::ProhibitedCharacterCallback);
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            std::string afterName = hierarchyName;

            if (!afterName.empty()) {
                m_hierarchyData[m_currentSelectedObjectIndex].SetHierarchyName(
                    FixHierarchyName(&m_hierarchyData[m_currentSelectedObjectIndex].GetHierarchyName(), hierarchyName));
            }
        }

        // Prefab kind
        ImGui::Text("Prefab");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Prefab", m_hierarchyData[m_currentSelectedObjectIndex].GetPrefabName().c_str())) {
            // Display prefab class name
            const auto& map = ACRegistrarForGameObject::GetClassMap();
            for (const auto& it : map) {
                bool isSelected = (it.first == m_hierarchyData[m_currentSelectedObjectIndex].GetPrefabName());
                // If changed
                if (ImGui::Selectable(it.first.c_str(), isSelected)) {
                    m_hierarchyData[m_currentSelectedObjectIndex].SetPrefab(m_currentScene.Get(), it.first);
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::TreeNode("Transform")) {
            // Position
            ImGui::Text("Position");
            ImGui::SameLine();
            Vector3f pos = m_hierarchyData[m_currentSelectedObjectIndex].GetPos();
            if (ImGui::InputFloat3("##Position", pos.GetArrayPtr(), "%.2f", ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackCharFilter, ProhibitedNonNumericCallback)) {
                m_hierarchyData[m_currentSelectedObjectIndex].SetPos(pos);
            }

            // Scale
            ImGui::Text("Scale");
            ImGui::SameLine();
            Vector3f scale = m_hierarchyData[m_currentSelectedObjectIndex].GetScale();
            if (ImGui::InputFloat3("##Scale", scale.GetArrayPtr(), "%.2f", ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackCharFilter, ProhibitedNonNumericCallback)) {
                m_hierarchyData[m_currentSelectedObjectIndex].SetScale(scale);
            }

            // Eular angle
            ImGui::Text("Angle");
            ImGui::SameLine();
            Vector3f eularAngle = m_hierarchyData[m_currentSelectedObjectIndex].GetEularAngle();
            if (ImGui::InputFloat3("##Angle", eularAngle.GetArrayPtr(), "%.2f", ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackCharFilter, ProhibitedNonNumericCallback)) {
                m_hierarchyData[m_currentSelectedObjectIndex].SetEularAngle(eularAngle);
            }

            ImGui::TreePop();
        }

        ImGui::End();
    }
}
#endif // _EDITOR
