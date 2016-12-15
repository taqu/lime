cbuffer ConstantBuffer : register(b0)
{
    float4x4 mwvp;
    float4x4 mw;
}

//--------------------------------
struct VSInput
{
    float3 position : POSITION;
    float2 tex0 : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};

VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;

    float4 position = float4(input.position, 1.0f);
    float4 outPos;

    outPos = mul(position, mwvp);
    output.position = outPos;

    output.tex0 = input.tex0;

    float4 world = mul(position, mw);
    output.worldPos = world.xyz;

    return output;
}
