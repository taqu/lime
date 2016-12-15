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
    float3 normal : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
    float3 viewPos : TEXCOORD2;
    float2 velocity : TEXCOORD3;
};

VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;

    float4 position = float4(input.position, 1);

    float4x4 mwvp0 = mul(mworld0, mvp0);
    float4x4 mwvp1 = mul(mworld1, mvp1);
    float4x4 mwv1 = mul(mworld1, mview);
    float4 position0 = mul(position, mwvp0);
    float4 position1 = mul(position, mwvp1);

    output.position = position1;
    output.normal = mul(input.normal.xyz, (float3x3)mworld1);
    output.worldPos = mul(position, mworld1).xyz;
    output.viewPos = mul(position, mwv1).xyz;
    output.velocity = calcVelocity(position0, position1);
    return output;
}
