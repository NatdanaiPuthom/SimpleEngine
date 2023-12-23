#include "../PBRFunctions.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    float3 toEye = normalize(cameraPosition.xyz - aInput.worldPosition.xyz);
    
    float3 fresnel = Fresnel_Schlick(
        float3(0.25f, 0.25f, 0.25f),
        float3(0.0f, 1.0f, 0.0f),
        toEye
    );
    
    float3 reflection = aDefaultTexture.Sample(aSampler, aInput.position.xy / float2(resolution.x, resolution.y)).rgb;
    
    output.color.rgb = fresnel * reflection;
    output.color.a = 1.0f;
    
    return output;
}