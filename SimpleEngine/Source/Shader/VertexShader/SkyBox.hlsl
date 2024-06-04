#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    const float4 vertexObjectPos = aInput.position;
    const float4 vertexWorldPos = mul(modelWorld, vertexObjectPos);
    const float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.worldPosition = vertexWorldPos.xyzw;
    output.uv = float2(aInput.uv.x, 1.0 - aInput.uv.y);
    
    output.normal = mul((float3x3) modelWorld, aInput.normal);
    output.tangent = mul((float3x3) modelWorld, aInput.tangent);
    output.bitangent = mul((float3x3) modelWorld, aInput.bitangent);
    
    output.clip = aInput.clip;
    output.color = aInput.color;
    
    return output;
}
