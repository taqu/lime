#include "Common.vs"

//--------------------------------
struct VSInput
{
    float3 position : POSITION;
    float4 normal : NORMAL;
    //float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float3 position : POSITION;
    float3 normal : TEXCOORD0;
    //float2 uv : TEXCOORD1;
};

VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;

    float4 position = float4(input.position, 1);
    output.position = mul(position, mworld1).xyz;
    output.normal = mul(input.normal, (float3x3)mworld1);
    //output.uv = input.uv;
    return output;
}
