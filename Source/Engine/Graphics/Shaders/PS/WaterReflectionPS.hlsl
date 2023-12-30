#include "../PBRFunctions.hlsli"

float3 EvalPBR(in float3 worldPosition, in float3 color, in float3 normal, in float3 vertexNormal, in float metalness, in float roughness, in float emissive)
{
    float3 toEye = normalize(cameraPosition.xyz - worldPosition);
    float3 specularColor = lerp((float3) 0.04f, color.rgb, metalness);
    float3 diffuseColor = lerp((float3) 0.00f, color.rgb, 1 - metalness);
    float ambientOcclusion = 1.0f;

    float3 ambiance = EvaluateAmbiance(
        aCubeMap, normal, vertexNormal,
        toEye, roughness,
        ambientOcclusion, diffuseColor, specularColor
    );

    float3 directionalLight = EvaluateDirectionalLight(
        diffuseColor, specularColor, normal, roughness,
        directionalLightColor.xyz, directionLightDirection.xyz, toEye.xyz
    );

    float3 emissiveAlbedo = color.rgb * emissive;

    return (ambiance + directionalLight + emissiveAlbedo);
}

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