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
    
    float3 color = float3(0.145f, 0.322f, 0.353f);
    
    float metalness = 0.75f;
    float roughness = 0.5f;
    float emissive = 0.0f;
    
    float3 radiance = float3(0.0f, 0.0f, 0.0f);
    radiance = EvalPBR(aInput.worldPosition.xyz, color.rgb, float3(0.0f,-1.0f,0.0f), aInput.normal, metalness, roughness, 0.0f);

    float3 toEye = normalize(cameraPosition.xyz - aInput.worldPosition.xyz);
    
    float3 fresnel = Fresnel_Schlick(
		float3(0.25f, 0.25f, 0.25f),
		float3(0.0f, 1.0f, 0.0f),
		toEye
	);
    
    float3 reflection = aDefaultTexture.Sample(aSampler, (aInput.position.xy / resolution)).rgb;
    output.color.rgb = fresnel * reflection + radiance;
    output.color.a = 0.5f;
    
    return output;
}