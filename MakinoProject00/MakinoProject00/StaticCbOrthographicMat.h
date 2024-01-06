/**
 * @file   StaticCbOrthographicMat.h
 * @brief  This file handles the static CB of the orthographic projection matrix.
 * 
 * @author Shodai Makino
 * @date   2023/12/9
 */

#ifndef __STATIC_CB_ORTHOGRAPHIC_MAT_H__
#define __STATIC_CB_ORTHOGRAPHIC_MAT_H__

#include "StaticCbAllocator.h"
#include "RenderTarget.h"

 /** @brief Static CB class that handles orthographic projection matrix */
class CStaticCbOrthographicMat : public ACStaticCbAllocator<DirectX::XMFLOAT4X4> {
public:
    /** @brief Constructor */
    CStaticCbOrthographicMat() : ACStaticCbAllocator("OrthographicProjMat", 5), m_prevRenderTarget(nullptr) {}

protected:
    /** @brief Allocate data */
    Utl::Dx::CPU_DESCRIPTOR_HANDLE AllocateData() override;

    /** @brief Advance to the next data assignment? */
    bool IsAdvanceNextAllocate() override;

    /** @brief Refresh process that must be called at the end of every frame in derived class */
    void RefreshDerived() override { m_prevRenderTarget = nullptr; }

private:
    /** @brief previous used render target */
    ACRenderTarget* m_prevRenderTarget;
};

#endif // !__STATIC_CB_ORTHOGRAPHIC_MAT_H__
