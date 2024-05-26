#include "../Common.hlsli"

FullScreenVertexToPixel main(FullScreenVertexInput aInput)
{
    float4 pos[3] =
    {
        float4(-1.0f, -1.0f, 0.0f, 1.0f),
		float4(-1.0f, 3.0f, 0.0f, 1.0f),
		float4(3.0f, -1.0f, 0.0f, 1.0f)
    };

    float2 uv[3] =
    {
        float2(0.0f, 1.0f),
		float2(0.0f, -1.0f),
		float2(2.0f, 1.0f)
    };
    
    FullScreenVertexToPixel output;
    output.position = pos[aInput.index];
    output.uv = uv[aInput.index];
    
    return output;
}
