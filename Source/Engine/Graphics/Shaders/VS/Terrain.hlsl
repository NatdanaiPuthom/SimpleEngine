#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    float4 vertexObjectPos = aInput.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.uv = aInput.uv;
    output.normal = aInput.normal;
    output.position = vertexClipPos;
    
    float3 lightDir = normalize(float3(0, -0.5, 1));
    output.color = aInput.color * dot(lightDir, float3(-aInput.normal.xyz));
    output.color.a = 1;
    
    return output;
}
