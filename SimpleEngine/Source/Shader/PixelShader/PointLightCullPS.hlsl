#include "../PBRFunctions.hlsli"

//PixelOutput main(PixelInputType aInput)
//{
//    PixelOutput output;
 
//    float3 accumulatedPointLight = 0;
//    if (distance(pointLights[0].position.xyz, aInput.worldPosition.xyz) < pointLights[0].range)
//    {
//        accumulatedPointLight += EvaluatePointLight(
//			    diffuseColor, specularColor, pixelNormal, roughness,
//			    pointLights[p].color.rgb, pointLights[p].color.w, pointLights[p].range, pointLights[p].position.xyz,
//			    toEye.xyz, aPosition.xyz);
//        return float(1.0f).rrrr;
//    }

//    output.color = float(1.0f).rrrr;
    
//    return output;
//}


PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    float2 uv = aInput.position.xy / resolution.xy;
   
    float3 position = GlobalBufferPositionTexture.Sample(GlobalDefaultSampler, uv).rgb;
    float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, uv).rgb;
    float3 pixelNormal = normalize(2.0f * GlobalBufferNormalTexture.Sample(GlobalDefaultSampler, uv).xyz - 1.0f);
    float3 material = GlobalBufferMaterialTexture.Sample(GlobalDefaultSampler, uv).rgb;
    float4 ambientOcclusionSample = GlobalBufferAmbientOcclusionAndCustom.Sample(GlobalDefaultSampler, uv);
    
    float3 toEye = normalize(cameraPosition.xyz - position.xyz);
    
    float3 vertexNormal = ambientOcclusionSample.gba;
    float ambientOcclusion = ambientOcclusionSample.r;
    
    float metalness = material.r;
    float roughness = material.g;
    
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness); 
    
    float3 accumulatedPointLight = 0;
    uint pointLightIndex = currentPointLightCount - 1;
    if (distance(pointLights[pointLightIndex].position.xyz, position.xyz) < pointLights[pointLightIndex].range)
    {
        accumulatedPointLight += EvaluatePointLight(
			    diffuseColor, specularColor, pixelNormal, roughness,
			    pointLights[pointLightIndex].color.rgb, pointLights[pointLightIndex].color.w, pointLights[pointLightIndex].range, pointLights[pointLightIndex].position.xyz,
			    toEye.xyz, position.xyz);
    } 
    
    output.color = float4(accumulatedPointLight, 1.0f);
    return output;
}
