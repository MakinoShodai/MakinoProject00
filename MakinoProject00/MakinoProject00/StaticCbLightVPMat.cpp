#include "StaticCbLightVPMat.h"
#include "Scene.h"
#include "StaticResourceRegistry.h"
#include "StaticCbDirLightParam.h"

// The number of frustum corner
const UINT FRUSTUM_CORNER_NUM = 8;
// Inverse of the number of frustum corner
const float INV_FRUSTUM_CORNER_NUM = 1.0f / (float)FRUSTUM_CORNER_NUM;
// 2 divided by pixel of shadow map
const float DIV2_SHADOWMAP_SIZE = 2.0f / (float)SHADOWMAP_SIZE;

// Value to split the first cascade
const float FIRST_CASCADE_SPLIT_VALUE = 0.05f;
// Value to split the second cascade
const float SECOND_CASCADE_END_Z = 0.1f;

// Constructor
CLightVPCalculator::CLightVPCalculator()
    : m_lightRegistry(nullptr)
    , m_lightVPs()
    , m_isCalculated(false)
    , m_cascadeSplitsPassShader{}
    , m_entireShadowMatrix()
    , m_cascadeOffsets{}
    , m_cascadeScales{}
{ 
    m_cascadeSplitVals[0] = FIRST_CASCADE_SPLIT_VALUE;
    m_cascadeSplitVals[1] = SECOND_CASCADE_END_Z;
    m_cascadeSplitVals[2] = 1.0f;
}

