#include "Common.vs"

//--------------------------------
struct VSInput
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
    uint2 bone : BLENDINDICES;
    float2 weight : BLENDWEIGHT;
    uint instanceId : SV_InstanceID;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    uint instanceId : SV_InstanceID;
};

VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;
    float3x4 finalMatrix = transformMatrix(input.bone, input.weight, matrices1);
    float4 position = float4(mul(finalMatrix, float4(input.position, 1)), 1);

    float4x4 mwvp = mul(mworld1, mlvp[input.instanceId]);
    position = mul(position, mwvp);

    output.position = position;
    output.instanceId = input.instanceId;
    return output;
}
