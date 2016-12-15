#define MOTIONVELOCITY
#include "Common.vs"

//--------------------------------
struct VSInput
{
    float3 position : POSITION;
    float4 normal : NORMAL;
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

    float4 position = float4(input.position, 1);

    float4x4 mwvp0 = mul(mworld0, mvp0);
    float4x4 mwvp1 = mul(mworld1, mvp1);
    output.position0 = mul(position, mwvp0);
    output.position = output.position1 = mul(position, mwvp1);
    output.position0 /= output.position0.w;
    output.position1 /= output.position1.w;
    return output;
}
