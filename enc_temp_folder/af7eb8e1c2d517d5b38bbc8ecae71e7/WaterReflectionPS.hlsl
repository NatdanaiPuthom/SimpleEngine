#include "../PBRFunctions.hlsli"

Texture2D aRefractionTexture : register(t1);
Texture2D aDuDvMap : register(t2);
Texture2D aDuDvMap2 : register(t3);

PixelOutput main(PixelInputType aInput)
{   
    PixelOutput output;
    
    float2 normalizedDeviceCoords = aInput.position.xy / float2(resolution.x, resolution.y);

    float2 uv = normalizedDeviceCoords;
    
    float distortionStrength = 0.02f;
    float moveFactor = sin(totalTime + 0.001f) * 0.01f;
    
    float2 wave = (aDuDvMap.Sample(aSampler, float2(aInput.uv.x + moveFactor, aInput.uv.y)).rg * 2.0f - 1.0f) * distortionStrength;
    float2 wave2 = (aDuDvMap2.Sample(aSampler, float2(-aInput.uv.x + moveFactor, aInput.uv.y + moveFactor)).rg * 2.0f - 1.0f) * distortionStrength;
    float2 totalWave = wave + wave2;
    uv += clamp(totalWave, 0.001f, 0.999f);
   
    
    
    float4 reflectColor = aDefaultTexture.Sample(aSampler, uv);
    float4 refractColor = aRefractionTexture.Sample(aSampler, uv);
    
    float4 finalColor = lerp(reflectColor, refractColor, 0.5f);
    
    float3 toEye = normalize(cameraPosition.xyz - aInput.worldPosition.xyz);
    float3 fresnel = Fresnel_Schlick(
        float3(0.75f, 0.75f, 0.75f),
        float3(0.0f, 1.0f, 0.0f),
        toEye
    );
    
    output.color.rgb = fresnel * finalColor.xyz;
    output.color.a = 1.0f;

    return output;
}