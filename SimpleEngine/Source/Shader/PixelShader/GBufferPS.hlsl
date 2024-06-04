#include "../Common.hlsli"

GBufferOutput main(PixelInputType aInput)
{
    const float2 uv = aInput.uv;
    
    const float4 diffuse = GlobalAlbedo.Sample(GlobalDefaultSampler, uv) * aInput.color; //NOTE(v9.37.0): Why float4 when we will never use alpha-channel
    const float3 material = GlobalMaterialTexture.Sample(GlobalDefaultSampler, uv).rgb;
    float3 normal = GlobalNormalTexture.Sample(GlobalDefaultSampler, uv).wyz;
    
    const float3 ambientLightColorAndIntensity = float3(1.0f, 1.0f, 1.0f); //NOTE(v9.37.0):  No clue, just put 1.0f for now
    const float3 ambientColor = diffuse.xyz * ambientLightColorAndIntensity;
    
    normal = 2.0f * normal - 1.0f; //NOTE(v9.37.0): Unpacking normal
    normal.z = sqrt(1.0f - saturate(normal.x * normal.x + normal.y * normal.y)); //NOTE(v9.37.0): No clue what this does
    normal = normalize(normal);
    
    float3x3 TBN = float3x3( //NOTE(v9.37.0): Not fully understand this either.
        normalize(aInput.tangent.xyz),
        normalize(-aInput.bitangent.xyz),
        normalize(aInput.normal.xyz)
    );
    
    TBN = transpose(TBN); //NOTE(v9.37.0): Why transpose? NOTE(v10.0.1): Inverse of TBN matrix is tranpose version of it and transpose is faster than inverse
    const float3 pixelNormal = normalize(mul(TBN, normal)); //NOTE(v9.37.0): What is PixelNormal?
    
    GBufferOutput output;
    
    output.position = aInput.worldPosition;
    output.albedo = float4(ambientColor, diffuse.w);;
    output.material = float4(material, diffuse.w);;
    output.normal = float4(0.5f + 0.5f * pixelNormal, 1.f); //NOTE(v9.37.0): Packing normal to between 0 and 1
    output.ambientOcclusionAndCustom.r = GlobalNormalTexture.Sample(GlobalDefaultSampler, uv).z;
    output.ambientOcclusionAndCustom.gba = aInput.normal.xyz;
    
    return output;
}
