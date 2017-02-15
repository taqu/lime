#include "Common.vs"

//--------------------------------
struct VSInput
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    uint2 bone : BLENDINDICES;
    float2 weight : BLENDWEIGHT;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : TEXCOORD0;
    float2 position1 : TEXCOORD1;
    float2 velocity : TEXCOORD2;
};

VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;

    float3x4 finalMatrix = transformMatrix(input.bone, input.weight, matrices1);

    float4 position = float4(input.position, 1);
    position = float4(mul(finalMatrix, position), 1);
    float3 normal = mul(finalMatrix, input.normal);

    float4x4 mwvp0 = mul(mworld0, mvp0);
    float4x4 mwvp1 = mul(mworld1, mvp1);
    float4 position0 = mul(position, mwvp0);
    float4 position1 = mul(position, mwvp1);

    output.position = position1;
    output.normal = mul(normal, (float3x3)mworld1);
    output.position1 = position1.zw;
    output.velocity.xy = calcVelocity(position0, position1);
    return output;
}
