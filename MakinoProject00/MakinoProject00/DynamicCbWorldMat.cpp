#include "DynamicCbWorldMat.h"
#include "GraphicsComponent.h"
#include "GameObject.h"
#include "RenderTarget.h"
#include "Scene.h"

// Sprite coordinate system magnification
float SPRITE_COORDINATE_SYSTEM_MAGNIFICATION = 1.0f / SPRITE_SCREEN_BOUNDARY;

// Calculate sprite scale according to resolution
// @param sprite Sprite component
// @param transform Transform of sprite component
// @param scale Return variable for scale
// @param pos Return variable for position
void CalculateSpriteScale(ACSprite* sprite, const Transformf& transform, Vector2f* scale, Vector2f* pos) {
    // Get width and height, of render target view
    float halfRtvWidth;
    float halfRtvHeight;
    ACRenderTarget* rtv = ACRenderTarget::GetCurrentAppliedRenderTarget();
    if (rtv != nullptr) {
        halfRtvWidth = rtv->GetWidth() * Utl::Inv::_2;
        halfRtvHeight = rtv->GetHeight() * Utl::Inv::_2;
    }
    else {
        OutputDebugString(L"Warning! Couldn't get the render target when calculating the world matrix of the image.\n");
        halfRtvWidth = 1920.0f * Utl::Inv::_2;
        halfRtvHeight = 1080.0f * Utl::Inv::_2;
    }

    // Calculate scale X and scale Y
    const Vector2f& wndSizeMag = CApplication::GetAny().GetWndSizeMagnification();
    scale->x() = (float)sprite->GetWidth() * transform.scale.x() * wndSizeMag.x();
    scale->y() = (float)sprite->GetHeight() * transform.scale.y() * wndSizeMag.y();

    // Correct the position to the sprite coordinate system
    if (pos != nullptr) {
        pos->x() = transform.pos.x() * SPRITE_COORDINATE_SYSTEM_MAGNIFICATION * halfRtvWidth;
        pos->y() = transform.pos.y() * SPRITE_COORDINATE_SYSTEM_MAGNIFICATION * halfRtvHeight;
    }
}

// World matrix
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicCbWorldMat::AllocateData(ACGraphicsComponent* component) {
    // Get the transform of the game object and Set its quaternion to a XMVECTORF32
    const Transformf& transform = component->GetGameObj()->GetTransform();
    DirectX::XMVECTORF32 quaternion = { transform.rotation.x(), transform.rotation.y(), transform.rotation.z(), transform.rotation.w() };

    // Calculate a world matrix
    DirectX::XMFLOAT4X4 mat;
    DirectX::XMStoreFloat4x4(&mat,
        DirectX::XMMatrixScaling(transform.scale.x(), transform.scale.y(), transform.scale.z()) *
        DirectX::XMMatrixRotationQuaternion(quaternion) *
        DirectX::XMMatrixTranslation(transform.pos.x(), transform.pos.y(), transform.pos.z())
    );

    return DirectDataCopy(&mat);
}

// World matrix for sprite
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicCbWorldMat::AllocateData(CSpriteUI* sprite) {
    // Get the transform of the game object and Set its quaternion to a XMVECTORF32
    const Transformf& transform = sprite->GetGameObj()->GetTransform();
    DirectX::XMVECTORF32 quaternion = { transform.rotation.x(), transform.rotation.y(), transform.rotation.z(), transform.rotation.w() };

    // Calculate sprite scale and position according to resolution
    Vector2f scale;
    Vector2f pos;
    CalculateSpriteScale(sprite, transform, &scale, &pos);

    // Calculate a world matrix
    DirectX::XMFLOAT4X4 mat;
    DirectX::XMStoreFloat4x4(&mat,
        DirectX::XMMatrixScaling(scale.x(), scale.y(), transform.scale.z()) *
        DirectX::XMMatrixRotationQuaternion(quaternion) *
        DirectX::XMMatrixTranslation(pos.x(), pos.y(), transform.pos.z())
    );

    return DirectDataCopy(&mat);
}

// Allocate data for sprite 3D
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicCbWorldMat::AllocateData(CSprite3D* sprite) {
    // Get the transform of the game object and Set its quaternion to a XMVECTORF32
    const Transformf& transform = sprite->GetGameObj()->GetTransform();
    DirectX::XMVECTORF32 quaternion = { transform.rotation.x(), transform.rotation.y(), transform.rotation.z(), transform.rotation.w() };

    // Calculate sprite scale according to resolution
    float scaleX = transform.scale.x() * sprite->GetResolutionRatio();

    // Calculate a world matrix
    DirectX::XMFLOAT4X4 mat;
    DirectX::XMStoreFloat4x4(&mat,
        DirectX::XMMatrixScaling(scaleX, transform.scale.y(), transform.scale.z()) *
        DirectX::XMMatrixRotationQuaternion(quaternion) *
        DirectX::XMMatrixTranslation(transform.pos.x(), transform.pos.y(), transform.pos.z())
    );

    return DirectDataCopy(&mat);
}

// Allocate data for billboard
Utl::Dx::CPU_DESCRIPTOR_HANDLE CDynamicCbWorldMat::AllocateData(CBillboard* billboard) {
    auto camera = billboard->GetGameObj()->GetScene()->GetCameraRegistry()->GetCameraPriority();
    if (camera) {
        // Get the transform of the game object
        const Transformf& transform = billboard->GetGameObj()->GetTransform();

        // Calculate sprite scale according to resolution
        float scaleX = transform.scale.x() * billboard->GetResolutionRatio();

        // Calculate a world matrix
        DirectX::XMFLOAT4X4 mat;
        DirectX::XMStoreFloat4x4(&mat,
            DirectX::XMMatrixScaling(scaleX, transform.scale.y(), transform.scale.z()) *
            camera->GetRotationMatrix() *
            DirectX::XMMatrixTranslation(transform.pos.x(), transform.pos.y(), transform.pos.z())
        );

        return DirectDataCopy(&mat);
    }

    OutputDebugString(L"Warning! Couldn't get the camera from the scene.\n");
    return Utl::Dx::CPU_DESCRIPTOR_HANDLE();
}
