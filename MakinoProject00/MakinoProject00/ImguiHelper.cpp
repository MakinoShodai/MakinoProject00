#include "ImguiHelper.h"
#include "SwapChain.h"
#include "CommandManager.h"

// Initialize
void CImguiHelper::Initialize() {
    // Check version
    IMGUI_CHECKVERSION();
    // Create context
    ImGui::CreateContext();

    // Enable keyboard
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Create descriptor heap for imgui
    m_descriptorHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);

    // Set style color
    ImGui::StyleColorsDark();

    // Initialize imgui
    ImGui_ImplWin32_Init(CApplication::GetAny().GetWndHandle());
    ImGui_ImplDX12_Init(CApplication::GetAny().GetDXDevice(), SCREEN_BUFFERING_NUM,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, m_descriptorHeap.GetHeap(),
        m_descriptorHeap.GetCpuHandle(),
        m_descriptorHeap.GetGpuHandle());
}

// Drawing process
void CImguiHelper::Draw() {
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
        // Lock
        std::lock_guard<std::mutex> lock(m_mutexToCreate);

        // Set the instance to be created as already created
        for (auto& it : m_windowInstanceToCreate) {
            if (it == nullptr) { continue; }
            m_windowInstance.emplace_back(std::move(it));
        }
        m_windowInstanceToCreate.clear();
    }

    for (size_t i = 0; i < m_windowInstance.size(); ++i) {
        // If the function is already deleted, erase it
        if (m_windowInstance[i] == nullptr) {
            m_windowInstance.erase(m_windowInstance.begin() + i);
            i--;
            continue;
        }

        // Call the function and if false is returned, erase the instance
        if (false == (*m_windowInstance[i])()) {
            m_windowInstance.erase(m_windowInstance.begin() + i);
            i--;
            continue;
        }
    }

    // Set descriptor heap to command list
    ID3D12GraphicsCommandList* cmdList = CCommandManager::GetMain().GetGraphicsCmdList();
    cmdList->SetDescriptorHeaps(1, m_descriptorHeap.GetHeapAddress());

    // Render process
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
}

// Add window function
void CImguiHelper::CThreadSafeFeature::AddWindowFunction(CSyncSharedPtr<ImguiWindowFunction> function) {
    if (function == nullptr) {
        return;
    }
    // Lock
    std::lock_guard<std::mutex> lock(m_owner->m_mutexToCreate);
    // Add to instances to be created
    m_owner->m_windowInstanceToCreate.emplace_back(std::move(function));
}

// Add window function
void CImguiHelper::CThreadSafeFeature::AddWindowFunction(ImguiWindowFunction function) {
    // Lock
    std::lock_guard<std::mutex> lock(m_owner->m_mutexToCreate);
    // Add to instances to be created
    m_owner->m_windowInstanceToCreate.emplace_back(CSyncSharedPtr<ImguiWindowFunction>::Make(std::move(function)));
}

// Process to be called at instance destruction
void CImguiHelper::OnDestroy() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
