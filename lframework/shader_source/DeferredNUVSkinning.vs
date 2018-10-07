#include "Common.vs"

//--------------------------------
struct VSInput
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    uint2 bone : BLENDINDICES;
    float2 weight : BLENDWEIGHT;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : TEXCOORD0;
    float3 tangent : TEXCOORD1;
    float3 binormal : TEXCOORD2;
    float2 uv : TEXCOORD3;
    float2 position1 : TEXCOORD4;
    float2 velocity : TEXCOORD5;
};


VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;

    float4 position = float4(input.position, 1);
    float3x4 finalMatrix = transformMatrix(input.bone, input.weight, matrices1);

    position = float4(mul(finalMatrix, position), 1);
    float3 normal = normalize( mul(finalMatrix, input.normal) );
    float3 tangent = normalize( mul(finalMatrix, input.tangent) );

    float4x4 mwvp0 = mul(mworld0, mvp0);
    float4x4 mwvp1 = mul(mworld1, mvp1);
    float4 position0 = mul(position, mwvp0);
    float4 position1 = mul(position, mwvp1);

    output.position = position1;
    output.normal = mul(normal, (float3x3)mworld1);
    output.tangent = mul(tangent, (float3x3)mworld1);
    output.binormal = normalize(cross(output.tangent, output.normal));
    output.uv = input.uv;
    output.position1 = position1.zw;
    output.velocity.xy = calcVelocity(position0, position1);
    return output;
}
