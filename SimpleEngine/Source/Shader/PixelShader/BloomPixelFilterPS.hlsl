#include "../Common.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    const float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv).rgb;
    
    float3 color = float3(0.0f, 0.0f, 0.0f);
    
    if (albedo.r > 0.99f)
    {
        color.r = albedo.r;
    }
    
    if (albedo.g > 0.99f)
    {
        color.g = albedo.g;
    }
    
    if (albedo.b > 0.99f)
    {
        color.b = albedo.b;
    }
    
    output.color.rgb = color;
    output.color.a = 1.0f;
    
    return output;
}
