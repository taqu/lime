cbuffer ConstantBuffer0 : register(b0)
{
    float4x4 mwvp;
}

cbuffer ConstantBuffer1 : register(b1)
{
    float4 ratio;
}

struct VSInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 tex0 : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    float4 position = float4(input.position, 1);

    VSOutput output;
    output.position = mul(position, mwvp);
    output.color = input.color;
    output.color.rgb *= ratio.x;
    output.tex0 = input.texcoord;
    return output;
}
