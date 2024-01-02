#include "../PBRFunctions.hlsli"

Texture2D aRefractionTexture : register(t1);

PixelOutput main(PixelInputType aInput)
{   
    PixelOutput output;
    
    float2 normalizedDeviceCoords = aInput.position.xy / float2(resolution.x, resolution.y);

    float2 uv = normalizedDeviceCoords;
    
    float4 reflectColor = aDefaultTexture.Sample(aSampler, uv);
    float4 refractColor = aRefractionTexture.Sample(aSampler, uv);

    output.color = lerp(reflectColor, refractColor, 0.5f);
    output.color.b += 0.15f; // so we can see the difference between the reflection and the original texture

    output.color *= aInput.color;

    return output;
}