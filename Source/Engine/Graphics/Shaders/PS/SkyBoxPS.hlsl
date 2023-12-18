#include "../Common.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
 
    float3 cubeMap = normalize(aInput.worldPosition.xyz - cameraPosition);
    
    float3 color = aCubeMap.Sample(aSampler, cubeMap).rgb;

    output.color = float4(color, 1);
    
    return output;
}
