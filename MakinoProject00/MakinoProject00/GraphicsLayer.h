/**
 * @file   GraphicsLayer.h
 * @brief  This file handles the layer function in the graphics processing.
 *
 * @author Shodai Makino
 * @date   2023/11/19
 */

#ifndef __GRAPHICS_LAYER_H__
#define __GRAPHICS_LAYER_H__

/** @brief Internal type of the graphics layer enumeration type */
using GraphicsLayerUINT = UINT32;

/** @brief Graphics layer type */
enum class GraphicsLayer : GraphicsLayerUINT {
    /** @brief Standard layer */
    Standard = 0,
    /** @brief User interface */
    UI,
    /** @brief Translucent object */
    Transparent,
    /** @brief Translucent and no culling object */
    TransparentNoCulling,
    /** @brief Shading that both writes and reads */
    ReadWriteShading,
    /** @brief Shading (read only) */
    ReadOnlyShading,
    /** @brief Sky dome */
    SkyDome,
    /** @brief Shading that both writes and reads for grass */
    ReadWriteShadingForGrass,
    /** @brief The maximum number of layer */
    Max,
};

/**
   @brief Convert from 'GraphicsLayer' to string
   @param layer Layer to be converted
   @return Converted string
*/
inline std::wstring GraphicsLayerEnumToString(GraphicsLayer layer) {
    switch (layer) {
    case GraphicsLayer::Standard:
        return L"STANDARD";
    case GraphicsLayer::UI:
        return L"UI";
    case GraphicsLayer::Transparent:
        return L"Transparent";
    case GraphicsLayer::ReadWriteShading:
        return L"ReadWriteShading";
    case GraphicsLayer::Max:
        return L"MAX";
    default:
        return L"Unknown";
    }
}

#endif // !__GRAPHICS_LAYER_H__
