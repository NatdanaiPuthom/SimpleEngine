#include "../PBRFunctions.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    const float2 uv = aInput.position.xy / resolution.xy;
    
    float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, uv).rgb;
      
    const float luminance = dot(float3(0.2126, 0.7152, 0.0722), albedo);
    
    albedo = luminance + saturation * (albedo - luminance); //NOTE(v10.0.5): Apply saturation
    albedo = exp2(exposure) * albedo; //NOTE(v10.0.5): Apply exposure 
    albedo = 0.18 * pow(abs(albedo / 0.18), contrast); //NOTE(v10.0.5): Apply contrast
    albedo *= tint; //NOTE(v10.0.5): Apply tint
    albedo = max(0.0f, albedo - blackpoint); //NOTE(v10.0.5): Apply blackpoint
    
    if (useToneMapping)
    {
        //albedo = tonemap_s_gamut3_cine(albedo); //NOTE(v10.0.4): This look trash
        albedo = ACESFilm(albedo); //NOTE(v10.0.5): This also look trash
    }
    
    output.color = float4(albedo, 1.0f);
    return output;
}
