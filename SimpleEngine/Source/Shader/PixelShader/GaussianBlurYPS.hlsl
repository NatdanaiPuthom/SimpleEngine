#include "../Common.hlsli"

PixelOutput main(FullScreenVertexToPixel aInput)
{
    PixelOutput output;
    
    float texelSize = 1.0f / resolution.y;
    float3 blurColor = 0;
    
    const float GaussianKernel5[5] = { 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136 };

    unsigned int kernelSize = 5;
    float start = (((float) (kernelSize) - 1.0f) / 2.0f) * -1.0f;
    
    for (unsigned int idx = 0; idx < kernelSize; idx++)
    {
        float2 uv = aInput.uv + float2(texelSize * (start + (float) idx), 0.0f);
        float3 resource = GlobalBufferAlbedoTexture.Sample(GlobalDefaultSampler, aInput.uv).rgb;
        blurColor += resource * GaussianKernel5[idx];
    }

    output.color.rgb = blurColor;
    output.color.a = bloom;
    
    return output;
}
