#include "../PBRFunctions.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    const float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv).rgb;
    
    float3 color = float3(0.0f, 0.0f, 0.0f);
    
    const float3 toneMappedColor = tonemap_s_gamut3_cine(albedo);
    
    if (albedo.r > bloomPixelFilterThreshold)
    {
        color.r = toneMappedColor.r;
    }
    
    if (albedo.g > bloomPixelFilterThreshold)
    {
        color.g = toneMappedColor.g;
    }
    
    if (albedo.b > bloomPixelFilterThreshold)
    {
        color.b = toneMappedColor.b;
    }
    
    output.color.rgb = color;
    output.color.a = 1.0f;
    
    return output;
}
