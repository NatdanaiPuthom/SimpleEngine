#include "../Common.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    const float3 original = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv).rgb;
    const float4 downAndUpscaled = GlobalBufferNormalTexture.Sample(GlobalDefaultSampler, aInput.uv).rgba;
    
    output.color.rgb = original + downAndUpscaled.rgb * downAndUpscaled.a;
    output.color.a = 1.0f;
    
    return output;
}
