
#define SIMPLE_MAX_JOINTS 64

SamplerState GlobalDefaultSampler : register(s0);

Texture2D GlobalAlbedo                      : register(t0);
Texture2D GlobalMaterialTexture             : register(t1);
Texture2D GlobalNormalTexture               : register(t2);
Texture2D GlobalAmbientOcclusionAndCustom   : register(t3);
TextureCube GlobalCubeMap                   : register(t4);
Texture2D GlobalDirectionalLightShadowMap   : register(t5);

Texture2D GlobalBufferPositionTexture           : register(t4);
Texture2D GlobalBufferAlbedoTexture             : register(t5);
Texture2D GlobalBufferNormalTexture             : register(t6);
Texture2D GlobalBufferMaterialTexture           : register(t7);
Texture2D GlobalBufferAmbientOcclusionAndCustom : register(t8);

cbuffer CameraBuffer : register(b0)
{
    float4x4 worldToClipMatrix;
    float3 cameraPosition;
    float paddingCameraBuffer;
    
    uint2 resolution;
    float2 resolutionPadding;
}

cbuffer TransformBuffer : register(b1)
{
    float4x4 modelWorld;
}

cbuffer TimeBuffer : register(b2)
{
    float totalTime;
    float deltaTime;
    float2 paddingTotalTime;
}

cbuffer LightBuffer : register(b3)
{  
    float4x4 directionalLightWorldToProjectionMatrix;
    float4 directionalLightColor;
   
    float3 directionLightDirection;
    float paddingDirectionalLightDirection;
};

cbuffer JointBuffer : register(b4)
{
    float4x4 bones[SIMPLE_MAX_JOINTS];
}

struct FullScreenVertexInput
{
    unsigned int index : SV_VertexID;
};

struct FullScreenVertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv       : UV;
};

struct PixelOutput
{
    float4 color : SV_Target;
};

struct GBufferOutput
{
    float4 position                  : SV_TARGET0;
    float4 albedo                    : SV_TARGET1;
    float4 normal                    : SV_TARGET2;
    float4 material                  : SV_TARGET3;
    float4 ambientOcclusionAndCustom : SV_TARGET4;
};

struct VertexInputType
{
    float4 position     : POSITION0;
    float4 color        : COLOR0;
    float4 bones        : BONES0;
    float4 weights      : WEIGHTS0;
    float3 normal       : NORMAL0;
    float3 tangent      : TANGENT0;
    float3 bitangent    : BITANGENT0;
    float2 uv           : TEXCOORD0;
    float clip          : SV_ClipDistance0;
};

struct PixelInputType
{
    float4 position         : SV_POSITION0;
    float4 worldPosition    : WORLDPOSITION0;
    float4 color            : COLOR0;
    float3 normal           : NORMAL0;
    float3 tangent          : TANGENT0;
    float3 bitangent        : BITANGENT0;
    float2 uv               : TEXCOORD0;
    float clip              : SV_ClipDistance0;
};