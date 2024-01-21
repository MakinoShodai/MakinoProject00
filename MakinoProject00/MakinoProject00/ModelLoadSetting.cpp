#include "ModelLoadSetting.h"
#include "AssetNameDefine.h"
#include "ModelRegistry.h"

// Performs setting when loading models
void ModelLoadSetting() {
    using namespace ModelInfo;
    using namespace ModelInfo::Load;
    CModelRegistry& registry = CModelRegistry::GetMain();

    // Model setting
    registry.AddModelLoadDesc(ModelName::DESK, ModelDesc(0.025f, false, CoordinateSystem::Blender));
    registry.AddModelLoadDesc(ModelName::CUTEBIRD, ModelDesc(0.075f, false, CoordinateSystem::Standard, 0.2f));

    // Add model's texture
    registry.AddModelAdditionalTex(ModelName::CUTEBIRD, TexName::CUTEBIRD_FACE_BASIC,
        ModelInfo::Load::AdditionalModelTex(AddTexID::CUTEBIRD_FACE_DIE, TexName::CUTEBIRD_FACE_DIE));

    // Add model's transparent texture
    registry.AddModelTransparentTex(ModelName::CUTEBIRD, TexName::CUTEBIRD_FACE_BASIC);

    // Animation setting
    registry.AddAnimLoadDesc(AnimName::CUTEBIRD_DIE, AnimDesc(0.25f));
    registry.AddAnimLoadDesc(AnimName::CUTEBIRD_RUN, AnimDesc(0.75f));
}
