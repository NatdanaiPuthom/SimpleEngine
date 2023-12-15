
SamplerState aSampler : register(s0);
Texture2D aDefaultTexture : register(t0);

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
    float3 timePadding;
}

cbuffer DirectionLightBuffer : register(b3)
{
    float3 directionLight;
    float directionLightPadding;
}

struct PixelOutput
{
    float4 color : SV_Target;
};

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 uv : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPosition : WORLDPOSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 uv : TEXCOORD0;
};