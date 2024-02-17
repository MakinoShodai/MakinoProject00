/**
 * @file   GraphicsLayerRegistry.h
 * @brief  This file handles drawing components for each layer.
 * 
 * @author Shodai Makino
 * @date   2023/11/19
 */

#ifndef __GRAPHICS_LAYER_REGISTRY_H__
#define __GRAPHICS_LAYER_REGISTRY_H__

#include "Singleton.h"
#include "GraphicsLayer.h"
#include "UtilityForException.h"
#include "GraphicsComponent.h"
#include "Sprite.h"
#include "Model.h"
#include "Shape.h"

/** @brief Variant class for pointer to dynamic array of graphics components */
using GraphicsComponentVectorVariantPtr = std::variant<
    std::vector<CSpriteUI*>*,
    std::vector<CSprite3D*>*,
    std::vector<CBillboard*>*,
    std::vector<CBasicModel*>*,
    std::vector<CSkeletalModel*>*,
    std::vector<CTexShape*>*,
    std::vector<CColorOnlyShape*>*,
    std::vector<CDebugColliderShape*>*
>;

/** @brief Variant class for const pointer to dynamic array of graphics components */
using ConstGraphicsComponentVectorVariantPtr = Utl::Type::Conv::ConstVariant<GraphicsComponentVectorVariantPtr>::type;

/** @brief Evaluation expression for 'GraphicsComponentVectorVariantPtr' */
template <typename T>
struct CheckGraphicsComponentVectorVariantPtr {
    using VecValueType = typename std::remove_pointer_t<T>::value_type;
    static constexpr bool value =
        std::is_base_of_v<ACGraphicsComponent, std::remove_pointer_t<VecValueType>>
        && std::is_pointer_v<T>&& std::is_pointer_v<VecValueType>;
};

// Check internal types of GraphicsComponentVectorVariantPtr
static_assert(Utl::Type::Traits::CheckVariantTypes<GraphicsComponentVectorVariantPtr, CheckGraphicsComponentVectorVariantPtr>(),
    L"All types in GraphicsComponentVectorVariantPtr must be std::vector<>* of pointer type to ACGraphicsComponent's derived classes");

/** @brief Each vector of graphics components */
struct EachGraphicsComponentVector {
    /** @brief Dynamic array for CSpriteUI */
    std::vector<CSpriteUI*> spriteUIs;
    /** @brief Dynamic array for CSprite3D */
    std::vector<CSprite3D*> sprite3Ds;
    /** @brief Dynamic array for CBillboard */
    std::vector<CBillboard*> billboards;
    /** @brief Dynamic array for CBasicModel */
    std::vector<CBasicModel*> basicModels;
    /** @brief Dynamic array for CSkeletalModel */
    std::vector<CSkeletalModel*> skeletalModels;
    /** @brief Dynamic array for CTexShape */
    std::vector<CTexShape*> texShapes;
    /** @brief Dynamic array for CColorOnlyShape */
    std::vector<CColorOnlyShape*> colorShapes;
    /** @brief Dynamic array for CDebugColliderShape */
    std::vector<CDebugColliderShape*> colliderShapes;

    /**
       @brief Get dynamic array corresponding to the specified type
       @param type Type of graphcis component
       @return Pointer to dynamic array corresponding to the specified type
    */
    inline GraphicsComponentVectorVariantPtr GetTypeArrayPtr(GraphicsComponentType type) { return InnerGetTypeArrayPtr(this, type); }

    /**
       @brief Get dynamic array corresponding to the specified type
       @param type Type of graphcis component
       @return Pointer to dynamic array corresponding to the specified type
    */
    inline const ConstGraphicsComponentVectorVariantPtr GetTypeArrayConstPtr(GraphicsComponentType type) const { return InnerGetTypeArrayPtr(this, type); }

    /**
       @brief Check if the array of the specified type is empty
       @param type Type of graphcis component
       @return Result
    */
    bool CheckEmpty(GraphicsComponentType type) const;

private:
    /**
       @brief Get dynamic array corresponding to the specified type (inside function)
       @param instance Pointer to instance
       @param type Type of graphcis component
       @return Pointer to dynamic array corresponding to the specified type
    */
    template<class T>
    static auto InnerGetTypeArrayPtr(T* instance, GraphicsComponentType type) {
        using VariantPtrType = std::conditional_t<std::is_const_v<T>, ConstGraphicsComponentVectorVariantPtr, GraphicsComponentVectorVariantPtr>;

        switch (type) {
        case GraphicsComponentType::SpriteUI:
            return static_cast<VariantPtrType>(&instance->spriteUIs);
        case GraphicsComponentType::Sprite3D:
            return static_cast<VariantPtrType>(&instance->sprite3Ds);
        case GraphicsComponentType::Billboard:
            return static_cast<VariantPtrType>(&instance->billboards);
        case GraphicsComponentType::BasicModel:
            return static_cast<VariantPtrType>(&instance->basicModels);
        case GraphicsComponentType::SkeletalModel:
            return static_cast<VariantPtrType>(&instance->skeletalModels);
        case GraphicsComponentType::TexShape:
            return static_cast<VariantPtrType>(&instance->texShapes);
        case GraphicsComponentType::ColorShape:
            return static_cast<VariantPtrType>(&instance->colorShapes);
        case GraphicsComponentType::DebugColliderShape:
            return static_cast<VariantPtrType>(&instance->colliderShapes);
        default:
            throw Utl::Error::CFatalError(L"Dynamic array corresponding to the specified type doesn't exist");
        }
    }
};

 /** @brief This class handles drawing components for each layer */
class CGraphicsLayerRegistry {
public:
    /**
       @brief Constructor
    */
    CGraphicsLayerRegistry() = default;

    /**
       @brief Destructor
    */
    ~CGraphicsLayerRegistry() = default;

    /**
       @brief Register graphics component in the registry
       @param component Graphics component to be registred
    */
    void Register(ACGraphicsComponent* component);

    /**
       @brief Exclude graphcis component from the registry
       @param component Graphics component to be excluded
    */
    void Exclude(ACGraphicsComponent* component);

    /**
       @brief Check if the graphics components to be drawn exists
       @param layers Layers to be checked
       @param types The type to check to see if it exists in the layer
       @return Returns true if they exists
    */
    bool CheckExists(const std::vector<GraphicsLayer>& layers, const std::vector<GraphicsComponentType>& types);

    /** @brief Get all components belonging to a layer */
    const EachGraphicsComponentVector& GetLayerComponents(GraphicsLayer layer) { return m_layerComponents[(GraphicsLayerUINT)layer]; }

private:
    /** @brief Array of graphics components for each layer */
    EachGraphicsComponentVector m_layerComponents[(GraphicsLayerUINT)GraphicsLayer::Max];
};

#endif // !__GRAPHICS_LAYER_REGISTRY_H__
