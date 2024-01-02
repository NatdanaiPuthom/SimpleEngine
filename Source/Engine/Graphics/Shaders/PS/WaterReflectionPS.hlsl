#include "../PBRFunctions.hlsli"

Texture2D aRefractionTexture : register(t1);

PixelOutput main(PixelInputType aInput)
{   
    PixelOutput output;
    
    float2 normalizedDeviceCoords = aInput.position.xy / float2(resolution.x, resolution.y);

    float2 uv = normalizedDeviceCoords;
    
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