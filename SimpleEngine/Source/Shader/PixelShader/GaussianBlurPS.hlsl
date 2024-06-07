#include "../Common.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    //const float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv).rgb;
    
    //float2 pixelOffset = float2(ddx(aInput.uv.x), ddy(aInput.uv.y));
    
    //float3 p00 = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv + pixelOffset * float2(-1.0f, -1.0f)).rgb;
    //float3 p01 = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv + pixelOffset * float2(-1.0f, 1.f)).rgb;
    //float3 p10 = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv + pixelOffset * float2(1.0f, -1.0f)).rgb;
    //float3 p11 = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv + pixelOffset * float2(1.0f, 1.0f)).rgb;
    
    //output.color.rgb = 0.25f * (p00 + p01 + p10 + p11);
    //output.color.a = bloom;
    
    const float3 albedo = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv).rgb;
    const float2 pixelOffset = float2(ddx(aInput.uv.x), ddy(aInput.uv.y));

    const float kernel[25] =
    {
        1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f,
        4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f,
        7.0f / 273.0f, 26.0f / 273.0f, 41.0f / 273.0f, 26.0f / 273.0f, 7.0f / 273.0f,
        4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f,
        1.0f / 273.0f, 4.0f / 273.0f, 7.0f / 273.0f, 4.0f / 273.0f, 1.0f / 273.0f
    };

   const float2 offsets[25] =
    {
        float2(-2.0f, -2.0f), float2(-1.0f, -2.0f), float2(0.0f, -2.0f), float2(1.0f, -2.0f), float2(2.0f, -2.0f),
        float2(-2.0f, -1.0f), float2(-1.0f, -1.0f), float2(0.0f, -1.0f), float2(1.0f, -1.0f), float2(2.0f, -1.0f),
        float2(-2.0f, 0.0f), float2(-1.0f, 0.0f), float2(0.0f, 0.0f), float2(1.0f, 0.0f), float2(2.0f, 0.0f),
        float2(-2.0f, 1.0f), float2(-1.0f, 1.0f), float2(0.0f, 1.0f), float2(1.0f, 1.0f), float2(2.0f, 1.0f),
        float2(-2.0f, 2.0f), float2(-1.0f, 2.0f), float2(0.0f, 2.0f), float2(1.0f, 2.0f), float2(2.0f, 2.0f)
    };

    float3 colorSum = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 25; ++i)
    {
        float2 sampleUV = aInput.uv + offsets[i] * pixelOffset;
        float3 sampleColor = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, sampleUV).rgb;
        colorSum += sampleColor * kernel[i];
    }

    output.color.rgb = colorSum;
    output.color.a = bloom;
     
    return output;
}
