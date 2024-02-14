#include "DynamicCbTexCoordParam.h"

// Allocate data for no component
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicCbTexCoordParam::AllocateDataNoComponent(const Vector2f& tiling, const Vector2f& offset) {
    Vector4f data(tiling.x(), tiling.y(), offset.x(), offset.y());
    return DirectDataCopy(&data);
}

// Allocate data
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicCbTexCoordParam::AllocateData(ACGraphicsComponent* component) {
    return DirectDataCopy(&component->GetTexCoordParam());
}
