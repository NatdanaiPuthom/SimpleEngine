
SamplerState aSampler : register(s0);
Texture2D aTexture : register(t0);

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

struct PixelOutput
{
    float4 color : SV_Target;
};

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
};