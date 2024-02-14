// World matrix
#define CB_REGISTER_WORLD_MAT b0

// View projection matrix
#ifndef REMOVE_POS_FROM_VIEW

#ifndef _2D
#define CB_REGISTER_VIEW_PROJ_MAT b1
#else
#define CB_REGISTER_ORTHOGRAPHIC_PROJ_MAT b1
#endif // !_2D

#else
#define CB_REGISTER_REMOVE_POS_VIEW_PROJ_MAT b1
#endif // !REMOVE_POS_FROM_VIEW

#define CB_TEXCOORD_PARAMETER_REGISTER b2

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
    
#ifdef _OUT_WORLDPOS
    float4 worldPos : POSITION0;
#endif // _OUT_WORLDPOS
    
#ifdef _OUT_CASCADE_LIGHTPOS
    float depthVS : TEXCOORD1;
#endif // _OUT_CASCADE_LIGHTPOS
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
    // Set output variable for world position
#ifdef _OUT_WORLDPOS
    output.worldPos = output.pos;
#endif // _OUT_WORLDPOS
    
    // Apply view projection matrix
#ifndef REMOVE_POS_FROM_VIEW
#ifndef _2D
    output.pos = mul(viewProjMat.viewProj, output.pos);
#else
    output.pos = mul(orthographicProjMat.proj, output.pos);
#endif // !_2D
    
#else
    output.pos = mul(removePosViewProjMat.viewProj, output.pos);
#endif // !REMOVE_POS_FROM_VIEW
    
#ifdef _OUT_CASCADE_LIGHTPOS
    output.depthVS = output.pos.w;
#endif // _OUT_CASCADE_LIGHTPOS
    
    // Copy texture coordinate
    output.uv = input.uv / texCoordParam.param.xy + texCoordParam.param.zw;

    // Apply rotation component of world matrix to normal svector
#ifndef _SPRITE
    output.normal = mul((float3x3)worldMat.world, input.normal);
#endif // !_SPRITE
    
    return output;
}