// Calculate all light view projection matrices and return it
const AllLightVP* CLightVPCalculator::Calculate() {
    if (m_isCalculated) {
        return &m_lightVPs;
    }

    CDirectionalLightComponent* dirLight = m_lightRegistry->GetDirectionalLight().Get();

    if (dirLight) {
        CCameraComponent* camera = dirLight->GetScene()->GetCameraRegistry()->GetCameraPriority().Get();
        if (camera) {
            // Get each property
            const Vector3f& lightDir = dirLight->GetTransform().GetForward();
            float cameraNear = camera->GetNear();
            float cameraFar = camera->GetFar();

            // Get inverse matrix of camera view projection matrix
            DirectX::XMMATRIX invCameraViewProj = DirectX::XMMatrixInverse(nullptr, camera->GenerateViewMatrix() * camera->GenerateProjectionMatrix());

            // Calculate corner positions of frustum in world space
            Vector3f entireFrustumCorners[FRUSTUM_CORNER_NUM] = {
                Vector3f(-1.0f,  1.0f, 0.0f),
                Vector3f(1.0f,  1.0f, 0.0f),
                Vector3f(1.0f, -1.0f, 0.0f),
                Vector3f(-1.0f, -1.0f, 0.0f),
                Vector3f(-1.0f,  1.0f, 1.0f),
                Vector3f(1.0f,  1.0f, 1.0f),
                Vector3f(1.0f, -1.0f, 1.0f),
                Vector3f(-1.0f, -1.0f, 1.0f),
            };
            for (UINT i = 0; i < FRUSTUM_CORNER_NUM; ++i) {
                // Convert positions coordinate space from projection space to world space
                entireFrustumCorners[i] = Utl::Math::ToVector3f(DirectX::XMVector3TransformCoord(Utl::Math::ToXMVECTOR(entireFrustumCorners[i], 1.0f), invCameraViewProj));
            }

            // Matrix for offsetting from [-1, 1] projection space to [0, 1] UV space
            DirectX::XMMATRIX toUVSpaceMatrix = 
                DirectX::XMMatrixScaling(0.5f, -0.5f, 1.0f) * 
                DirectX::XMMatrixTranslation(0.5f, 0.5f, 0.0f);

            // Calculate the shadow camera matrix for the entire frustum
            DirectX::XMMATRIX entireShadowMatrix;
            CalculateShadowMatrix(lightDir, entireFrustumCorners, toUVSpaceMatrix, &entireShadowMatrix);

            // Calculate the view projection matrix for each cascade map
            for (UINT cascadeIndex = 0; cascadeIndex < CASCADE_NUM; ++cascadeIndex) {
                // Get previous split value
                float prevSplitVal = (cascadeIndex == 0) ? 0.0f : m_cascadeSplitVals[cascadeIndex - 1];

                // Calculate the positions of the corners of the frustum of this cascade index
                Vector3f splitFrustumCorners[FRUSTUM_CORNER_NUM];
                for (UINT i = 0; i < 4; ++i) {
                    Vector3f vFarCorner = entireFrustumCorners[i + 4] - entireFrustumCorners[i];
                    splitFrustumCorners[i + 4] = entireFrustumCorners[i] + vFarCorner * m_cascadeSplitVals[cascadeIndex];
                    splitFrustumCorners[i]     = entireFrustumCorners[i] + vFarCorner * prevSplitVal;
                }

                // Compute center positions of the frustum of this cascade index
                Vector3f frustumCenter = Vector3f::Zero();
                for (UINT i = 0; i < FRUSTUM_CORNER_NUM; ++i) {
                    frustumCenter += splitFrustumCorners[i];
                }
                frustumCenter *= INV_FRUSTUM_CORNER_NUM;

                // Calculate a radius of a sphere containing the frustum of this cascade index
                float sphereRadius = 0.0f;
                for (UINT i = 0; i < 8; ++i) {
                    float distance = (splitFrustumCorners[i] - frustumCenter).Length();
                    sphereRadius = (std::max)(sphereRadius, distance);
                }
                // Round the radius to 1/16 accuracy
                sphereRadius = std::ceil(sphereRadius * 16.0f) / 16.0f;

                // #NOTE : Set the values to just fit the frustum, respectively
                // Create view matrix of shadow camera
                Vector3f shadowCameraPos = frustumCenter - lightDir * sphereRadius;
                DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(
                    Utl::Math::ToXMVECTOR(shadowCameraPos, 0.0f),
                    Utl::Math::ToXMVECTOR(frustumCenter, 0.0f),
                    Utl::Math::ToXMVECTOR(Utl::Math::UNIT3_UP, 0.0f)
                );
                // Create projection matrix of shadow camera
                DirectX::XMMATRIX proj = DirectX::XMMatrixOrthographicOffCenterLH(-sphereRadius, sphereRadius, -sphereRadius, sphereRadius, 0.0f, sphereRadius * 2.0f);

                // Create a rounding matrix to move in texel size increments to avoid flickering
                // Calculate the origin of the shadow camera's projection space
                DirectX::XMVECTOR shadowOrigin = DirectX::XMVector4Transform(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), view * proj);
                shadowOrigin = DirectX::XMVectorScale(shadowOrigin, (float)SHADOWMAP_SIZE * Utl::Inv::_2);

                // Perform rounding and calculate the difference
                DirectX::XMVECTOR roundedOrigin = DirectX::XMVectorRound(shadowOrigin);
                DirectX::XMVECTOR roundDiff = DirectX::XMVectorSubtract(roundedOrigin, shadowOrigin);
                roundDiff = DirectX::XMVectorScale(roundDiff, DIV2_SHADOWMAP_SIZE);
                roundDiff = DirectX::XMVectorSetZ(roundDiff, 0.0f);
                roundDiff = DirectX::XMVectorSetW(roundDiff, 0.0f);

                // Apply the difference to the projection matrix of shadow camera
                proj.r[3] = DirectX::XMVectorAdd(proj.r[3], roundDiff);

                // Copy the matrix to pass to the shader
                DirectX::XMMATRIX viewProj = view * proj;
                DirectX::XMStoreFloat4x4(&m_lightVPs.m_cascadeVPMatrices[cascadeIndex], viewProj);

                // Calculate the split value to be passed to the shader
                m_cascadeSplitsPassShader[cascadeIndex] = cameraNear + m_cascadeSplitVals[cascadeIndex] * (cameraFar - cameraNear);

                // Calculate offset and scale values for this cascade index
                // Convert matrix from [-1, 1] projection space to [0, 1] UV space and get the inverse matrix of it
                DirectX::XMMATRIX shadowMatrix = DirectX::XMMatrixMultiply(viewProj, toUVSpaceMatrix);
                DirectX::XMMATRIX invShadowMat = DirectX::XMMatrixInverse(nullptr, shadowMatrix);

                // Convert the position of the corner of this cascade index frustum 
                // to the position in the shadow camera projection space of the entire frustum
                DirectX::XMFLOAT3 cascadeCornerUVSpace;
                DirectX::XMStoreFloat3(&cascadeCornerUVSpace, 
                    DirectX::XMVector3TransformCoord(
                            DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), invShadowMat), 
                        entireShadowMatrix));

                // Convert also the vertex at the opposite position
                DirectX::XMFLOAT3 oppositeCornerUVSpace;
                DirectX::XMStoreFloat3(&oppositeCornerUVSpace, 
                    DirectX::XMVector3TransformCoord(
                        DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), invShadowMat), 
                    entireShadowMatrix));

                // Assigns the offset after applying the shadow matrix to the object's vertices
                m_cascadeOffsets[cascadeIndex] = Vector4f(-cascadeCornerUVSpace.x, -cascadeCornerUVSpace.y, -cascadeCornerUVSpace.z, 0.0f);

                // Calculate and assign a scale after applying the shadow matrix to the object's vertices
                Vector3f cascadeScale = Vector3f::Ones() / ((Utl::Math::ToVector3f(oppositeCornerUVSpace) - Utl::Math::ToVector3f(cascadeCornerUVSpace)));
                m_cascadeScales[cascadeIndex] = Vector4f(cascadeScale.x(), cascadeScale.y(), cascadeScale.z(), 1.0f);
            }

            m_isCalculated = true;
            return &m_lightVPs;
        }
    }

    return nullptr;
}

