﻿#include "DynamicCbRegistry.h"
#include "DynamicCbWorldMat.h"
#include "DynamicCbColor.h"

// Initialize
void CDynamicCbRegistry::Initialize() {
    EmplaceCbAllocator<CDynamicCbWorldMat>(100);
    EmplaceCbAllocator<CDynamicCbColor>(100);
}

// Refresh process that must be called at the end of every frame
void CDynamicCbRegistry::FrameRefresh() {
    for (auto& it : m_cbAllocators) {
        it->Refresh();
    }
}
