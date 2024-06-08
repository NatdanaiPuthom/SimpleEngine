#include "../Common.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    const float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv).rgb;
    output.color = float4(albedo, 1.0f); 
    return output;
}
