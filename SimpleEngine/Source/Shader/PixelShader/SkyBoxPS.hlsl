#include "../Common.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
 
    const float3 position = normalize(aInput.worldPosition.xyz - cameraPosition);
    const float3 color = GlobalCubeMap.SampleLevel(GlobalDefaultSampler, position, 0).rgb;

    output.color = float4(color, 1);
    
    return output;
}
