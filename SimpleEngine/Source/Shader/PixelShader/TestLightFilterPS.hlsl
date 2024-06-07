#include "../Common.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv).rgb;
    
    float3 color = float3(0.0f, 0.0f, 0.0f);
    
    if (albedo.r > 0.8)
    {
        color.r = albedo.r;
    }
    
    if (albedo.g > 0.8)
    {
        color.g = albedo.g;
    }
    
    if (albedo.b > 0.8)
    {
        color.b = albedo.b;
    }
    
    
    output.color.rgb = color;
    output.color.a = 1.0f;
    
    return output;
}
