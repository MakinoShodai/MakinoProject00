// Color texture
#define CB_REGISTER_MATERIAL_REGISTER b0
// Directional light parameter
#define CB_REGISTER_DIRLIGHT_REGISTER b1

// Shadow map sampler
#define SMP_REGISTER_SHADOWMAP_SMP s0

// Point lights
#define SB_REGISTER_POINT_LIGHT t0

// Main texture
#ifdef _USE_TEX
#define TEX_REGISTER_MAIN_TEX t1
#define TEX_REGISTER_SHADOWMAP_TEX t2
// Main sampler
#define SMP_REGISTER_MAIN_SMP s1
#else
#define TEX_REGISTER_SHADOWMAP_TEX t1
#endif // _USE_TEX

#include "CommonResources.hlsli"

// Input
struct INPUT {
    float4 pos : SV_POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 worldPos : POSITION0;
    float depthVS : TEXCOORD1;
};

// Shadow factor
static const float SHADOW_FACTOR = 0.3f;

// Minimum value of shadow bias
static const float MIN_SHADOW_BIAS = 0.005f;
// Maximum value of shadow bias
static const float MAX_SHADOW_BIAS = 0.015f;

// Array of offset values (used in soft shadow (PCF))
static const float2 OFFSETS[9] = {
    float2(-1, -1), float2(0, -1), float2(1, -1),
    float2(-1, 0), float2(0, 0), float2(1, 0),
    float2(-1, 1), float2(0, 1), float2(1, 1)
};

// Calculate shadow
float CalculateShadow(in float dotNL, in float3 posWS, in uint cascadeIndex) {
    // Get texture size
    float width;
    float height;
    STATIC(shadowMapTex)[cascadeIndex].GetDimensions(width, height);
    
    // Apply the shadow camera view projection matrix for the entire frustum
    float3 samplePos = mul(dirLightParam.entireShadowVPMatrix, float4(posWS, 1.0f)).xyz;
    // Apply the offset and scale for each cascade
    samplePos += dirLightParam.cascadeOffsets[cascadeIndex].xyz;
    samplePos *= dirLightParam.cascadeScales[cascadeIndex].xyz;
    
    // Compute normal bias
    float bias = max(MAX_SHADOW_BIAS * (1.0f - abs(dotNL)), MIN_SHADOW_BIAS);
    
    // Get object's depth
    float objDepth = samplePos.z - bias;
    
    // Soft shadow (PCF)
    float totalShadow = 0.0f;
    [unroll]
    for (uint j = 0; j < 9; ++j) {
        float2 offsetUV = samplePos.xy + OFFSETS[j] / width;
        float shadowMapDepth = STATIC(shadowMapTex)[cascadeIndex].Sample(shadowMapSmp, offsetUV).r;
        totalShadow += (objDepth < shadowMapDepth) ? 1.0f : SHADOW_FACTOR;
    }
    return totalShadow / 9.0f;
}

// Calculate directional light color (Phong model)
float3 CalculateDirLightColor(in float dotNL, in float3 albedoColor, in float3 N, in float3 vView, in float shadowFactor) {
    float3 lightColor = dirLightParam.color.rgb * (dirLightParam.intensity * shadowFactor);
    
    // Calculate diffuse factor
    float diffuseFactor = max(0.0f, dotNL);
    
    // Calculate diffuse color
    float3 diffuse = lightColor * diffuseFactor;
    
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    if (dotNL >= 0.0f) {
        // Calculate specular factor
        float3 vReflect = reflect(dirLightParam.lightDir.xyz, N);
        float specularFactor = saturate(dot(vReflect, vView));
        specularFactor = pow(specularFactor, material.shininess.x) * material.shininess.y;
        // Calculate specular color
        specular += lightColor * specularFactor;
    }
    
    // Return total color
    return diffuse * albedoColor + lightColor * albedoColor + specular;
}

// Calculate point light color (Phong model)
float3 CalculatePointLightColor(in float3 objPosWS, in float3 albedoColor, in float3 N, in float3 vView, in PointLightParam light, in float shadowFactor) {
    // Compute vector from the light to the object and get the distance of it
    float3 lightToObj = objPosWS - light.posWS.xyz;
    float dist = length(lightToObj);
    
    // Calculate attenuation and the color of the light to which it is applied
    float attenuation = 1.0f / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * dist * dist);
    float3 attenuatedLightColor = light.color.rgb * (attenuation * light.intensity * shadowFactor);
    
    // Calculate diffuse factor
    lightToObj = normalize(lightToObj);
    float dotNL = -dot(N, lightToObj);
    float diffuseFactor = max(0.0f, dotNL);
    
    // Calculate diffuse color
    float3 diffuse = attenuatedLightColor * diffuseFactor;
    
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    if (dotNL >= 0.0f) {
        // Calculate specular factor
        float3 vReflect = reflect(lightToObj, N);
        float specularFactor = max(0.0f, dot(vReflect, vView));
        specularFactor = pow(specularFactor, material.shininess.x) * material.shininess.y;
        // Calculate specular color
        specular += attenuatedLightColor * specularFactor;
    }
    
    // Return total color
    return diffuse * albedoColor + specular;
}

float4 main(INPUT input) : SV_TARGET{
#ifdef _USE_TEX
    float4 color = float4(DYNAMIC(mainTex).Sample(mainSmp, input.uv)) * material.color;
#else
    float4 color = material.color;
#endif // _USE_TEX
    
    clip(color.a - 0.001f);
    
    // Pre calculating
    float3 N = normalize(input.normal);
    float3 vView = normalize(dirLightParam.cameraPos.xyz - input.worldPos.xyz);
    float dotNL = -dot(N, dirLightParam.lightDir.xyz);
    
    // Shadowing
    float shadow = 1.0f;
    [unroll]
    for (uint cascadeIndex = 0; cascadeIndex < CASCADE_NUM; ++cascadeIndex) {
        if (input.depthVS <= dirLightParam.cascadeEndZ[cascadeIndex]) {
            shadow = CalculateShadow(dotNL, input.worldPos.xyz, cascadeIndex);
            break;
        }
    }
    
    // Phong shading
    // Calculate directional light color
    color.rgb = CalculateDirLightColor(dotNL, color.rgb, N, vView, shadow);
    
    // Apply point lights
    for (uint pointsIndex = 0; pointsIndex < dirLightParam.pointLightsNum; ++pointsIndex) {
        color.rgb += CalculatePointLightColor(input.worldPos.xyz, color.rgb, N, vView, STATIC(pointLights)[pointsIndex], shadow);
    }
    
    return color;
}
