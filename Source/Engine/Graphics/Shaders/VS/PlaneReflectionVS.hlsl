#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    float4 vertexObjectPos = aInput.position;
    float clipStorage = modelWorld._44;

    float4x4 modelToWorldReal = modelWorld;
    modelToWorldReal._44 = 1.0f;

    float4 vertexWorldPos = mul(modelToWorldReal, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.worldPosition = vertexWorldPos.xyzw;
    output.uv = float2(aInput.uv.x, 1.0 - aInput.uv.y);
    
    output.normal = aInput.normal;
    output.tangent = aInput.tangent;
    output.bitangent = aInput.bitangent;
    
    output.clip = aInput.clip;
    output.color = aInput.color;
    
    bool mirrored = clipStorage > 1.5f; //this means we are rendering to the water reflection texture
    if (mirrored) //water position is -1.0 y
    {
        float waterY = -1.0f;
        float vertexWaterDistance = vertexWorldPos.y - waterY;
        output.clip = -vertexWaterDistance;
    }
    
    return output;
}