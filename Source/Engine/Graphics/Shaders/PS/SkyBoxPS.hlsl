#include "../Common.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
 
 
    float3 cubeMap = aCubeMap.Sample(aSampler, aInput.position.xyz).rgb;

    output.color = float4(cubeMap,1);
    
    return output;
}
