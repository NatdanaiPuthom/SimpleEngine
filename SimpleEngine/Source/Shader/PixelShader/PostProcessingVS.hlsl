#include "../Common.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    const float2 uv = aInput.position.xy / resolution.xy;
    float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, uv).rgb;
  
    albedo += tint;
    
    output.color = float4(albedo,1.0f);
    
    return output;
 
}
