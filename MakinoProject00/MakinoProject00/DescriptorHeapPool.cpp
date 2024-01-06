#include "DescriptorHeapPool.h"
#include "CommandManager.h"

// Initialize
void CDescriptorHeapPool::Initialize(UINT numPerHeap, UINT initNum) {
    m_numPerDescriptorHeap = numPerHeap;
    for (UINT i = 0; i < SCREEN_BUFFERING_NUM; ++i) {
        for (UINT j = 0; j < initNum; ++j) {
            m_descriptorHeap[i].emplace_back();
            m_descriptorHeap[i][j].Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, m_numPerDescriptorHeap);
        }
    }
}

// Drawing preparation processing necessary every frame
void CDescriptorHeapPool::ReadyToDraw() {
    m_currentArrayIndex = 0; 
    m_currentUsedRegion = 0; 

    // Register as descriptor heap to be used
    CCommandManager::GetMain().GetGraphicsCmdList()->SetDescriptorHeaps(1, m_descriptorHeap[CSwapChain::GetMain().GetNextBackBufferIndex()][m_currentArrayIndex].GetHeapAddress());
}

// Advance to the next descriptor heap
void CDescriptorHeapPool::AdvanceNextDescriptorHeap() {
    // Get index of next back buffer
    UINT backBufferIndex = CSwapChain::GetMain().GetNextBackBufferIndex();

    // Initialize the region of use
    m_currentUsedRegion = 0;

    // Advance to the next descriptor heap and if there are not enough descriptor heaps, create a new one
    m_currentArrayIndex++;
    if (m_currentArrayIndex >= (UINT)m_descriptorHeap[backBufferIndex].size()) {
        m_descriptorHeap[backBufferIndex].emplace_back();
        m_descriptorHeap[backBufferIndex][m_currentArrayIndex].Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, m_numPerDescriptorHeap);
    }

    // Register as descriptor heap to be used
    CCommandManager::GetMain().GetGraphicsCmdList()->SetDescriptorHeaps(1, m_descriptorHeap[backBufferIndex][m_currentArrayIndex].GetHeapAddress());
}
