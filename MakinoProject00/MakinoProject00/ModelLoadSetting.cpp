#include "ModelLoadSetting.h"
#include "AssetNameDefine.h"
#include "ModelRegistry.h"

// Performs setting when loading models
void ModelLoadSetting() {
    // Model setting
    CModelRegistry::GetMain().AddModelLoadDesc(MODEL_NAME_UNITYCHAN, ModelInfo::Load::ModelDesc(0.025f, ModelInfo::Load::CoordinateSystem::Standard, 1.0f));
    CModelRegistry::GetMain().AddModelLoadDesc(MODEL_NAME_DESK, ModelInfo::Load::ModelDesc(0.025f, ModelInfo::Load::CoordinateSystem::Blender));

    // Animation setting
    CModelRegistry::GetMain().AddAnimLoadDesc(ANIM_NAME_UNITYCHAN_JUMP, ModelInfo::Load::AnimDesc(1.0f));
    CModelRegistry::GetMain().AddAnimLoadDesc(ANIM_NAME_UNITYCHAN_RUN, ModelInfo::Load::AnimDesc(1.0f, true));

    // Animation interpolation setting
    CModelRegistry::GetMain().AddAnimToAnimSetting(ANIM_NAME_UNITYCHAN_JUMP, ANIM_NAME_UNITYCHAN_RUN, ModelInfo::InterpolationSetting(5.0f));
    CModelRegistry::GetMain().AddAnimToAnimSetting(ANIM_NAME_UNITYCHAN_RUN, ANIM_NAME_UNITYCHAN_JUMP, ModelInfo::InterpolationSetting(0.3f));
}
