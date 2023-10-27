
cbuffer FrameBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 modelToWorld;
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

struct PixelOutput
{
    float4 color : SV_TARGET;
};
