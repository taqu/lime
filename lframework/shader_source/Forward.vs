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
};

VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;

    float4 position = float4(input.position, 1);

    float4x4 mwvp = mul(mworld1, mvp1);
    output.position = mul(position, mwvp);
    output.normal = mul(input.normal.xyz, (float3x3)mworld1);

    position = mul(position, mworld1);
    output.worldPos = position.xyz;
    output.viewPos.xyz = mul(position, mview).xyz;
    return output;
}
