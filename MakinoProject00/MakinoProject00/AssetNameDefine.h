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
    /** @brief Texture name for test */
    const std::wstring CHECKERBOARD_TEX = L"Texture/checkerboard.png";
    /** @brief Texture name for soil */
    const std::wstring SOIL_TEX = L"Texture/soil.jpg";
    /** @brief Texture name for sky dome */
    const std::wstring SKYDOME = L"Texture/skydome.png";
    /** @brief Texture name for grass */
    const std::wstring GRASS = L"Texture/grass_01.png";
    /** @brief Texture name for capsule tablet */
    const std::wstring CAPSULE_TABLET_TEX = L"Texture/capsuleTabletTex.png";
    /** @brief Texture name for basic face texture of cute bird */
    const std::wstring CUTEBIRD_FACE_BASIC = L"Model/Cutebird/Dino_Face01.png";
    /** @brief Texture name for die face texture of cute bird */
    const std::wstring CUTEBIRD_FACE_DIE = L"Model/Cutebird/Dino_Face18.png";
    /** @brief Texture name for plants texture of tree */
    const std::wstring TREE_PLANTS = L"Model/Trees/Plants.png";
} // namespace TexName

namespace ModelName {
    /** @brief Model name for coordinate axis */
    const std::wstring COORD_AXIS = L"Axis/axis.fbx";
    /** @brief Model name for cute bird */
    const std::wstring CUTEBIRD = L"Cutebird/Cute_Bird_b.fbx";

    /** @brief Model name for middle stone 01 */
    const std::wstring STONE_MIDDLE01 = L"Stones/middle_stone_01.fbx";
    /** @brief Model name for middle stone 02 */
    const std::wstring STONE_MIDDLE02 = L"Stones/middle_stone_02.fbx";
    /** @brief Model name for middle stone 03 */
    const std::wstring STONE_MIDDLE03 = L"Stones/middle_stone_03.fbx";
    /** @brief Model name for small stone 01 */
    const std::wstring STONE_SMALL01 = L"Stones/small_stone_01.fbx";
    /** @brief Model name for small stone 02 */
    const std::wstring STONE_SMALL02 = L"Stones/small_stone_02.fbx";
    /** @brief Model name for small stone 03 */
    const std::wstring STONE_SMALL03 = L"Stones/small_stone_03.fbx";
    /** @brief Model name for small stone 04 */
    const std::wstring STONE_SMALL04 = L"Stones/small_stone_04.fbx";
    /** @brief Model name for small stone 05 */
    const std::wstring STONE_SMALL05 = L"Stones/small_stone_05.fbx";
    /** @brief Model name for small stone 06 */
    const std::wstring STONE_SMALL06 = L"Stones/small_stone_06.fbx";

    /** @brief Model name for big brick 01 */
    const std::wstring BRICK_BIG01 = L"Ruins/big_brick_01.fbx";
    /** @brief Model name for big brick 02 */
    const std::wstring BRICK_BIG02 = L"Ruins/big_brick_02.fbx";
    /** @brief Model name for big brick 04 */
    const std::wstring BRICK_BIG04 = L"Ruins/big_brick_04.fbx";
    /** @brief Model name for column 03 */
    const std::wstring COLUMN03 = L"Ruins/column_03.fbx";
    /** @brief Model name for wall element 02 */
    const std::wstring WALL_ELEM_02 = L"Ruins/wall_elem_02.fbx";

    /** @brief Model name for tree 01 */
    const std::wstring TREE01 = L"Trees/tree_01.fbx";
    /** @brief Model name for tree 02 */
    const std::wstring TREE02 = L"Trees/tree_02.fbx";
    /** @brief Model name for tree 03 */
    const std::wstring TREE03 = L"Trees/tree_03.fbx";
    /** @brief Model name for tree 04 */
    const std::wstring TREE04 = L"Trees/tree_04.fbx";
    /** @brief Model name for tree 05 */
    const std::wstring TREE05 = L"Trees/tree_05.fbx";
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

