#include "DynamicCbColor.h"

// Allocate data for no component
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicCbColor::AllocateDataNoComponent(const Colorf& color) {
    return DirectDataCopy(&color);
}

// Color
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicCbColor::AllocateData(ACGraphicsComponent* component) {
    return DirectDataCopy(&component->GetColor());
}
