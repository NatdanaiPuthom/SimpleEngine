
SamplerState aSampler : register(s0);
Texture2D aDefaultTexture : register(t0);
TextureCube aCubeMap : register(t14);

cbuffer FrameBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
    float3 cameraPosition;
    
    uint2 resolution;
    float2 resolutionPadding;
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 modelWorld;
}

cbuffer TimeBuffer : register(b2)
{
    float totalTime;
    float3 paddingTotalTime;
}

cbuffer DirectionLightBuffer : register(b3)
{
    float3 directionLightDirection;
    float directionLightDirectionPadding;
    
    float4 directionalLightColor;
}

cbuffer AmbientLightBuffer : register(b4)
{
    float3 skyColor;
    float paddingSkyColor;
    
    float3 groundColor;
    float paddingGroundColor;
}

struct PixelOutput
{
    float4 color : SV_Target;
};

struct VertexInputType
{
    float4 position : POSITION0;
    float4 color : COLOR0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float3 bitangent : BITANGENT0;
    float2 uv : TEXCOORD0;
    float clip : SV_ClipDistance0;
};

struct PixelInputType
{
    float4 position : SV_POSITION0;
    float4 worldPosition : WORLDPOSITION0;
    float4 color : COLOR0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float3 bitangent : BITANGENT0;
    float2 uv : TEXCOORD0;
    float clip : SV_ClipDistance0;
};