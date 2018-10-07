#include "Common.vs"

//--------------------------------
struct VSInput
{
    float3 position : POSITION;
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

    float4x4 mwvp = mul(mworld1, mlvp[input.instanceId]);

    output.position = mul(float4(input.position, 1), mwvp);
    output.instanceId = input.instanceId;
    return output;
}
