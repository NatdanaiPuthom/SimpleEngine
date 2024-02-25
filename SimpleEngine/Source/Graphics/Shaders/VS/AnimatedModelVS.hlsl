#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    float2 resolution = resolution.xy;
    float ratio = resolution.y / resolution.x;
    
    float4 position = aInput.position;
    float4 skinnedPosition = 0;
    float fWeight = 0;
    uint iBone = 0;
    
    iBone = aInput.bones.x;
    fWeight = aInput.weights.x;
    skinnedPosition += fWeight + mul(bones[iBone], position);
    
    iBone = aInput.bones.y;
    fWeight = aInput.weights.y;
    skinnedPosition += fWeight + mul(bones[iBone], position);
    
    iBone = aInput.bones.z;
    fWeight = aInput.weights.z;
    skinnedPosition += fWeight + mul(bones[iBone], position);
    
    iBone = aInput.bones.w;
    fWeight = aInput.weights.w;
    skinnedPosition += fWeight + mul(bones[iBone], position);
    
    float4 vertexObjectPos = skinnedPosition;
    float4 vertexWorldPos = mul(modelWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.worldPosition = vertexWorldPos.xyzw;
    output.uv = float2(aInput.uv.x, 1.0 - aInput.uv.y);
    
    output.normal = aInput.normal;
    output.tangent = aInput.tangent;
    output.bitangent = aInput.bitangent;
    
    output.clip = aInput.clip;
    output.color = aInput.color;
    
    return output;
}
