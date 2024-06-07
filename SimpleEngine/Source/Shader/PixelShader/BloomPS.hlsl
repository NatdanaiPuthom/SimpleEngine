#include "../Common.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    const float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv).rgb;
    
    float2 pixelOffset = float2(ddx(aInput.uv.x), ddy(aInput.uv.y));
    
    float3 p00 = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv + pixelOffset * float2(-1.0f, -1.0f)).rgb;
    float3 p01 = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv + pixelOffset * float2(-1.0f, 1.f)).rgb;
    float3 p10 = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv + pixelOffset * float2(1.0f, -1.0f)).rgb;
    float3 p11 = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv + pixelOffset * float2(1.0f, 1.0f)).rgb;
    
    output.color.rgb = 0.25f * (p00 + p01 + p10 + p11);
    output.color.a = 1.0f;
    
    return output;
}
