#include "GraphicsLayerRegistry.h"

// Check if the array of the specified type is empty
bool EachGraphicsComponentVector::CheckEmpty(GraphicsComponentType type) const {
    switch (type) {
    case GraphicsComponentType::SpriteUI:
        return spriteUIs.empty();
    case GraphicsComponentType::Sprite3D:
        return sprite3Ds.empty();
    case GraphicsComponentType::Billboard:
        return billboards.empty();
    case GraphicsComponentType::BasicModel:
        return basicModels.empty();
    case GraphicsComponentType::SkeletalModel:
        return skeletalModels.empty();
    case GraphicsComponentType::TexShape:
        return texShapes.empty();
    case GraphicsComponentType::ColorShape:
        return colorShapes.empty();
    case GraphicsComponentType::DebugColliderShape:
        return colliderShapes.empty();
    default:
        throw Utl::Error::CFatalError(L"Dynamic array corresponding to the specified type doesn't exist");
    }
}

// Register graphics component in the registry
void CGraphicsLayerRegistry::Register(ACGraphicsComponent* component) {
    GraphicsComponentVectorVariantPtr vecPtr = m_layerComponents[(GraphicsLayerUINT)component->GetLayer()].GetTypeArrayPtr(component->GetType());

    std::visit([component](auto& vecPtr) {
        using T = std::decay_t<decltype(*vecPtr)>;
        using ElementType = typename T::value_type;
        vecPtr->push_back(dynamic_cast<ElementType>(component));
    }, vecPtr);
}

// Exclude graphcis component from the registry
void CGraphicsLayerRegistry::Exclude(ACGraphicsComponent* component) {
    // Get the array of layer to which the graphics component now belongs
    GraphicsComponentVectorVariantPtr vecPtr = m_layerComponents[(GraphicsLayerUINT)component->GetLayer()].GetTypeArrayPtr(component->GetType());

    std::visit([component](auto& vecPtr) {
        using T = std::decay_t<decltype(*vecPtr)>;
        using ElementType = typename T::value_type;
        ElementType castComponent = dynamic_cast<ElementType>(component);

        // Find a match in the array
        auto it = std::find(vecPtr->begin(), vecPtr->end(), castComponent);
        if (it != vecPtr->end()) {
            // Delete when a match is found
            *it = std::move(vecPtr->back());
            vecPtr->pop_back();
        }
    }, vecPtr);
}

// Check if the graphics components to be drawn exists
bool CGraphicsLayerRegistry::CheckExists(const std::vector<GraphicsLayer>& layers, const std::vector<GraphicsComponentType>& types) {
    for (auto layer : layers) {
        EachGraphicsComponentVector& vector = m_layerComponents[(GraphicsLayerUINT)layer];

        for (auto type : types) {
            if (false == vector.CheckEmpty(type)) {
                return true;
            }
        }
    }

    return false;
}
