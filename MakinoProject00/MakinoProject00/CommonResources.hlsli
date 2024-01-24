#define STATIC(name) CONNECT_TOKEN(name, _S)
#define DYNAMIC(name) CONNECT_TOKEN(name, _D)

/** @brief Connect tokens to tokens */
#define CONNECT_TOKEN(a, b) INNER_CONNECT_TOKEN(a, b)
/** @brief Connect tokens to tokens (Inner) */
#define INNER_CONNECT_TOKEN(a, b) a##b

// The number of cascade
static const uint CASCADE_NUM = 3;

// Parameter for point lights
struct PointLightParam {
    float4 color; // Light color
    float4 posWS; // Position (world coordinate space)
    float4 attenuation; // x = constant attenuation, y = linear attenuation, z = quadratic attenuation
    float intensity; // Intensity of this light
};

// World matrix
#ifdef CB_REGISTER_WORLD_MAT
cbuffer DYNAMIC(WorldMat) : register(CB_REGISTER_WORLD_MAT) {
    struct {
        float4x4 world;
    } worldMat;
}
#endif // CB_REGISTER_WORLD_MAT

// View projection matrix
#ifdef CB_REGISTER_VIEW_PROJ_MAT
cbuffer STATIC(ViewProjMat) : register(CB_REGISTER_VIEW_PROJ_MAT) {
    struct {
        float4x4 viewProj;
    } viewProjMat;
}
#endif // CB_REGISTER_VIEW_PROJ_MAT

// Orthographic projection matrix
#ifdef CB_REGISTER_ORTHOGRAPHIC_PROJ_MAT
cbuffer STATIC(OrthographicProjMat) : register(CB_REGISTER_ORTHOGRAPHIC_PROJ_MAT) {
    struct {
        float4x4 proj;
    } orthographicProjMat;
}
#endif // CB_REGISTER_ORTHOGRAPHIC_PROJ_MAT

// Light view projection matrix
#ifdef CB_REGISTER_LIGHT_VIEW_PROJ_MAT
cbuffer STATIC(LightViewProjMat) : register(CB_REGISTER_LIGHT_VIEW_PROJ_MAT) {
    struct {
        float4x4 viewProj;
    } lightViewProjMat;
}
#endif // CB_REGISTER_LIGHT_VIEW_PROJ_MAT

// Color parameter
#ifdef CB_REGISTER_COLOR_REGISTER
cbuffer DYNAMIC(Color) : register(CB_REGISTER_COLOR_REGISTER) {
    struct {
        float4 color;
    } changeColor;
}
#endif // CB_REGISTER_COLOR_REGISTER

// Material parameter
#ifdef CB_REGISTER_MATERIAL_REGISTER
cbuffer DYNAMIC(Material) : register(CB_REGISTER_MATERIAL_REGISTER) {
    struct {
        float4 color;
        float2 shininess;
    } material;
}
#endif // CB_REGISTER_MATERIAL_REGISTER

// For directional light parameter
#ifdef CB_REGISTER_DIRLIGHT_REGISTER
cbuffer STATIC(DirLightParam) : register(CB_REGISTER_DIRLIGHT_REGISTER) {
    struct {
        float4   color;                     // Color of this light
        float4   ambientColor;              // Ambient color
        float4   lightDir;                  // Light direction (unit vector)
        float4   cameraPos;                 // Position of camera in the world coordinate space
        float4   cascadeEndZ;               // Clip position Z of each cascade transformed by the view projection matrix of the camera
        float4x4 entireShadowVPMatrix;      // Shadow camera view projection matrix of the entire cascade
        float4 cascadeOffsets[CASCADE_NUM]; // Offsets after applying the shadow matrix to the object's vertices for each cascades
        float4 cascadeScales[CASCADE_NUM];  // Scales after applying the shadow matrix to the object's vertices for each cascades
        float intensity;                    // Intensity of this light
        uint  pointLightsNum;               // The number of point lights
    } dirLightParam;
}
#endif // CB_REGISTER_DIRLIGHT_REGISTER



// Animation matrix
#ifdef SB_REGISTER_ANIM_MAT
StructuredBuffer<float4x4> DYNAMIC(animMatrices) : register(SB_REGISTER_ANIM_MAT);
#endif // SB_REGISTER_ANIM_MAT

// Point lights
#ifdef SB_REGISTER_POINT_LIGHT
StructuredBuffer<PointLightParam> STATIC(pointLights) : register(SB_REGISTER_POINT_LIGHT);
#endif // SB_REGISTER_POINT_LIGHT


// Main texture
#ifdef TEX_REGISTER_MAIN_TEX
Texture2D<float4> DYNAMIC(mainTex) : register(TEX_REGISTER_MAIN_TEX);
#endif // TEX_REGISTER_MAIN_TEX

// Shadowmap texture
#ifdef TEX_REGISTER_SHADOWMAP_TEX
Texture2D<float4> STATIC(shadowMapTex)[CASCADE_NUM] : register(TEX_REGISTER_SHADOWMAP_TEX);
#endif // TEX_REGISTER_SHADOWMAP_TEX



// Main sampler
#ifdef SMP_REGISTER_MAIN_SMP
SamplerState mainSmp : register(SMP_REGISTER_MAIN_SMP);
#endif // SMP_REGISTER_MAIN_SMP

// Main sampler
#ifdef SMP_REGISTER_SHADOWMAP_SMP
SamplerState shadowMapSmp : register(SMP_REGISTER_SHADOWMAP_SMP);
#endif // SMP_REGISTER_SHADOWMAP_SMP
