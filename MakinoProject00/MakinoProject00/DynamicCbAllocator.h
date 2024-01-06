/**
 * @file   DynamicCbAllocator.h
 * @brief  This file handles class that allocates data for constant buffer per object.
 * 
 * @author Shodai Makino
 * @date   2023/12/9
 */

#ifndef __DYNAMIC_CB_ALLOCATOR_H__
#define __DYNAMIC_CB_ALLOCATOR_H__

#include "AbstractCbAllocator.h"
#include "DynamicResource.h"

/**
   @brief This class allocates data for constant buffer view per object
   @tparam ConstBuffType type of constant buffer view
   @details
   The following must be done in the derived class.
   Only 'UINT maxUsePerFrame' should be the argument of the constructor of the derived class.
*/
template<class ConstBuffType>
class ACDynamicCbAllocatorPerMesh : public IDynamicResource, public ACCbAllocator<ConstBuffType> {
public:
    /**
       @brief Constructor
       @param maxUsePerFrame The maximum number of buffers that can be used in one frame
       @param hlslBufferName The name of cbuffer defined in hlsl
    */
    ACDynamicCbAllocatorPerMesh(UINT maxUsePerFrame, std::string hlslBufferName)
        : ACCbAllocator<ConstBuffType>(maxUsePerFrame, std::move(hlslBufferName))
    {}

