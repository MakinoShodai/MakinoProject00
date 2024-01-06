#include "DynamicCbColor.h"

// Color
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicCbColor::AllocateData(ACGraphicsComponent* component) {
    return DirectDataCopy(&component->GetColor());
}
