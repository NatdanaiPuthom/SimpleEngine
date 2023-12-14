#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    float4 vertexObjectPos = aInput.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.normal = aInput.normal;
    output.uv = aInput.uv;
    
    //output.color = aInput.color * dot(directionLight, float3(-aInput.normal.xyz)); //Directional Light
    output.color = aInput.color; ;
    output.color.a = 1;
    
    return output;
}
