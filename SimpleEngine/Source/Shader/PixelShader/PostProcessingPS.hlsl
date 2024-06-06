#include "../PBRFunctions.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    const float2 uv = aInput.position.xy / resolution.xy;
    
    float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, uv).rgb;
      
    const float luminance = dot(float3(0.2126, 0.7152, 0.0722), albedo);
    const float3 newSaturationColor = luminance + saturation * (albedo - luminance);
    albedo = newSaturationColor;
    
    const float3 newExposureColor = exp2(exposure) * albedo;
    albedo = newExposureColor;
    
    const float3 newContrast = 0.18 * pow(albedo / 0.18, constrast);
    albedo = newContrast;
    
    const float3 newTint = tint * albedo;
    albedo = newTint;
    
    const float3 newBlackPoint = max(0.0f, albedo - blackpoint);
    albedo = newBlackPoint;
    
   //albedo = tonemap_s_gamut3_cine(albedo); //NOTE(v10.0.4): This look trash
    
    output.color = float4(albedo, 1.0f);
    return output;
}
