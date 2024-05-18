#include "../PBRFunctions.hlsli"

PixelOutput main(PixelInputType aInput)
{
    PixelOutput output;
    
    float4 worldNormal = mul(modelWorld, float4(aInput.normal, 0.0f));
    float lightIntensity = saturate(dot(normalize(worldNormal.xyz), normalize(-directionLightDirection)));
    
    float4 albedo = aInput.color * aDefaultTexture.Sample(aSampler, aInput.uv);
    
    float shadowFactor = 1.0f;
    float4 directionalLightProjectedPositionTemp = mul(directionalLightWorldToProjectionMatrix, aInput.worldPosition);
    float3 directionalLightProjectedPosition = directionalLightProjectedPositionTemp.xyz / directionalLightProjectedPositionTemp.w;
    
    if (clamp(directionalLightProjectedPosition.x, -1.0f, 1.0f) == directionalLightProjectedPosition.x &&
		clamp(directionalLightProjectedPosition.y, -1.0f, 1.0f) == directionalLightProjectedPosition.y)
    {
        float computedZ = directionalLightProjectedPosition.z;
        float shadowMapZ = GlobalDirectionalLightShadowMap.Sample(aSampler, 0.5f + float2(0.5f, -0.5f) * directionalLightProjectedPosition.xy).z;

        float bias = 0.001f;
        
        shadowFactor = (computedZ < shadowMapZ + bias);
        shadowFactor += 0.0f;
    }
    
    float3 radiance = albedo.rgb * (directionalLightColor.xyz) + shadowFactor ;
    output.color.rgb = radiance * lightIntensity * directionalLightColor.a;
    output.color.a = albedo.a;
    
    return output;
}