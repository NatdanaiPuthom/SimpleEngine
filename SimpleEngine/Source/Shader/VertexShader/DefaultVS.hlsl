#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    const float4 vertexObjectPos = aInput.position;
    const float4 vertexWorldPos = mul(aInput.instanceTransform, vertexObjectPos);
    const float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.worldPosition = vertexWorldPos.xyzw;
    output.uv = float2(aInput.uv.x, 1.0 - aInput.uv.y);
     
    output.normal = mul((float3x3) aInput.instanceTransform, aInput.normal);
    output.tangent = mul((float3x3) aInput.instanceTransform, aInput.tangent);
    output.bitangent = mul((float3x3) aInput.instanceTransform, aInput.bitangent);
    
    output.color = aInput.color;
    
    return output;
}
