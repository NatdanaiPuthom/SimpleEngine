#include "../Common.hlsli"

PixelInputType main(VertexInputType aInput)
{
    PixelInputType output;
    
    float4 vertexObjectPos = aInput.position;
    
    float4x4 modelToWorldReal = modelToWorld;
    float clipStorage = modelToWorld._44;
    modelToWorldReal._44 = 2.0f;

    float4 vertexWorldPos = mul(modelToWorldReal, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.worldPosition = vertexWorldPos;
    output.uv = float2(aInput.uv.x, 1.0f - aInput.uv.y);
    
    output.normal = normalize(mul(modelToWorldReal, float4(aInput.normal, 0)).xyz);
    output.tangent = normalize(mul(modelToWorldReal, float4(aInput.tangent, 0)).xyz);
    output.bitangent = normalize(mul(modelToWorldReal, float4(aInput.bitangent, 0)).xyz);

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
