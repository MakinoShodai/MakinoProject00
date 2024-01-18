#include "ModelLoadSetting.h"
#include "AssetNameDefine.h"
#include "ModelRegistry.h"

// Performs setting when loading models
void ModelLoadSetting() {
    using namespace ModelInfo;
    using namespace ModelInfo::Load;
    CModelRegistry& registry = CModelRegistry::GetMain();

    // Model setting
    registry.AddModelLoadDesc(MODEL_NAME_DESK, ModelDesc(0.025f, CoordinateSystem::Blender));
    registry.AddModelLoadDesc(MODEL_NAME_CUTEBIRD, ModelDesc(0.075f, CoordinateSystem::Standard, 0.2f));

    // Animation setting
    registry.AddAnimLoadDesc(ANIM_NAME_CUTEBIRD_DIE, AnimDesc(0.25f));
    registry.AddAnimLoadDesc(ANIM_NAME_CUTEBIRD_RUN, AnimDesc(0.75f));
}
