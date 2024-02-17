/**
 * @file   ModelLoadSetting.h
 * @brief  This file performs setting when loading models.
 * 
 * @author Shodai Makino
 * @date   2024/1/11
 */

#ifndef __MODEL_LOAD_SETTING_H__
#define __MODEL_LOAD_SETTING_H__

/** @brief Namespace where additional texture ID of model is defined */
namespace AddTexID {
    const int CUTEBIRD_FACE_DIE = 0;
} // AddTexID

/** @brief Performs setting when loading models */
void ModelLoadSetting();

#endif // !__MODEL_LOAD_SETTING_H__
