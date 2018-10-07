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
    float4 texS[NUM_CASCADES] : TEXCOORD3;
};

VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;

    float4 position = float4(input.position, 1);

    float4x4 mwvp = mul(mworld1, mvp1);
    output.position = mul(position, mwvp);
    output.normal.xyz = mul(input.normal.xyz, (float3x3)mworld1);

    float4 worldPos = mul(position, mworld1);
    output.worldPos.xyz = worldPos.xyz;
    //output.worldPos.w = distance(worldPos.xyz, cameraPos.xyz);
    output.viewPos.xyz = mul(worldPos, mview).xyz;
    calcShadowTexcoord(output.texS, worldPos, mlvp);
    return output;
}
