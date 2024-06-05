#include "../Common.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    const float2 uv = aInput.position.xy / resolution.xy;
    
    float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, uv).rgb;
      
    const float luminance = dot(float3(0.2126f, 0.7152f, 0.0722f), albedo);
    const float3 newExposeColor = exp2(exposure) * albedo;
    const float3 newSaturationColor = luminance + saturation * (albedo - luminance);
    
    albedo = newExposeColor + newSaturationColor;
    
    output.color = float4(albedo,1.0f);
    return output;
}
