#include "../Common.hlsli"

Texture2D aTexture : register(t0);
SamplerState aSampler : register(s0);

PixelOutput main(PixelInputType input)
{  
    PixelOutput result;
    
    result.color = aTexture.Sample(aSampler, input.uv.xy).rgba;
    
    return result;
}
