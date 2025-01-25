#include "../Common.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;

    const float3 positionDiff = aInput.worldPosition.xyz - cameraPosition;
    const float3 normalizedPositionDiff = normalize(positionDiff);

    // Sample skybox color
    const float3 skyboxColor = GlobalCubeMap.SampleLevel(GlobalDefaultSampler, normalizedPositionDiff, 0).rgb;

    // Parameters for blending
    float blendStart = -0.1; // Start blending slightly below the horizon
    float blendEnd = 0.1; // End blending slightly above the horizon

    // Early exit if skybox flooring is disabled or above the blend region
    if (!skyboxUseFlooring || normalizedPositionDiff.y > blendEnd)
    {
        output.color = float4(skyboxColor, 1.0f);
        return output;
    }

    // Horizon blending range based on vertical position
    float horizonBlend = smoothstep(blendStart, blendEnd, normalizedPositionDiff.y);

    // Ground color gradient (near the ground level)
    float groundGradient = smoothstep(-5.0, 0.0, positionDiff.y); // Soften ground gradient blending
    float3 groundColor = lerp(float3(0.3f, 0.3f, 0.3f), float3(0.4f, 0.4f, 0.4f), groundGradient); // Darker near ground

    // Haze above the horizon (light blue tint for atmospheric scattering)
    float maxHazeHeight = 2.0f; // Limit haze effect below a certain height
    float hazeStrength = saturate((normalizedPositionDiff.y - blendEnd) / (maxHazeHeight - blendEnd)); // Gradual haze fade
    hazeStrength *= smoothstep(0.0, maxHazeHeight, normalizedPositionDiff.y); // Smoothly fade haze near top

    float3 hazeColor = float3(0.8f, 0.85f, 1.0f); // Light blue for sky haze
    float3 skyWithHaze = lerp(skyboxColor, hazeColor, hazeStrength);

    // Combine ground and sky with horizon blending
    float3 blendedColor = lerp(groundColor, skyWithHaze, horizonBlend);

    // Output the final color
    output.color = float4(blendedColor, 1.0f);
    return output;
}