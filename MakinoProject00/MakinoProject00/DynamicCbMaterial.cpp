#include "DynamicCbMaterial.h"

// Allocate data
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicCbMaterial::AllocateData(ACGraphicsComponent* component, UINT meshIndex) {
    const PerMeshMaterialNumeric& mat = component->GetMaterialNumeric(meshIndex);

    MaterialInHlsl data;
    data.color = component->GetColor();
    data.shininessPeroperty = Vector2f(mat.shininess, mat.shininessScale);

    return DirectDataCopy(&data);
}