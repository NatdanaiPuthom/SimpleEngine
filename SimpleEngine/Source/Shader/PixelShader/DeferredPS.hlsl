#include "../Common.hlsli"

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
    
    //output.color = PBRColor(position, albedo, pixelNormal, material, vertexNormal, ambientOcclusion);
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
   
    return output;
}
