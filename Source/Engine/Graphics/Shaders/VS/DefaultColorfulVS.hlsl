#include "../Common.hlsli"

PixelInputType main (VertexInputType aInput)
{
    PixelInputType output;
    
    float4 vertexObjectPos = aInput.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
   
    float offsetR = 0.0f;
    float offsetG = 2.0f * 3.14159265358979323846f / 3.0f;
    float offsetB = 4.0f * 3.14159265358979323846f / 3.0f;
    
    output.color.r = (sin(elapsedTime + offsetR) + 1.0f) * 0.5f;
    output.color.g = (sin(elapsedTime + offsetG) + 1.0f) * 0.5f;
    output.color.b = (sin(elapsedTime + offsetB) + 1.0f) * 0.5f;
    output.color.a = aInput.color.a;
    
    output.normal = aInput.normal;
    output.uv = aInput.uv;
    
    return output;
}