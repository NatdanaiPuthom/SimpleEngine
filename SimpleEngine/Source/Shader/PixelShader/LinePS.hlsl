#include "../Common.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    output.color = aInput.color;
    
    return output;
}
