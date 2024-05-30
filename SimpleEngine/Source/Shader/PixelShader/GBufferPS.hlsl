#include "../Common.hlsli"

GBufferOutput main(PixelInputType aInput)
{
    float2 uv = aInput.uv;
    
    float4 diffuse = GlobalAlbedo.Sample(GlobalDefaultSampler, uv) * aInput.color; //NOTE(v9.37.0): Why float4 when we will never use alpha-channel
    float3 material = GlobalMaterialTexture.Sample(GlobalDefaultSampler, uv).rgb;
    float3 normal = GlobalNormalTexture.Sample(GlobalDefaultSampler, uv).wyz; //NOTE(v9.37.0): Why are we using wyz?
    
    float3 ambientLightColorAndIntensity = float3(1.0f, 1.0f, 1.0f); //NOTE(v9.37.0):  No clue, just put 1.0f for now
    float3 ambientColor = diffuse.xyz * ambientLightColorAndIntensity;
    float3 directColor = float3(0.0f, 0.0f, 0.0f); //NOTE(v9.37.0):  What is purpose of this?
    
    normal = 2.0f * normal - 1.0f; //NOTE(v9.37.0): Unpacking normal
    normal.z = sqrt(1.0f - saturate(normal.x * normal.x + normal.y * normal.y)); //NOTE(v9.37.0): No clue what this does
    normal = normalize(normal);
    
    float3x3 TBN = float3x3( //NOTE(v9.37.0): Not fully understand this either
        normalize(aInput.tangent.xyz),
        normalize(-aInput.bitangent.xyz),
        normalize(aInput.normal.xyz)
    );
    
    TBN = transpose(TBN); //NOTE(v9.37.0): Why transpose?
    float3 pixelNormal = normalize(mul(TBN, normal)); //NOTE(v9.37.0): What is PixelNormal?
    
    GBufferOutput output;
    
    output.position = aInput.worldPosition;
    output.albedo = float4(ambientColor + directColor, diffuse.w);;
    output.material = float4(material, diffuse.w);;
    output.normal = float4(0.5f + 0.5f * pixelNormal, 1.f); //NOTE(v9.37.0): Packing normal to between 0 and 1
    output.ambientOcclusionAndCustom.r = GlobalNormalTexture.Sample(GlobalDefaultSampler, uv).z;
    output.ambientOcclusionAndCustom.gba = aInput.normal.xyz;
    
    return output;
}
