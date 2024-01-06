#include "DescriptorHeap.h"
#include "UtilityForException.h"

// Create descriptor heap
void CDescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT numDescriptors) {
    // Create descriptor for descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = type;
    desc.Flags = flags;
    desc.NumDescriptors = numDescriptors;
    desc.NodeMask = 0;

    // Create descriptor heap
    HR_CHECK(_T("Create descriptor heap"),
        CApplication::GetAny().GetDXDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_descriptorHeap.GetAddressOf())));

    // Get handles for descriptor heap
    m_cpuHandle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    m_gpuHandle = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
}
