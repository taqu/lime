#include "Common.vs"

//--------------------------------
struct VSInput
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : TEXCOORD0;
    float3 tangent : TEXCOORD1;
    float3 binormal : TEXCOORD2;
    float2 uv : TEXCOORD3;
    float3 worldPos : TEXCOORD4;
    float3 viewPos : TEXCOORD5;
};

VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;

    float4 position = float4(input.position, 1);

    float4x4 mwvp = mul(mworld1, mvp1);
    output.position = mul(position, mwvp);
    output.normal = mul(input.normal.xyz, (float3x3)mworld1);
    output.tangent = mul(input.tangent.xyz, (float3x3)mworld1);
    output.binormal = normalize(cross(output.tangent, output.normal));
    output.uv = input.uv;

    position = mul(position, mworld1);
    output.worldPos = position.xyz;
    output.viewPos.xyz = mul(position, mview).xyz;
    return output;
}
