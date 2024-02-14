#include "GpuOptimizedResource.h"
#include "Application.h"
#include "UtilityForDirectX.h"
#include "CommandManager.h"
#include "UtilityForException.h"

// Create resource
void ACGpuOptimizedResource::CreateResource(UINT64 byteSize, D3D12_RESOURCE_STATES resourceState, MappingFunction mapFunc) {
    // Create heap properties and resource descriptor
    Utl::Dx::HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD); // Accessible from CPU
    Utl::Dx::RESOURCE_DESC resDesc(byteSize); // Bytes of all data

    // Create intermidiate buffer
    Microsoft::WRL::ComPtr<ID3D12Resource> interBuffer;
    HR_CHECK(L"Create intermidiate buffer",
        CApplication::GetAny().GetDXDevice()->CreateCommittedResource(
            &heapProp, D3D12_HEAP_FLAG_NONE,             // Default
            &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, // Upload heap
            nullptr, IID_PPV_ARGS(interBuffer.GetAddressOf())
        )
    );

    // Update heap properties and resource descriptor for resource
    UpdateDataForResource(&heapProp, &resDesc);

    // Create resource
    HR_CHECK(L"Create resource",
        CApplication::GetAny().GetDXDevice()->CreateCommittedResource(
            &heapProp, D3D12_HEAP_FLAG_NONE,          // Default
            &resDesc, D3D12_RESOURCE_STATE_COPY_DEST, // Copy destination
            nullptr, IID_PPV_ARGS(m_resource.GetAddressOf())
        )
    );

    // Mapping starts
    UINT8* mappedBuffer = nullptr;
    interBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedBuffer));

    // Mapping data
    mapFunc(mappedBuffer);

    // Mapping ends
    interBuffer->Unmap(0, nullptr);

    // Copy resource
    auto cmdList = CCommandManager::GetMain().GetGraphicsCmdList();
    CopyRegion(cmdList, interBuffer.Get());

    // Create resource barrier
    Utl::Dx::RESOURCE_BARRIER barrier(m_resource.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST, // Copy destination
        resourceState // Specified state
    );

    // Transition the state of texture resource using a resource barrier
    CCommandManager::GetMain().GetGraphicsCmdList()->ResourceBarrier(1, &barrier);

    // Execute commands
    CCommandManager::GetMain().CommandsExecute(CommandType::Graphics, true);
}
