
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};

PixelOutput main (PixelInputType input)
{  
    PixelOutput result;
    float2 uv = input.position.xy / float2(1280, 720);

    float4 colorValue;

    if (uv.x < 0.5f)
    {
        colorValue = float4(uv.y, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        colorValue.r = (sin(uv.y * 6.28f) + 1) * 0.5f;
        colorValue.g = uv.y;
        colorValue.b = 1.0f - uv.y;
        colorValue.a = 1.0f;
    }

    result.color = colorValue;
    return result;
}