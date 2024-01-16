/**
 * @file   DynamicSbAllocator.h
 * @brief  This file handles class that allocates data for structured buffer per object.
 * 
 * @author Shodai Makino
 * @date   2023/12/16
 */

#ifndef __DYNAMIC_SB_ALLOCATOR_H__
#define __DYNAMIC_SB_ALLOCATOR_H__

#include "DynamicResource.h"
#include "AbstractSbAllocator.h"

/** @brief Interface for structured buffer allocator that need to be allocated per object */
class IDynamicSbAllocator : public IDynamicResource {
public:
    /**
       @brief Destructor
    */
    virtual ~IDynamicSbAllocator() override = default;

    /**
       @brief Release strutured buffers corresponding to the component sent
       @param graphics component that wants to release buffer
    */
    virtual void ReleaseBuffer(ACGraphicsComponent* graphics) = 0;

protected:
    /**
       @brief Create a strutured buffer corresponding to the component sent
       @param graphics component that wants to create buffer
    */
    virtual void CreateBuffer(ACGraphicsComponent* graphics) = 0;
    /** @brief Create a strutured buffer corresponding to the component sent */
    inline virtual void CreateBuffer(ACModel* model) { return CreateBuffer(static_cast<ACGraphicsComponent*>(model)); }
    /** @brief Create a strutured buffer corresponding to the component sent */
    inline virtual void CreateBuffer(CBasicModel* model) { return CreateBuffer(static_cast<ACModel*>(model)); }
    /** @brief Create a strutured buffer corresponding to the component sent */
    inline virtual void CreateBuffer(CSkeletalModel* model) { return CreateBuffer(static_cast<ACModel*>(model)); }
    /** @brief Create a strutured buffer corresponding to the component sent */
    inline virtual void CreateBuffer(ACSprite* sprite) { return CreateBuffer(static_cast<ACGraphicsComponent*>(sprite)); }
    /** @brief Create a strutured buffer corresponding to the component sent */
    inline virtual void CreateBuffer(CSpriteUI* sprite) { return CreateBuffer(static_cast<ACSprite*>(sprite)); }
    /** @brief Create a strutured buffer corresponding to the component sent */
    inline virtual void CreateBuffer(CSprite3D* sprite) { return CreateBuffer(static_cast<ACSprite*>(sprite)); }
    /** @brief Create a strutured buffer corresponding to the component sent */
    inline virtual void CreateBuffer(CBillboard* sprite) { return CreateBuffer(static_cast<ACSprite*>(sprite)); }
    /** @brief Create a strutured buffer corresponding to the component sent */
    inline virtual void CreateBuffer(CTexShape* shape) { return CreateBuffer(static_cast<ACGraphicsComponent*>(shape)); }
    /** @brief Create a strutured buffer corresponding to the component sent */
    inline virtual void CreateBuffer(CColorOnlyShape* shape) { return CreateBuffer(static_cast<ACGraphicsComponent*>(shape)); }
    /** @brief Create a strutured buffer corresponding to the component sent */
    inline virtual void CreateBuffer(CDebugColliderShape* shape) { return CreateBuffer(static_cast<ACGraphicsComponent*>(shape)); }
};

/** @brief Structured buffer allocator class that need to be allocated per object */
template<class StructBuffType>
class ACDynamicSbAllocatorPerMesh : public IDynamicSbAllocator, public ACSbAllocater<StructBuffType> {
public:
    /**
       @brief Constructor
       @param hlslBufferName The name of cbuffer defined in hlsl
    */
    ACDynamicSbAllocatorPerMesh(std::string hlslBufferName)
        : ACSbAllocater<StructBuffType>(std::move(hlslBufferName))
    {}

    /**
       @brief Destructor
    */
    virtual ~ACDynamicSbAllocatorPerMesh() = default;

    /**
       @brief Release strutured buffers corresponding to the component sent
       @param graphics component that wants to release buffer
    */
    void ReleaseBuffer(ACGraphicsComponent* graphics) override final;

    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CBasicModel* model, UINT meshIndex) override final { return InnerGetAllocateDataHandle(model, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CSkeletalModel* model, UINT meshIndex) override final { return InnerGetAllocateDataHandle(model, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CSpriteUI* sprite, UINT meshIndex) override final { return InnerGetAllocateDataHandle(sprite, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CSprite3D* sprite, UINT meshIndex) override final { return InnerGetAllocateDataHandle(sprite, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CBillboard* sprite, UINT meshIndex) override final { return InnerGetAllocateDataHandle(sprite, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CTexShape* shape, UINT meshIndex) override final { return InnerGetAllocateDataHandle(shape, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CColorOnlyShape* shape, UINT meshIndex) override final { return InnerGetAllocateDataHandle(shape, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CDebugColliderShape* shape, UINT meshIndex) override final { return InnerGetAllocateDataHandle(shape, meshIndex); }

    /**
       @brief Refresh process that must be called at the end of every frame
    */
    void Refresh() override final { ACSbAllocater<StructBuffType>::RefreshSbAllocator(); }

    /** @brief Get the name of cbuffer defined in hlsl */
    const std::string& GetName() override final { return ACSbAllocater<StructBuffType>::m_hlslBufferName; }

private:
    /**
       @brief Internal function of 'GetAllocatedDataHandle'
       @param component Graphics component that wants to allocate data in the structured buffer
       @param meshIndex Index of mesh held by graphics component
       @return The handle that the graphics component allocates data in the structured buffer
    */
    template<class T>
        requires std::is_base_of_v<ACGraphicsComponent, T>
    Utl::Dx::CPU_DESCRIPTOR_HANDLE InnerGetAllocateDataHandle(T* component, UINT meshIndex);
};

// Release strutured buffers corresponding to the component sent
template<class StructBuffType>
void ACDynamicSbAllocatorPerMesh<StructBuffType>::ReleaseBuffer(ACGraphicsComponent* graphics) {
    // Get the number of all meshes
    UINT meshNum = graphics->GetMeshNum();
    // Release all meshes data
    for (UINT i = 0; i < meshNum; ++i) {
        ACSbAllocater<StructBuffType>::Release(graphics->GetMeshKey(i));
    }
}

// Internal function of 'GetAllocatedDataHandle'
template<class StructBuffType>
template<class T>
    requires std::is_base_of_v<ACGraphicsComponent, T>
Utl::Dx::CPU_DESCRIPTOR_HANDLE ACDynamicSbAllocatorPerMesh<StructBuffType>::InnerGetAllocateDataHandle(T* component, UINT meshIndex) {
    // Check the state of the mesh of the sent component
    MeshKey sentComponentKey = component->GetMeshKey(meshIndex);
    SbAllocatorKeyState flag = ACSbAllocater<StructBuffType>::CheckKey(sentComponentKey);
    switch (flag) {
        // If the key doesn't exist in the map, create new structured buffer
    case SbAllocatorKeyState::NotExist:
        this->CreateBuffer(component);
        break;
        // If key's data has been calculated, return its handle
    case SbAllocatorKeyState::Calculated:
        return ACSbAllocater<StructBuffType>::GetCPUHandle(sentComponentKey);
        break;
    default:
        break;
    }

    // Calculate structured buffer data, and return its handle
    return this->AllocateData(component, meshIndex);
}

#endif // !__DYNAMIC_SB_ALLOCATOR_H__
