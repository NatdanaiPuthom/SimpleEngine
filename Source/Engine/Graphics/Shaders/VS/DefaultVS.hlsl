#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    float4 vertexObjectPos = aInput.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.color = aInput.color;
    output.uv = aInput.uv;
    output.normal = aInput.normal;

    return output;
}
