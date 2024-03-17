#include "../PBRFunctions.hlsli"

Texture2D aRefractionTexture : register(t1);
Texture2D aDuDvMap : register(t2);
Texture2D aDuDvMap2 : register(t3);

PixelOutput main(PixelInputType aInput)
{   
    PixelOutput output;
    
    float2 normalizedDeviceCoords = aInput.position.xy / float2(resolution.x, resolution.y);
    
    float2 reflectUV = normalizedDeviceCoords;
    float2 refractUV = normalizedDeviceCoords;
    
    float distortionStrength = 0.01f;
    float2 wave = (aDuDvMap.Sample(aSampler, float2(aInput.uv.x + waterMoveFactor, aInput.uv.y)).rg * 2.0f - 1.0f) * distortionStrength;
    float2 wave2 = (aDuDvMap.Sample(aSampler, float2(-aInput.uv.x + waterMoveFactor, aInput.uv.y + waterMoveFactor)).rg * 2.0f - 1.0f) * distortionStrength;
    float2 totalWave = wave + wave2;
    
    reflectUV += clamp(totalWave, 0.001f, 0.999f);
    refractUV.y += clamp(totalWave.y, 0.001f, 0.999f);
    
    float4 reflectColor = aDefaultTexture.Sample(aSampler, reflectUV);
    float4 refractColor = aRefractionTexture.Sample(aSampler, refractUV);
    
    float3 toEye = normalize(cameraPosition.xyz - aInput.worldPosition.xyz);
    float refractiveFactor = dot(toEye, float3(0.0, 1.0, 0.0));
    refractiveFactor -= 0.1f;
    refractiveFactor = clamp(refractiveFactor, 0.0f, 1.0f);
    
    float4 finalColor = lerp(reflectColor, refractColor, refractiveFactor);
    
    output.color.rgb =  finalColor.xyz;
    output.color.b += 0.15f;
    output.color.a = 1.0f;

    return output;
}