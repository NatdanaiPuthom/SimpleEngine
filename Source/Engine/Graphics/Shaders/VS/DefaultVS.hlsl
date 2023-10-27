
cbuffer FrameBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 modelToWorld;
}

cbuffer TimeBuffer : register(b2)
{
    float elapsedTime;
    float3 padding;
}

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInputType main (VertexInputType aInput)
{
    PixelInputType output;
    
    float4 vertexObjectPos = aInput.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToClipMatrix, vertexWorldPos);
    
    output.position = vertexClipPos;
    output.color = aInput.color;
    
    return output;
}