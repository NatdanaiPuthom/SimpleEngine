#include "../Common.hlsli"

Texture2D aGrassC : register(t0);
Texture2D aRockC : register(t1);
Texture2D aSnowC : register(t2);

Texture2D aGrassN : register(t3);
Texture2D aSnowN : register(t4);
Texture2D aRockN : register(t5);

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
    
    float lightIntensity = max(0.0, dot(normal, directionLight));
    
    float3 finalColor = color * lightIntensity;
    
    output.color = float4(finalColor, 1);
 
    return output;
}
