// World matrix
#define CB_REGISTER_WORLD_MAT b0
// Light view projection matrix
#define CB_REGISTER_LIGHT_VIEW_PROJ_MAT b1

// Animation matrix
#ifdef _ANIM
#define SB_REGISTER_ANIM_MAT t0
#endif // _ANIM

#include "CommonResources.hlsli"

struct INPUT_STANDARD {
    float3 pos : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    
#ifdef _ANIM
    float4 weight : WEIGHT;
    uint4 index : INDEX;
#endif // _ANIM
};

struct RASTER_STANDARD {
    float4 pos : SV_POSITION0;
};

RASTER_STANDARD main(INPUT_STANDARD input) {
    RASTER_STANDARD output;
    
    // Convert float3 to float4
    output.pos = float4(input.pos, 1.0f);
    
    // Apply animation matrices
#ifdef _ANIM
    float4x4 animMat =
        DYNAMIC(animMatrices)[input.index.x] * input.weight.x +
        DYNAMIC(animMatrices)[input.index.y] * input.weight.y +
        DYNAMIC(animMatrices)[input.index.z] * input.weight.z +
        DYNAMIC(animMatrices)[input.index.w] * input.weight.w;
    output.pos = mul(animMat, output.pos);
#endif // _ANIM
    
    // Apply world matrix
    output.pos = mul(worldMat.world, output.pos);
    
    // Apply light view projection matrix
    output.pos = mul(lightViewProjMat.viewProj, output.pos);
    
    return output;
}
