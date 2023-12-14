#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    float4 vertexObjectPos = aInput.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.uv = aInput.uv;
    output.position = vertexClipPos;
    
    output.color = aInput.color; /* * dot(directionLight, float3(-aInput.normal.xyz))*/;
    output.color.a = 1;
    output.normal = aInput.normal;
    
    return output;
}
