#include "GraphicsLayerRegistry.h"

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
