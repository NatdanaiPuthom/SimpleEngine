#include "../Common.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    const float3 positionDiff = aInput.worldPosition.xyz - cameraPosition;
    const float3 normalizedPositionDiff = normalize(positionDiff);
    
    if (skyboxUseFlooring && normalizedPositionDiff.y < 0)
    {    
        float gradient = saturate((positionDiff.y + 5.0) / 10.0); // Adjust scale and offset
        float3 groundColor = lerp(float3(0.35f, 0.35f, 0.35f), float3(0.4f, 0.4f, 0.4f), gradient);

        output.color = float4(groundColor, 1.0f);
        return output;
    }
    
    const float3 color = GlobalCubeMap.SampleLevel(GlobalDefaultSampler, normalizedPositionDiff, 0).rgb;

    output.color = float4(color, 1);
    
    return output;
}
