#include "../PBRFunctions.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    float4 worldNormal = mul(modelWorld, float4(aInput.normal, 0.0f));
    float lightIntensity = saturate(dot(normalize(worldNormal.xyz), normalize(-directionLightDirection)));
    
    float4 albedo = aInput.color * aDefaultTexture.Sample(aSampler, aInput.uv);
  
    float3 pointLightColor = 0;
    for (unsigned int i = 0; i < numberOfPointlights; i++)
    {
        pointLightColor += pointlights[i].color.xyz;     
    }
    
    float3 radiance = albedo.xyz * (directionalLightColor.xyz + pointLightColor);
    output.color.xyz = radiance * lightIntensity * directionalLightColor.a;
    output.color.a = 1.0f;
    
    return output;
}