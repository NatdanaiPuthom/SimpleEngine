#include "../Common.hlsli"
#include "../PBRFunctions.hlsli"

Texture2D aGrassC : register(t1);
Texture2D aRockC : register(t2);
Texture2D aSnowC : register(t3);

Texture2D aGrassN : register(t4);
Texture2D aRockN : register(t5);
Texture2D aSnowN : register(t6);

Texture2D aGrassM : register(t8);
Texture2D aRockM : register(t9);
Texture2D aSnowM : register(t10);

Texture2D aTestNormal : register(t7);

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    float slopeBlend = smoothstep(0.70f, 1.0f, aInput.normal.y);
    float heightBlend = smoothstep(-0.05f, 0.25f, aInput.worldPosition.y);
    
    float4 grassColor = aGrassC.Sample(aSampler, aInput.uv);
    float4 rockColor = aRockC.Sample(aSampler, aInput.uv);
    float4 snowColor = aSnowC.Sample(aSampler, aInput.uv);
    float3 color = lerp(rockColor, lerp(grassColor, snowColor, heightBlend), slopeBlend).rgb;
    
    float4 grassNormal = aGrassN.Sample(aSampler, aInput.uv);
    float4 rockNormal = aRockN.Sample(aSampler, aInput.uv);
    float4 snowNormal = aSnowN.Sample(aSampler, aInput.uv);
    float3 normal = lerp(rockNormal, lerp(grassNormal, snowNormal, heightBlend), slopeBlend).rgb;
    
    float3x3 TBN = float3x3(aInput.tangent, aInput.bitangent, aInput.normal);
    normal = normalize(mul(TBN, normal));
    
    float3 toEye = normalize(cameraPosition - aInput.worldPosition.xyz);
    
    //Material
    float3 grassMaterial = aGrassM.Sample(aSampler, aInput.uv).rgb;
    float3 rockMaterial = aRockM.Sample(aSampler, aInput.uv).rgb;
    float3 snowMaterial = aSnowM.Sample(aSampler, aInput.uv).rgb;
    
    float3 materialLerp = lerp(rockMaterial, lerp(grassMaterial, snowMaterial, heightBlend), slopeBlend).rgb;
    
    float Metalness = materialLerp.r;
    float Roughness = materialLerp.g;
    float Emissive = materialLerp.b;
    
    float3 specularColor = lerp((float3) 0.04f, color, Metalness);
    float3 diffuseColor = lerp((float3) 0.00f, color, 1 - Metalness);
    float ambientOcclusion = 1.0f;

    
   
    
    float3 ambiance = EvaluateAmbiance(
		aCubeMap, normal, aInput.normal.xyz,
		toEye, Roughness,
		ambientOcclusion, diffuseColor, specularColor
	);
    
    float3 directionalLight = EvaluateDirectionalLight(
		diffuseColor, specularColor, aInput.normal.xyz, Roughness,
		directionalLightColor.xyz, directionLightDirection.xyz, toEye.xyz
	);
    
    
    //float lightIntensity = max(0.0, dot(directionLightDirection, normal));
    //float3 directional = directionalLightColor * lightIntensity;
    //float3 ambient = ((0.5f + 0.5f * aInput.normal.y) * skyColor + (0.5f - 0.5f * aInput.normal.y) * groundColor);
    
    //float3 finalColor = saturate(color * (ambient + directional));
    
    //output.color = float4(finalColor, 1);
    //output.color.a = 1;
    
    output.color = float4(ambiance + directionalLight, 1);
    
    output.color.rgb = 0.5f + 0.5f * aInput.normal; //Debugging
 
    return output;
}
