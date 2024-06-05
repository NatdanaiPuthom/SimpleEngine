#include "../PBRFunctions.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
   
    const float2 uv = aInput.position.xy / resolution.xy;
   
    const float3 position = GlobalBufferPositionTexture.Sample(GlobalDefaultSampler, uv).rgb;
    const float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, uv).rgb;
    const float3 pixelNormal = normalize(2.0f * GlobalBufferNormalTexture.Sample(GlobalDefaultSampler, uv).xyz - 1.0f);
    const float3 material = GlobalBufferMaterialTexture.Sample(GlobalDefaultSampler, uv).rgb;
    
    const float3 toEye = normalize(cameraPosition.xyz - position.xyz);
    
    const float metalness = material.r;
    const float roughness = material.g;
    
    const float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    const float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);
    
    const float3 pointLightColor = EvaluatePointLight(
            diffuseColor,
            specularColor,
            pixelNormal,
            roughness,
            pointLights[0].color.rgb,
            pointLights[0].color.w,
            pointLights[0].range,
            pointLights[0].position.xyz,
            toEye.xyz,
            position.xyz
        );
    
    output.color = float4(pointLightColor, 1.0f);
    return output;
}