    /**
       @brief Destructor
    */
    virtual ~ACDynamicCbAllocatorPerMesh() = default;

    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CBasicModel* model, UINT meshIndex) override { return InnerGetAllocateDataHandle(model, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CSkeletalModel* model, UINT meshIndex) override { return InnerGetAllocateDataHandle(model, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CSpriteUI* sprite, UINT meshIndex) override { return InnerGetAllocateDataHandle(sprite, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CSprite3D* sprite, UINT meshIndex) override { return InnerGetAllocateDataHandle(sprite, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CBillboard* sprite, UINT meshIndex) override { return InnerGetAllocateDataHandle(sprite, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CTexShape* shape, UINT meshIndex) override { return InnerGetAllocateDataHandle(shape, meshIndex); }
    /** @brief Get handle of allocated data in structured buffer */
    inline Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CColorOnlyShape* shape, UINT meshIndex) override { return InnerGetAllocateDataHandle(shape, meshIndex); }

    /**
       @brief Refresh process that must be called at the end of every frame
    */
    void Refresh() override final;

    /** @brief Get the name of cbuffer defined in hlsl */
    const std::string& GetName() override final { return ACCbAllocator<ConstBuffType>::m_hlslBufferName; }

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

protected:
    /** @brief Map to connect handles of calculated data, and meshes held by graphics components that calculated it */
    std::unordered_map<MeshKey, Utl::Dx::CPU_DESCRIPTOR_HANDLE> m_cachedHandleMap;
};

/**
   @brief This class allocates data for constant buffer view per object
   @tparam ConstBuffType type of constant buffer view
   @details
   The following must be done in the derived class.
   Only 'UINT maxUsePerFrame' should be the argument of the constructor of the derived class.
*/
template<class ConstBuffType>
class ACDynamicCbAllocator : public ACDynamicCbAllocatorPerMesh<ConstBuffType> {
public:
    /**
       @brief Constructor
       @param maxUsePerFrame The maximum number of buffers that can be used in one frame
       @param hlslBufferName The name of cbuffer defined in hlsl
    */
    ACDynamicCbAllocator(UINT maxUsePerFrame, std::string hlslBufferName)
        : ACDynamicCbAllocatorPerMesh<ConstBuffType>(maxUsePerFrame, std::move(hlslBufferName))
    {}

    /**
       @brief Destructor
    */
    virtual ~ACDynamicCbAllocator() = default;

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

protected:
    /**
       @brief Allocate data calculated for sent graphics component in the constant buffer
       @param graphics component that wants to calculate data
       @param meshIndex Index of mesh held by graphics component
       @return The handle of allocated data in the constant buffer
    */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACGraphicsComponent* graphics, UINT meshIndex) override final { return AllocateData(graphics); }
    /** @brief Allocate data calculated for sent model in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACModel* model, UINT meshIndex) override final { return AllocateData(model); }
    /** @brief Allocate data calculated for sent model in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CBasicModel* model, UINT meshIndex) override final { return AllocateData(model); }
    /** @brief Allocate data calculated for sent model in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CSkeletalModel* model, UINT meshIndex) override final { return AllocateData(model); }
    /** @brief Allocate data calculated for sent abstract sprite class in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACSprite* sprite, UINT meshIndex) override final { return AllocateData(sprite); }
    /** @brief Allocate data calculated for sent sprite UI in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CSpriteUI* sprite, UINT meshIndex) override final { return AllocateData(sprite); }
    /** @brief Allocate data calculated for sent sprite 3D in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CSprite3D* sprite, UINT meshIndex) override final { return AllocateData(sprite); }
    /** @brief Allocate data calculated for sent billboard in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CBillboard* sprite, UINT meshIndex) override final { return AllocateData(sprite); }
    /** @brief Allocate data calculated for sent texture shape in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CTexShape* shape, UINT meshIndex) override final { return AllocateData(shape); }
    /** @brief Allocate data calculated for sent color only shape in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CColorOnlyShape* shape, UINT meshIndex) override final { return AllocateData(shape); }

    /**
       @brief Allocate data calculated for sent graphics component in the constant buffer
       @param graphics component that wants to calculate data
       @return The handle of allocated data in the constant buffer
    */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACGraphicsComponent* graphics) = 0;
    /** @brief Allocate data calculated for sent model in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACModel* model) { return AllocateData(static_cast<ACGraphicsComponent*>(model)); }
    /** @brief Allocate data calculated for sent model in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CBasicModel* model) { return AllocateData(static_cast<ACModel*>(model)); }
    /** @brief Allocate data calculated for sent model in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CSkeletalModel* model) { return AllocateData(static_cast<ACModel*>(model)); }
    /** @brief Allocate data calculated for sent abstract sprite class in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACSprite* sprite) { return AllocateData(static_cast<ACGraphicsComponent*>(sprite)); }
    /** @brief Allocate data calculated for sent sprite UI in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CSpriteUI* sprite) { return AllocateData(static_cast<ACSprite*>(sprite)); }
    /** @brief Allocate data calculated for sent sprite 3D in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CSprite3D* sprite) { return AllocateData(static_cast<ACSprite*>(sprite)); }
    /** @brief Allocate data calculated for sent billboard in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CBillboard* sprite) { return AllocateData(static_cast<ACSprite*>(sprite)); }
    /** @brief Allocate data calculated for sent texture shape in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CTexShape* shape) { return AllocateData(static_cast<ACGraphicsComponent*>(shape)); }
    /** @brief Allocate data calculated for sent color only shape in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CColorOnlyShape* shape) { return AllocateData(static_cast<ACGraphicsComponent*>(shape)); }

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

// Refresh process that must be called at the end of every frame in this class
template<class ConstBuffType>
void ACDynamicCbAllocatorPerMesh<ConstBuffType>::Refresh() {
    // Refresh ACCbAllocator
    ACCbAllocator<ConstBuffType>::RefreshCbAllocator();

    // ClearMap
    m_cachedHandleMap.clear();
}

// Internal function of 'GetAllocatedDataHandle'
template<class ConstBuffType>
template<class T>
    requires std::is_base_of_v<ACGraphicsComponent, T>
Utl::Dx::CPU_DESCRIPTOR_HANDLE ACDynamicCbAllocatorPerMesh<ConstBuffType>::InnerGetAllocateDataHandle(T* component, UINT meshIndex) {
    // If the cache already has calculated data, return it
    MeshKey sentComponentKey = component->GetMeshKey(meshIndex);
    auto it = m_cachedHandleMap.find(sentComponentKey);
    if (it != m_cachedHandleMap.end()) {
        return m_cachedHandleMap[sentComponentKey];
    }

    // Calculate constant buffer data, and get its handle
    Utl::Dx::CPU_DESCRIPTOR_HANDLE handle = AllocateData(component, meshIndex);

    // Set the handle to the cache, and return it
    m_cachedHandleMap.emplace(sentComponentKey, handle);
    return handle;
}

// Internal function of 'GetAllocatedDataHandle'
template<class ConstBuffType>
template<class T>
    requires std::is_base_of_v<ACGraphicsComponent, T>
Utl::Dx::CPU_DESCRIPTOR_HANDLE ACDynamicCbAllocator<ConstBuffType>::InnerGetAllocateDataHandle(T* component, UINT meshIndex) {
    // #NOTE : Don't separate the process for each mesh held by the sent component
    // If the cache already has calculated data, return it
    MeshKey sentComponentKey = component->GetMeshKey(0);
    auto it = ACDynamicCbAllocatorPerMesh<ConstBuffType>::m_cachedHandleMap.find(sentComponentKey);
    if (it != ACDynamicCbAllocatorPerMesh<ConstBuffType>::m_cachedHandleMap.end()) {
        return ACDynamicCbAllocatorPerMesh<ConstBuffType>::m_cachedHandleMap[sentComponentKey];
    }

    // Calculate constant buffer data, and get its handle
    Utl::Dx::CPU_DESCRIPTOR_HANDLE handle = AllocateData(component, 0);

    // Set the handle to the cache, and return it
    ACDynamicCbAllocatorPerMesh<ConstBuffType>::m_cachedHandleMap.emplace(sentComponentKey, handle);
    return handle;
}

#endif // !__DYNAMIC_CB_ALLOCATOR_H__
