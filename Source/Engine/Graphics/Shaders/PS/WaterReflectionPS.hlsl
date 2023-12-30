#include "../PBRFunctions.hlsli"

PixelOutput main(PixelInputType aInput)
{   
    PixelOutput output;
    
    float2 normalizedDeviceCoords = aInput.position.xy / float2(resolution.x, resolution.y);

    float2 uv = normalizedDeviceCoords;

    output.color = aDefaultTexture.Sample(aSampler, uv);
    output.color.b += 0.15f; // so we can see the difference between the reflection and the original texture

    output.color *= aInput.color;

    return output;
}