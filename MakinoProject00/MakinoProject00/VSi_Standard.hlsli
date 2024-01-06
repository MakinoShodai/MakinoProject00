// World matrix
#define CB_REGISTER_WORLD_MAT b0

// View projection matrix
#ifndef _2D
#define CB_REGISTER_VIEW_PROJ_MAT b1
#else
#define CB_REGISTER_ORTHOGRAPHIC_PROJ_MAT b1
#endif // !_2D

// Animation matrix
#ifdef _ANIM
#define SB_REGISTER_ANIM_MAT t0
#endif // _ANIM

#include "CommonResources.hlsli"

struct INPUT_STANDARD {
    float3 pos : POSITION0;
    float2 uv : TEXCOORD0;
#ifndef _SPRITE
    float3 normal : NORMAL0;
#endif // !_SPRITE
    
#ifdef _ANIM
    float4 weight : WEIGHT;
    uint4 index : INDEX;
#endif // _ANIM
};

struct RASTER_STANDARD {
    float4 pos : SV_POSITION0;
    float2 uv : TEXCOORD0;
#ifndef _SPRITE
    float3 normal : NORMAL0;
#endif // !_SPRITE
};

RASTER_STANDARD main(INPUT_STANDARD input) {
    RASTER_STANDARD output;
    
    output.pos = float4(input.pos, 1.0f);
    
#ifdef _ANIM
    float4x4 animMat =
        DYNAMIC(animMatrices)[input.index.x] * input.weight.x +
        DYNAMIC(animMatrices)[input.index.y] * input.weight.y +
        DYNAMIC(animMatrices)[input.index.z] * input.weight.z +
        DYNAMIC(animMatrices)[input.index.w] * input.weight.w;
    output.pos = mul(animMat, output.pos);
#endif // _ANIM
    
    output.pos = mul(worldMat.world, output.pos);
#ifndef _2D
    output.pos = mul(viewProjMat.viewProj, output.pos);
#else
    output.pos = mul(orthographicProjMat.proj, output.pos);
#endif // !_2D
    output.uv = input.uv;

#ifndef _SPRITE
    output.normal = mul((float3x3)worldMat.world, input.normal);
#endif // !_SPRITE
    
    return output;
}
