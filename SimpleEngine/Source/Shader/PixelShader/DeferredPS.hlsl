#include "../PBRFunctions.hlsli"

float4 PBRColor(float3 aPosition, float3 aAlbedo, float3 aVertexNormal, float3 aMaterial, float3 aPixelNormal, float aAmbientOcclusion)
{
    float3 albedo = aAlbedo;
    float3 pixelNormal = aPixelNormal;
    float3 vertexNormal = aVertexNormal;
    float3 material = aMaterial;
    
    float3 toEye = normalize(cameraPosition.xyz - aPosition.xyz);
     
    float metalness = material.r;
    float roughness = material.g;
    float emissive = material.b;
    
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);
    
    float3 cubemapAmbiance = EvaluateAmbiance(
		GlobalCubeMap, pixelNormal, vertexNormal,
		toEye, roughness,
		aAmbientOcclusion, diffuseColor, specularColor
	);
    
    float3 accumulatedPointLight = 0; // <- The sum of all point lights.
    //for (unsigned int p = 0; p < currentPointLightCount; p++)
    //{
    //    if (distance(pointLights[p].position.xyz, aPosition) < pointLights[p].range)
    //    {
    //        accumulatedPointLight += EvaluatePointLight(
			 //   diffuseColor, specularColor, pixelNormal, roughness,
			 //   pointLights[p].color.rgb, pointLights[p].color.w, pointLights[p].range, pointLights[p].position.xyz,
			 //   toEye.xyz, aPosition.xyz);
    //        return float(0.0f).rrrr;
    //    }
    //}
    
    float3 emissiveAlbedo = albedo.rgb * emissive;
    float3 ambientColor = ambientLightColorAndIntensity.rgb;
    
    float3 radiance = diffuseColor * ambientColor + cubemapAmbiance + accumulatedPointLight + emissiveAlbedo;
    
    return float4(radiance, 1.0f);
}

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    float2 uv = aInput.position.xy / resolution.xy;
    
    float3 position = GlobalBufferPositionTexture.Sample(GlobalDefaultSampler, uv).rgb;
    float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, uv).rgb;
    float3 pixelNormal = normalize(2.0f * GlobalBufferNormalTexture.Sample(GlobalDefaultSampler, uv).xyz - 1.0f);
    float3 material = GlobalBufferMaterialTexture.Sample(GlobalDefaultSampler, uv).rgb;
    float4 ambientOcclusionSample = GlobalBufferAmbientOcclusionAndCustom.Sample(GlobalDefaultSampler, uv);
    
    float3 vertexNormal = ambientOcclusionSample.gba;
    float ambientOcclusion = ambientOcclusionSample.r;
    
    output.color = PBRColor(position, albedo, vertexNormal, material, pixelNormal, ambientOcclusion);
   
    return output;
}
