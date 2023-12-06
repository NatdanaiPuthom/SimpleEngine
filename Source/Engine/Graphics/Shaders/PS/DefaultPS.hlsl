#include "../Common.hlsli"

PixelOutput main(PixelInputType aInput)
{  
    PixelOutput output;
    
    output.color = aInput.color * aTexture.Sample(aSampler, aInput.uv);
    
    return output;
}
