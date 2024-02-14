// Color texture
#define CB_REGISTER_COLOR_REGISTER b0

// Main texture
#ifdef _USE_TEX
#define TEX_REGISTER_MAIN_TEX t0
#endif // _USE_TEX

// Main sampler
#define SMP_REGISTER_MAIN_SMP s0

#include "CommonResources.hlsli"

// Input
struct INPUT {
    float4 pos : SV_POSITION0;
#ifdef _USE_TEX
    float2 uv : TEXCOORD0;
#endif
};

float4 main(INPUT input) : SV_TARGET {
#ifdef _USE_TEX
    float4 color = float4(DYNAMIC(mainTex).Sample(mainSmp, input.uv)) * changeColor.color;
#else
    float4 color = changeColor.color;
#endif // _USE_TEX
    
    clip(color.a - 0.001f);
    
    return color;
}
