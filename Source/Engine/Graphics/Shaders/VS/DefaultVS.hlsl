#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    float4 vertexObjectPos = aInput.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.color = aInput.color;
    
    output.uv = float2(aInput.uv.x, 1.0 - aInput.uv.y);
    
    output.worldPosition = vertexWorldPos.xyzw;
    output.normal = aInput.normal;
    output.tangent = aInput.tangent;
    output.bitangent = aInput.bitangent;
    output.clip = aInput.clip;

    return output;
}
