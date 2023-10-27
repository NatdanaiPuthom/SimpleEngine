
cbuffer TimeBuffer : register(b0)
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

PixelInputType main (VertexInputType input)
{
    PixelInputType output;
    output.position = input.position;
    
    float offsetR = 0.0f; 
    float offsetG = 2.0f * 3.14159265358979323846f / 3.0f; 
    float offsetB = 4.0f * 3.14159265358979323846f / 3.0f; 

    output.color.r = (sin(elapsedTime + offsetR) + 1.0f) * 0.5f;
    output.color.g = (sin(elapsedTime + offsetG) + 1.0f) * 0.5f;
    output.color.b = (sin(elapsedTime + offsetB) + 1.0f) * 0.5f;
    output.color.a = input.color.a;
       
    return output;
}