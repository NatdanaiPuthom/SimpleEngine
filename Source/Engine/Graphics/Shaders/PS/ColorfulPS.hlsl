#include "../Common.hlsli"

PixelOutput main(PixelInputType aInput)
{  
    float4 position = aInput.position;
    PixelOutput output;

    //float offsetR = 0.0f;
    //float offsetG = 2.0f * 3.14159265358979323846f / 3.0f;
    //float offsetB = 4.0f * 3.14159265358979323846f / 3.0f;

    float4 textureColor = aInput.color * aTexture.Sample(aSampler, cos(position).xy);

    //output.color.r = (sin(elapsedTime + offsetR) + 1.0f) * 0.5f * textureColor.r;
    //output.color.g = (sin(elapsedTime + offsetG) + 1.0f) * 0.5f * textureColor.g;
    //output.color.b = (sin(elapsedTime + offsetB) + 1.0f) * 0.5f * textureColor.b;
   //output.color.a = aInput.color.a;
    output.color = textureColor;

    return output;
}
