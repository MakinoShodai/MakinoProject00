#include "DynamicSbRegistry.h"
#include "DynamicSbAnimMat.h"

// Constructor
CDynamicSbRegistry::CDynamicSbRegistry() {
    EmplaceSbAllocator<CDynamicSbAnimMat>();
}

// Refresh process that must be called at the end of every frame
void CDynamicSbRegistry::FrameRefresh() {
    for (auto& it : m_sbAllocators) {
        it->Refresh();
    }
}

// Release strutured buffers corresponding to the component sent
void CDynamicSbRegistry::ReleaseBuffer(ACGraphicsComponent* component) {
    for (auto& it : m_sbAllocators) {
        it->ReleaseBuffer(component);
    }
}
