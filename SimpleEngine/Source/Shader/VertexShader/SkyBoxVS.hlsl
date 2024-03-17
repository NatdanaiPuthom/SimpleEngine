#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    float4 vertexObjectPos = aInput.position;
    float4 vertexWorldPos = mul(modelWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.worldPosition = vertexWorldPos;
    output.uv = float2(aInput.uv.x, 1.0f - aInput.uv.y);
    
    output.normal = normalize(mul(modelWorld, float4(aInput.normal, 0)).xyz);
    output.tangent = normalize(mul(modelWorld, float4(aInput.tangent, 0)).xyz);
    output.bitangent = normalize(mul(modelWorld, float4(aInput.bitangent, 0)).xyz);
    output.clip = aInput.clip;
    
    output.color = aInput.color;
    
    return output;
}
