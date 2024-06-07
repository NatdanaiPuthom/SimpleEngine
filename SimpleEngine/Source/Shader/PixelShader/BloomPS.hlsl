#include "../Common.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    const float3 original = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv).rgb;
    const float3 downAndUpscaled = GlobalBufferNormalTexture.Sample(GlobalDefaultSampler, aInput.uv).rgb;
    
    output.color.rgb = original + downAndUpscaled;
    output.color.a = 1.0f;
    
    return output;
}
