#include "../Common.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    float4 worldNormal = mul(modelWorld, float4(aInput.normal, 0.0f));
    float lightIntensity = saturate(dot(normalize(worldNormal.xyz), normalize(-directionLightDirection)));
    
    float4 color = aInput.color * aDefaultTexture.Sample(aSampler, aInput.uv);
    output.color = color * lightIntensity * directionalLightColor * directionalLightColor.a;
    output.color.a = 1.0f;
    
    return output;
}