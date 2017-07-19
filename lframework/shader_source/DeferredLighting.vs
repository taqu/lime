#include "Common.vs"

//--------------------------------
cbuffer LightMatrices : register(b6)
{
    float4 light_matrices[MAX_POINT_LIGHTS*3];
}

struct VSInput
{
    float3 position : POSITION;
    uint instanceId : SV_InstanceID;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    uint id : SV_InstanceID;
};

VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;

    uint index = 3*input.instanceId;
    float4x4 mw;
    mw[0] = light_matrices[index+0];
    mw[1] = light_matrices[index+1];
    mw[2] = light_matrices[index+2];
    mw[3] = float4(0,0,0,1);
    float4x4 mwvp = mul(transpose(mw), mvp1);

    float4 position = float4(input.position, 1);
    output.position = mul(position, mwvp);
    float2 texcoord = (output.position.xy/output.position.w);
    output.tex = texcoord*float2(0.5, -0.5) + float2(0.5,0.5);
    output.id = input.instanceId;
    return output;
}