// Calculate the shadow camera view projection matrix of the entire frustum
void CLightVPCalculator::CalculateShadowMatrix(const Vector3f& lightDir, const Vector3f* frustumCorners, const DirectX::XMMATRIX& toUVSpaceMatrix, DirectX::XMMATRIX* entireShadowMatrix) {
    // Compute center position of frustum
    Vector3f frustumCenter = Vector3f::Zero();
    for (UINT i = 0; i < FRUSTUM_CORNER_NUM; ++i) {
        frustumCenter += frustumCorners[i];
    }
    frustumCenter *= INV_FRUSTUM_CORNER_NUM;

    // Get position of shadow camera
    Vector3f shadowCameraPos = frustumCenter - lightDir;

    // Create view matrix of shadow camera
    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(
        Utl::Math::ToXMVECTOR(shadowCameraPos, 0.0f),
        Utl::Math::ToXMVECTOR(frustumCenter, 0.0f),
        Utl::Math::ToXMVECTOR(Utl::Math::UNIT3_UP, 0.0f)
    );
    // Create projection matrix of shadow camera
    DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicOffCenterLH(-0.5f, 0.5f, -0.5f, 0.5f, 0.0f, 1.0f);

    // Create a view projection matrix offset to UV space in [0, 1]
    *entireShadowMatrix = (view * projection) * toUVSpaceMatrix;
    DirectX::XMStoreFloat4x4(&m_entireShadowMatrix, *entireShadowMatrix);
}

// Constructor
CStaticCbLightVP::CStaticCbLightVP() 
    : ACStaticCbAllocator("LightViewProjMat", CASCADE_NUM)
    , m_lightVPCaculator(CSharedPtr<CLightVPCalculator>::Make())
    , m_cascadesIndex(0) 
    , m_isAdvance(true)
{
    auto dirLightAllocator = CStaticResourceRegistry::GetAny().GetStaticResource<CStaticCbDirLightParam>();
    if (dirLightAllocator == nullptr) {
        throw Utl::Error::CFatalError(L"Must have created CStaticCbDirLightParam before creating CStaticCbLightVP");
    }
    else {
        dirLightAllocator->SetLightVPCalculater(m_lightVPCaculator);
    }
}

// Scene start processing
void CStaticCbLightVP::Start(CScene* scene) {
    m_lightVPCaculator->SetLightRegistry(scene->GetLightRegistry());
}

// Advance to the next cascade shadow map
void CStaticCbLightVP::Advance() {
    m_cascadesIndex++;
    m_isAdvance = true;
}

// Allocate data
Utl::Dx::CPU_DESCRIPTOR_HANDLE CStaticCbLightVP::AllocateData() {
    const AllLightVP* allLightVP = m_lightVPCaculator->Calculate();

    m_isAdvance = false;
    if (allLightVP != nullptr) {
        return DirectDataCopy(&allLightVP->m_cascadeVPMatrices[m_cascadesIndex]);
    }

    throw Utl::Error::CStopDrawingSceneError(L"Warning! There is no directional light or camera in this scene.\n");
}

// Refresh process
void CStaticCbLightVP::RefreshDerived() {
    m_lightVPCaculator->ResetCalculated();
    m_cascadesIndex = 0;
    m_isAdvance = true;
}


