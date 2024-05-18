struct PostProcessVertexInput
{
	unsigned int myIndex : SV_VertexID;
};

struct PostProcessVertexToPixel
{
	float4 myPosition	: SV_POSITION;
	float2 myUV			: UV;
};

struct PostProcessPixelOutput
{
	float4 myColor		: SV_TARGET;
};

cbuffer BloomBuffer : register(b6)
{
    float3 postProcessGarbage0;
    float scale;
    float BloomBlending;
    int sampleLevel;
    
    float saturation;
    float exposure;
    
    float3 contrast;
    float postProcessGarbage2;
    float3 blackPoint;
    float postProcessGarbage3;
    float3 tint;
    float postProcessGarbage4;
};

static const float GaussianKernel5[5] =
{ 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136 };

Texture2D FullscreenTexture1 : register(t0);
Texture2D FullscreenTexture2 : register(t1);
Texture2D FullscreenTexture3 : register(t2);
Texture2D FullscreenTexture4 : register(t3);
SamplerState DefaultSampler : register(s0);