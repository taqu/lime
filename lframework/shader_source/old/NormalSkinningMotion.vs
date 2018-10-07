#define MOTIONVELOCITY
#include "Common.vs"

//--------------------------------
struct VSInput
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
    uint2 bone : BLENDINDICES;
    float2 weight : BLENDWEIGHT;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 position0 : POSITION0;
    float4 position1 : POSITION1;
};

VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;

    float3x4 finalMatrix0 = transformMatrix(input.bone, input.weight, matrices0);
    float3x4 finalMatrix1 = transformMatrix(input.bone, input.weight, matrices1);

    float4 position0 = float4(mul(finalMatrix0, float4(input.position, 1)), 1);
    float4 position1 = float4(mul(finalMatrix1, float4(input.position, 1)), 1);

    float4x4 mwvp0 = mul(mworld0, mvp0);
    float4x4 mwvp1 = mul(mworld1, mvp1);
    output.position0 = mul(position0, mwvp0);
    output.position = output.position1 = mul(position1, mwvp1);
    output.position0 /= output.position0.w;
    output.position1 /= output.position1.w;
    return output;
}