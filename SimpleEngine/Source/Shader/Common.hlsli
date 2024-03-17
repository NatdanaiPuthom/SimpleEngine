
#define SIMPLE_MAX_BONES 64

SamplerState aSampler : register(s0);
Texture2D aDefaultTexture : register(t0);
TextureCube aCubeMap : register(t14);

cbuffer FrameBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
    float3 cameraPosition;
    
    uint2 resolution;
    float2 resolutionPadding;
    
    float waterHeight;
    float3 paddingWaterHeight;
    
    float waterMoveFactor;
    float3 paddingWaterMoveFactor;
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

cbuffer LightBuffer : register(b3)
{
    float4 skyColor;
    float4 groundColor;
    
    float3 directionLightDirection;
    float paddingDirectionalLightDirection;
    
    float4 directionalLightColor;
    
    struct PointLightData
    {
        float4 position;
        float4 color;
        float range;
        float3 padding;
    } pointlights[8];
    
    uint numberOfPointlights;
    float3 paddingPointlightAmounts;
};

cbuffer BoneBuffer : register(b5)
{
    float4x4 bones[SIMPLE_MAX_BONES];
}

struct PixelOutput
{
    float4 color : SV_Target;
};

struct VertexInputType
{
    float4 position : POSITION0;
    float4 color : COLOR0;
    float4 bones : BONES0;
    float4 weights : WEIGHTS0;
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