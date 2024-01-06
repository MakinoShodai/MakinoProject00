/**
 * @file   DynamicResource.h
 * @brief  This file handles interface of dynamic resource that need to be allocated per object.
 * 
 * @author Shodai Makino
 * @date   2023/12/17
 */

#ifndef __DYNAMIC_RESOURCE_H__
#define __DYNAMIC_RESOURCE_H__

#include "UtilityForDirectX.h"
#include "GraphicsComponent.h"
#include "Model.h"
#include "Sprite.h"
#include "Shape.h"

 /** @brief Interface for dynamic resource that need to be allocated per object */
class IDynamicResource {
public:
    /**
       @brief Destructor
    */
    virtual ~IDynamicResource() = default;

    /**
       @brief Get handle of allocated data in constant buffer
       @param model Model that wants to allocate data in the constant buffer
       @param meshIndex Index of mesh held by model
       @return The handle that the model allocates data in the constant buffer
    */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CBasicModel* model, UINT meshIndex) = 0;
    /** @brief Get handle of allocated data in constant buffer */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CSkeletalModel* model, UINT meshIndex) = 0;
    /** @brief Get handle of allocated data in constant buffer */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CSpriteUI* sprite, UINT meshIndex) = 0;
    /** @brief Get handle of allocated data in constant buffer */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CSprite3D* sprite, UINT meshIndex) = 0;
    /** @brief Get handle of allocated data in constant buffer */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CBillboard* sprite, UINT meshIndex) = 0;
    /** @brief Get handle of allocated data in constant buffer */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CTexShape* shape, UINT meshIndex) = 0;
    /** @brief Get handle of allocated data in constant buffer */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE GetAllocatedDataHandle(CColorOnlyShape* shape, UINT meshIndex) = 0;

    /**
       @brief Refresh process that must be called at the end of every frame
    */
    virtual void Refresh() = 0;

    /** @brief Get the name of cbuffer defined in hlsl */
    virtual const std::string& GetName() = 0;

protected:
    /**
       @brief Allocate data calculated for a mesh held by sent graphics component in the constant buffer
       @param graphics component that wants to calculate data
       @param meshIndex Index of mesh held by graphics component
       @return The handle of allocated data in the constant buffer
    */
    virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACGraphicsComponent* graphics, UINT meshIndex) = 0;
    /** @brief Allocate data calculated for a mesh held by sent abstract model class in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACModel* model, UINT meshIndex) { return AllocateData(static_cast<ACGraphicsComponent*>(model), meshIndex); }
    /** @brief Allocate data calculated for a mesh held by sent non-animated model in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CBasicModel* model, UINT meshIndex) { return AllocateData(static_cast<ACModel*>(model), meshIndex); }
    /** @brief Allocate data calculated for a mesh held by sent skeletal model in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CSkeletalModel* model, UINT meshIndex) { return AllocateData(static_cast<ACModel*>(model), meshIndex); }
    /** @brief Allocate data calculated for a mesh held by sent abstract sprite class in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(ACSprite* sprite, UINT meshIndex) { return AllocateData(static_cast<ACGraphicsComponent*>(sprite), meshIndex); }
    /** @brief Allocate data calculated for a mesh held by sent sprite UI in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CSpriteUI* sprite, UINT meshIndex) { return AllocateData(static_cast<ACSprite*>(sprite), meshIndex); }
    /** @brief Allocate data calculated for a mesh held by sent sprite 3D in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CSprite3D* sprite, UINT meshIndex) { return AllocateData(static_cast<ACSprite*>(sprite), meshIndex); }
    /** @brief Allocate data calculated for a mesh held by sent billboard in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CBillboard* sprite, UINT meshIndex) { return AllocateData(static_cast<ACSprite*>(sprite), meshIndex); }
    /** @brief Allocate data calculated for a mesh held by sent texture shape in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CTexShape* shape, UINT meshIndex) { return AllocateData(static_cast<ACGraphicsComponent*>(shape), meshIndex); }
    /** @brief Allocate data calculated for a mesh held by sent color only shape in the constant buffer */
    inline virtual Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData(CColorOnlyShape* shape, UINT meshIndex) { return AllocateData(static_cast<ACGraphicsComponent*>(shape), meshIndex); }
};

#endif // !__DYNAMIC_RESOURCE_H__
