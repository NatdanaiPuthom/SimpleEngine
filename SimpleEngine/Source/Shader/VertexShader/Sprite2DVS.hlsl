#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    float4 vertexObjectPos = aInput.position;
    float4 vertexWorldPos = mul(modelWorld, vertexObjectPos);
    
    output.position = vertexWorldPos;
    output.worldPosition = vertexWorldPos;
    output.uv = float2(aInput.uv.x, 1.0 - aInput.uv.y);
    
    output.normal = aInput.normal;
    output.tangent = aInput.tangent;
    output.bitangent = aInput.bitangent;
    
    output.clip = aInput.clip;
    output.color = aInput.color;
    
    return output;
}
