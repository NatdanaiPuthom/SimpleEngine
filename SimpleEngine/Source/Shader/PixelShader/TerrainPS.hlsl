#include "../PBRFunctions.hlsli"

Texture2D aGrassC : register(t1);
Texture2D aRockC : register(t2);
Texture2D aSnowC : register(t3);

Texture2D aGrassN : register(t4);
Texture2D aRockN : register(t5);
Texture2D aSnowN : register(t6);

Texture2D aGrassM : register(t7);
Texture2D aRockM : register(t8);
Texture2D aSnowM : register(t9);

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    float slopeBlend = smoothstep(0.70f, 1.0f, aInput.normal.y);
    float heightBlend = smoothstep(-0.05f, 0.25f, aInput.worldPosition.y);
    
    float4 grassColor = aGrassC.Sample(aSampler, aInput.uv);
    float4 rockColor = aRockC.Sample(aSampler, aInput.uv);
    float4 snowColor = aSnowC.Sample(aSampler, aInput.uv);
    
    float3 grassNormal = expandNormal(aGrassN.Sample(aSampler, aInput.uv));
    float3 rockNormal = expandNormal(aRockN.Sample(aSampler, aInput.uv));
    float3 snowNormal = expandNormal(aSnowN.Sample(aSampler, aInput.uv));
    
    float3 grassMaterial = aGrassM.Sample(aSampler, aInput.uv).rgb;
    float3 rockMaterial = aRockM.Sample(aSampler, aInput.uv).rgb;
    float3 snowMaterial = aSnowM.Sample(aSampler, aInput.uv).rgb;
    
    float4 albedo = lerp(rockColor, lerp(grassColor, snowColor, heightBlend), slopeBlend).rgba;
    float3 material = lerp(rockMaterial, lerp(grassMaterial, snowMaterial, heightBlend), slopeBlend).rgb;
    float3 normal = lerp(rockNormal, lerp(grassNormal, snowNormal, heightBlend), slopeBlend).xyz;
    
    float3 toEye = normalize(cameraPosition.xyz - aInput.worldPosition.xyz);
    float ambientOcculusion = normal.b;
    
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    normal = normalize(normal);
    
    float3x3 TBN = float3x3(
        normalize(aInput.tangent.xyz),
        normalize(aInput.bitangent.xyz),
        normalize(aInput.normal.xyz)
    );
    
    TBN = transpose(TBN);
    
    float3 pixelNormal = normalize(mul(TBN, normal));
    float metalness = material.r;
    float roughness = material.g;
    float emissive = material.b;
    
    float3 specularColor = lerp((float3) 0.0f, albedo.rgb, metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);
    
    float3 ambiance = EvaluateAmbiance(
        aCubeMap, pixelNormal, aInput.normal.xyz,
        toEye, roughness,
        ambientOcculusion, diffuseColor, specularColor
    );
    
    float3 directionalLight = EvaluateDirectionalLight(
        diffuseColor, specularColor, pixelNormal, roughness,
        directionalLightColor.xyz, directionLightDirection.xyz, toEye.xyz
    );
    
    float3 emissiveAlbedo = albedo.rgb * emissive;
    float3 radiance = ambiance + directionalLight + emissiveAlbedo;
    
    float3 color = radiance * directionalLightColor.a;
    color = tonemap_s_gamut3_cine(color);
    
    output.color.rgb = color;
    output.color.a = albedo.a;
 
    return output;
}
