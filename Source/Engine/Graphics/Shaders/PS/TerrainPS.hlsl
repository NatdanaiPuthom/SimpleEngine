#include "../Common.hlsli"

Texture2D aGrassC : register(t0);
Texture2D aRockC : register(t1);
Texture2D aSnowC : register(t2);

Texture2D aGrassN : register(t3);
Texture2D aSnowN : register(t4);
Texture2D aRockN : register(t5);

float3 expandNormal(float4 normalTexture)
{
    float3 normal = normalTexture.agg;
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    return normalize(normal);
}

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    float slopeBlend = smoothstep(0.70f, 1.0f, aInput.normal.y);
    float heightBlend = smoothstep(-0.05f, 0.25f, aInput.worldPosition.y);
    
    float4 rockColor = aRockC.Sample(aSampler, aInput.uv);
    float4 snowColor = aSnowC.Sample(aSampler, aInput.uv);
    float4 grassColor = aGrassC.Sample(aSampler, aInput.uv);
    
    float3 color = lerp(rockColor, lerp(grassColor, snowColor, heightBlend), slopeBlend).rgb;
    
    output.color = float4(color, 1);
 
    return output;
}
