#include "../Common.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    float offsetR = 0.0f;
    float offsetG = 2.0f * 3.14159265358979323846f / 3.0f;
    float offsetB = 4.0f * 3.14159265358979323846f / 3.0f;
    
    output.color.r = (sin(totalTime + offsetR) + 1.0f) * 0.5f;
    output.color.g = (sin(totalTime + offsetG) + 1.0f) * 0.5f;
    output.color.b = (sin(totalTime + offsetB) + 1.0f) * 0.5f;
    output.color.a = aInput.color.a;
    
    return output;
}