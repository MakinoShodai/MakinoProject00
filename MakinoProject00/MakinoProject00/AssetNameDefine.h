/**
 * @file   AssetNameDefine.h
 * @brief  This file handles header file defining the asset names.
 * 
 * @author Shodai Makino
 * @date   2024/1/11
 */

#ifndef __ASSET_NAME_DEFINE_H__
#define __ASSET_NAME_DEFINE_H__

namespace TexName{
    /** @brief Texture name for test image */
    const std::wstring TEST = L"Texture/textest200x200.png";
    /** @brief Texture name for test image 2 */
    const std::wstring TEST2 = L"Texture/apollo11.png";
    /** @brief Texture name for basic face texture of cute bird */
    const std::wstring CUTEBIRD_FACE_BASIC = L"Model/Cutebird/Dino_Face01.png";
    /** @brief Texture name for die face texture of cute bird */
    const std::wstring CUTEBIRD_FACE_DIE = L"Model/Cutebird/Dino_Face18.png";
} // namespace TexName

namespace ModelName {
    /** @brief Model name for desk */
    const std::wstring DESK = L"desk/desk.fbx";
    /** @brief Model name for cute bird */
    const std::wstring CUTEBIRD = L"Cutebird/Cute_Bird_b.fbx";
} // namespace ModelName

namespace AnimName {
    /** @brief Animation name for idle animation of cute bird */
    const std::wstring CUTEBIRD_IDLE = L"Cutebird/Cutebird_IdleB.fbx";
    /** @brief Animation name for run animation of cute bird */
    const std::wstring CUTEBIRD_RUN = L"Cutebird/Cutebird_Move.fbx";
    /** @brief Animation name for jump animation of cute bird */
    const std::wstring CUTEBIRD_JUMP = L"Cutebird/Cutebird_Jump.fbx";
    /** @brief Animation name for die animation of cute bird */
    const std::wstring CUTEBIRD_DIE = L"Cutebird/Cutebird_DieB.fbx";
} // namespace AnimName

#endif // !__ASSET_NAME_DEFINE_H__

