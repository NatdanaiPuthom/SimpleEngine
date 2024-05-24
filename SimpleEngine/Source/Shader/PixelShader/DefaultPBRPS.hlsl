#include "../PBRFunctions.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    float4 worldNormal = mul(modelWorld, float4(aInput.normal, 0.0f));
    float lightIntensity = saturate(dot(normalize(worldNormal.xyz), normalize(-directionLightDirection)));
    
    float4 albedo = aInput.color * GlobalAlbedo.Sample(GlobalDefaultSampler, aInput.uv);
    
    float shadowFactor = 1.0f;
    float4 directionalLightProjectedPositionTemp = mul(directionalLightWorldToProjectionMatrix, aInput.worldPosition);
    float3 directionalLightProjectedPosition = directionalLightProjectedPositionTemp.xyz / directionalLightProjectedPositionTemp.w;
    
    if (clamp(directionalLightProjectedPosition.x, -1.0, 1.0) == directionalLightProjectedPosition.x &&
        clamp(directionalLightProjectedPosition.y, -1.0, 1.0) == directionalLightProjectedPosition.y)
    {
        float computedZ = directionalLightProjectedPosition.z;
        float shadowMapZ = GlobalDirectionalLightShadowMap.Sample(GlobalDefaultSampler, 0.5f + float2(0.5f, -0.5f) * directionalLightProjectedPosition.xy).r;
        float bias = 0.001;
        
        shadowFactor = (computedZ < shadowMapZ + bias);
    }
    
    float3 radiance = albedo.rgb * (directionalLightColor.xyz) + shadowFactor;
    output.color.rgb = radiance * lightIntensity * directionalLightColor.a;
    output.color.a = albedo.a;
    
    output.color.rgb = albedo.rgb + shadowFactor * directionalLightColor.rgb;
    
    return output;
}