#include "../PBRFunctions.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    if (currentPointLightCount == 0)
    {
        discard;
        output.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
        return output;
    }
    
    float2 uv = aInput.position.xy / resolution.xy;
   
    float3 position = GlobalBufferPositionTexture.Sample(GlobalDefaultSampler, uv).rgb;
    float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, uv).rgb;
    float3 pixelNormal = normalize(2.0f * GlobalBufferNormalTexture.Sample(GlobalDefaultSampler, uv).xyz - 1.0f);
    float3 material = GlobalBufferMaterialTexture.Sample(GlobalDefaultSampler, uv).rgb;
    float4 ambientOcclusionSample = GlobalBufferAmbientOcclusionAndCustom.Sample(GlobalDefaultSampler, uv);
    
    float3 toEye = normalize(cameraPosition.xyz - position.xyz);
    
    float3 vertexNormal = normalize(ambientOcclusionSample.gba);
    float ambientOcclusion = ambientOcclusionSample.r;
    
    float metalness = material.r;
    float roughness = material.g;
    
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness); 
    
    float3 accumulatedPointLight = 0;
    uint pointLightIndex = currentPointLightCount - 1;
   
    if (distance(pointLights[pointLightIndex].position.xyz, position.xyz) <= pointLights[pointLightIndex].range)
    {
        accumulatedPointLight += EvaluatePointLight(
			    diffuseColor, specularColor, pixelNormal, roughness,
			    pointLights[pointLightIndex].color.rgb, pointLights[pointLightIndex].color.w, pointLights[pointLightIndex].range, pointLights[pointLightIndex].position.xyz,
			    toEye.xyz, position.xyz);
    } 
    
    output.color = float4(accumulatedPointLight, 1.0f);
    return output;
}
