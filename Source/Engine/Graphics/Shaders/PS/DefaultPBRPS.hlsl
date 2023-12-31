#include "../Common.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    float lightIntensity = saturate(dot(normalize(aInput.normal), normalize(-directionLightDirection)));
    
    float4 color = aInput.color * aDefaultTexture.Sample(aSampler, aInput.uv);
    output.color = color * lightIntensity;
    output.color.a = 1.0f;
    
    return output;
}