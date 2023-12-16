#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    float4 vertexObjectPos = aInput.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.worldPosition = vertexWorldPos;
    output.uv = aInput.uv;
    
    output.normal = mul(modelToWorld, float4(aInput.normal, 0)).xyz;
    output.tangent = mul(modelToWorld, float4(aInput.tangent, 0)).xyz;
    output.bitangent = mul(modelToWorld, float4(aInput.bitangent, 0)).xyz;
    
    output.color = aInput.color;
    
    return output;
}
