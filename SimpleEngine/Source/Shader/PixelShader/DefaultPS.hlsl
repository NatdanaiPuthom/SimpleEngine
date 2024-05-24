#include "../Common.hlsli"

PixelOutput main(PixelInputType aInput)
{  
    PixelOutput output;
    
    output.color = aInput.color * GlobalAlbedo.Sample(GlobalDefaultSampler, aInput.uv);
    
    return output;
}
