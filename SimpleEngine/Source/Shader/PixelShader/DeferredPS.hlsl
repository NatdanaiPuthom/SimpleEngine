#include "../PBRFunctions.hlsli"

float4 PBRColor(float3 aPosition, float3 aAlbedo, float3 aVertexNormal, float3 aMaterial, float3 aPixelNormal, float aAmbientOcclusion)
{
    const float3 albedo = aAlbedo;
    const float3 pixelNormal = aPixelNormal;
    const float3 vertexNormal = aVertexNormal;
    const float3 material = aMaterial;
    
    const float3 toEye = normalize(cameraPosition.xyz - aPosition.xyz);
     
    const float metalness = material.r;
    const float roughness = material.g;
    const float emissive = material.b;
    
    const float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    const float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);
    
    const float3 cubemapAmbiance = EvaluateAmbiance(
		GlobalCubeMap, pixelNormal, vertexNormal,
		toEye, roughness,
		aAmbientOcclusion, diffuseColor, specularColor
	);
    
    const float3 emissiveAlbedo = albedo.rgb * emissive;
    const float3 ambientColor = ambientLightColorAndIntensity.rgb;
    
    const float3 radiance = diffuseColor * ambientColor + cubemapAmbiance + emissiveAlbedo;
    
    return float4(radiance, 1.0f);
}

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    const float2 uv = aInput.position.xy / resolution.xy;
    
    const  float3 position = GlobalBufferPositionTexture.Sample(GlobalDefaultSampler, uv).rgb;
    const float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, uv).rgb;
    const float3 pixelNormal = normalize(2.0f * GlobalBufferNormalTexture.Sample(GlobalDefaultSampler, uv).xyz - 1.0f);
    const float3 material = GlobalBufferMaterialTexture.Sample(GlobalDefaultSampler, uv).rgb;
    const float4 ambientOcclusionSample = GlobalBufferAmbientOcclusionAndCustom.Sample(GlobalDefaultSampler, uv);
    
    const float3 vertexNormal = normalize(ambientOcclusionSample.gba);
    const float ambientOcclusion = ambientOcclusionSample.r;
    
    output.color = PBRColor(position, albedo, vertexNormal, material, pixelNormal, ambientOcclusion);
   
    return output;
}
