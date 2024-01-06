#define STATIC(name) CONNECT_TOKEN(name, _S)
#define DYNAMIC(name) CONNECT_TOKEN(name, _D)

/** @brief Connect tokens to tokens */
#define CONNECT_TOKEN(a, b) INNER_CONNECT_TOKEN(a, b)
/** @brief Connect tokens to tokens (Inner) */
#define INNER_CONNECT_TOKEN(a, b) a##b

#ifdef CB_REGISTER_WORLD_MAT
cbuffer DYNAMIC(WorldMat) : register(CB_REGISTER_WORLD_MAT) {
    struct {
        float4x4 world;
    } worldMat;
}
#endif // CB_REGISTER_WORLD_MAT

#ifdef CB_REGISTER_VIEW_PROJ_MAT
cbuffer STATIC(ViewProjMat) : register(CB_REGISTER_VIEW_PROJ_MAT) {
    struct {
        float4x4 viewProj;
    } viewProjMat;
}
#endif // CB_REGISTER_VIEW_PROJ_MAT

#ifdef CB_REGISTER_ORTHOGRAPHIC_PROJ_MAT
cbuffer STATIC(OrthographicProjMat) : register(CB_REGISTER_ORTHOGRAPHIC_PROJ_MAT) {
    struct {
        float4x4 proj;
    } orthographicProjMat;
}
#endif // CB_REGISTER_ORTHOGRAPHIC_PROJ_MAT

#ifdef SB_REGISTER_ANIM_MAT
StructuredBuffer<float4x4> DYNAMIC(animMatrices) : register(SB_REGISTER_ANIM_MAT);
#endif // SB_REGISTER_ANIM_MAT

#ifdef CB_REGISTER_COLOR_REGISTER
cbuffer DYNAMIC(Color) : register(CB_REGISTER_COLOR_REGISTER) {
    struct {
        float4 color;
    } changeColor;
}
#endif // CB_REGISTER_COLOR_REGISTER

#ifdef TEX_REGISTER_MAIN_TEX
Texture2D<float4> DYNAMIC(mainTex) : register(TEX_REGISTER_MAIN_TEX);
#endif // TEX_REGISTER_MAIN_TEX

#ifdef SMP_REGISTER_MAIN_SMP
SamplerState mainSmp : register(SMP_REGISTER_MAIN_SMP);
#endif // SMP_REGISTER_MAIN_SMP
