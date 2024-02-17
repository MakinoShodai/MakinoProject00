/**
 * @file   StaticCbVPMat.h
 * @brief  This file handles the static CB allocater of the camera's view projection matrix.
 * 
 * @author Shodai Makino
 * @date   2023/12/6
 */

#ifndef __STATIC_CB_VPMAT_H__
#define __STATIC_CB_VPMAT_H__

#include "StaticCbAllocator.h"
#include "CameraRegistry.h"

/** @brief Static CB allocater class that handles view projection matrix */
class CStaticCbVP : public ACStaticCbAllocator<DirectX::XMFLOAT4X4> {
public:
    /** @brief Constructor */
    CStaticCbVP() : ACStaticCbAllocator("ViewProjMat") {}

    /** @brief Scene start processing */
    void Start(CScene* scene) override;

protected:
    /** @brief Allocate data */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData() override;

private:
    /** @brief Camera registry */
    CWeakPtr<CCameraRegistry> m_cameraRegistry;
};

/** @brief Static CB allocater class that handles remove a position view projection matrix */
class CStaticCbRemovePosVP : public ACStaticCbAllocator<DirectX::XMFLOAT4X4> {
public:
    /** @brief Constructor */
    CStaticCbRemovePosVP() : ACStaticCbAllocator("RemovePosViewProjMat") {}

    /** @brief Scene start processing */
    void Start(CScene* scene) override;

protected:
    /** @brief Allocate data */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData() override;

private:
    /** @brief Camera registry */
    CWeakPtr<CCameraRegistry> m_cameraRegistry;
};

#endif // !__STATIC_CB_VPMAT_H__
