#include "CommandManager.h"
#include "Application.h"
#include "UtilityForException.h"

// Destructor
CCommandManager::~CCommandManager() {
    // Wait for GPU
    GetAny().WaitForGPU();

    for (UINT i = 0; i < SCREEN_BUFFERING_NUM; ++i) {
        // Close command list
        HR_CHECK_OUTWIND("Close command list for graphics",
            m_graphicsCmdList[i]->Close()
        );

        // Clear commands
        GetAny().UnsafeGraphicsCommandsClear(i);
    }
}

// Initialize
void CCommandManager::Initialize() {
    // Create graphics command allocater
    for (UINT i = 0; i < SCREEN_BUFFERING_NUM; ++i) {
        HR_CHECK(L"Create graphics command allocater",
            CApplication::GetAny().GetDXDevice()->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_graphicsCmdAllocator[i].GetAddressOf()))
        );
    }

    // Create graphics command list
    for (UINT i = 0; i < SCREEN_BUFFERING_NUM; ++i) {
        HR_CHECK(L"Create graphics command list",
            CApplication::GetAny().GetDXDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                m_graphicsCmdAllocator[i].Get(), nullptr, IID_PPV_ARGS(m_graphicsCmdList[i].GetAddressOf())));
    }

    // Create command queue descriptor
    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
    cmdQueueDesc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;      // General setting
    cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // Default
    cmdQueueDesc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;       // Deafult
    cmdQueueDesc.NodeMask = 0;                                   // Single adapter
    // Create command queue
    HR_CHECK(L"Create command queue", 
        CApplication::GetAny().GetDXDevice()->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(m_cmdQueue.GetAddressOf())));

    // Create fence object
    m_fenceValue = 0;
    HR_CHECK(L"Create fence object", 
        CApplication::GetAny().GetDXDevice()->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())));
}

// Execute registered commands
void CCommandManager::CommandsExecute(CommandType type, bool isWaitForGPU) {
    // Just execute the commands
    UnsafeGraphicsCommandsExecute();
    
    // Wait for the GPU to complete processing
    GetAny().WaitForGPU();

    // Just clear the commands
    GetAny().UnsafeGraphicsCommandsClear(CSwapChain::GetMain().GetNextBackBufferIndex());
}

// Executes registered graphics commands, but does not wait or clear, so it is only used in special situations
void CCommandManager::UnsafeGraphicsCommandsExecute() {
    // Get next back buffer index
    UINT bufferIndex = CSwapChain::GetMain().GetNextBackBufferIndex();

    // Get graphics command type
    ID3D12CommandList* cmdList[1] = { m_graphicsCmdList[bufferIndex].Get() };
    // Close command list
    HR_CHECK_OUTWIND("Close command list for graphics",
        m_graphicsCmdList[bufferIndex]->Close()
    );

    // Execute command list
    m_cmdQueue->ExecuteCommandLists(1, cmdList);
}

// Clear graphics command list and graphics command allocator, However, it is not associated with execution and is used only in special situations
void CCommandManager::CThreadSafeFeature::UnsafeGraphicsCommandsClear(UINT index) {
    // Reset command allocater
    HR_CHECK_OUTWIND("Reset command allocater for graphics",
        m_owner->m_graphicsCmdAllocator[index]->Reset()
    );

    // Reset command list
    HR_CHECK_OUTWIND("Reset command list for graphics",
        m_owner->m_graphicsCmdList[index]->Reset(m_owner->m_graphicsCmdAllocator[index].Get(), nullptr)
    );
}

// Wait for the GPU to complete processing
void CCommandManager::CThreadSafeFeature::WaitForGPU() {
    HR_CHECK_OUTWIND("Add signal instruction to command queue",
        m_owner->m_cmdQueue->Signal(m_owner->m_fence.Get(), ++m_owner->m_fenceValue)
    );

    // Wait for GPU
    if (m_owner->m_fence->GetCompletedValue() != m_owner->m_fenceValue) {
        // Get event handle
        HANDLE eventH = CreateEvent(nullptr, false, false, nullptr);
        if (eventH != nullptr) {
            // When the fence value is updated, put the event in signal state
            HRESULT hr = m_owner->m_fence->SetEventOnCompletion(m_owner->m_fenceValue, eventH);
            // Is successful
            if (SUCCEEDED(hr)) {
                // Wait until the event is in signal state
                WaitForSingleObject(eventH, INFINITE);

                // Close event handle
                CloseHandle(eventH);
            }
            // Is unsuccessful
            else {
                // Close event handle
                CloseHandle(eventH);
            }
        }
    }
}
