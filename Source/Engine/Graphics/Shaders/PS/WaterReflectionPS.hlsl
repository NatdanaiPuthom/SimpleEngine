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
    //PixelOutput output;
    
    //float4x4 cameraTranspose = transpose(worldToClipMatrix);
    //float3 toEye = cameraTranspose[3].xyz - aInput.worldPosition.xyz;
    //float dist = abs(dot(toEye, cameraTranspose[2].xyz));
    
    //float2 p = aInput.worldPosition.xz;
    //float2 k0 = float2(6.f, 16.f);
    //float2 k1 = float2(10.f, -14.f);
    //float A = 0.0005f;
    
    //float2 heightDerivative = k0 * sin(dot(p, k0) + elapsedTime) + k1 * sin(dot(p, k1) + elapsedTime);
    //float2 maxValue = float2(0.f, length(k0) + length(k1));

    //float2 offset = A * (maxValue + heightDerivative) / dist;
    
    //float3 fresnel = Fresnel_Schlick(
    //    float3(0.25f, 0.25f, 0.25f),
    //    float3(0.0f, 1.0f, 0.0f),
    //    toEye
    //);
    
    //float3 reflection = aDefaultTexture.Sample(aSampler, aInput.position.xy / resolution + offset).rgb;
    //output.color.rgb = fresnel* reflection;
    //output.color.a = 1.0f;
    
 //   PixelOutput output;
    
 //   float3 color = float3(0.145f, 0.322f, 0.353f);
    
 //   float metalness = 0.75f;
 //   float roughness = 0.5f;
 //   float emissive = 0.0f;
    
 //   float3 radiance = float3(0.0f, 0.0f, 0.0f);
 //   radiance = EvalPBR(aInput.worldPosition.xyz, color.rgb, float3(0.0f, -1.0f, 0.0f), aInput.normal, metalness, roughness, 0.0f);

 //   float3 toEye = normalize(cameraPosition.xyz - aInput.worldPosition.xyz);
    
 //   float3 fresnel = Fresnel_Schlick(
	//	float3(0.25f, 0.25f, 0.25f),
	//	float3(0.0f, 1.0f, 0.0f),
	//	toEye
	//);
    
 //   float3 reflection = aDefaultTexture.Sample(aSampler, (aInput.position.xy / resolution)).rgb;
 //   output.color.rgb = fresnel * reflection + radiance;
 //   output.color.a = 0.5f;
    
 //   return output;
    
    PixelOutput output;
    float2 flippedUV = float2(aInput.uv.x, 1.0 - aInput.uv.y);
    output.color = aDefaultTexture.Sample(aSampler, flippedUV);
    
    return output;
}