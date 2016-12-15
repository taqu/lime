cbuffer ConstantBuffer0 : register(b0)
{
    float4 invScreenSize;
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
    float2 scale = float2(invScreenSize.x, invScreenSize.y) * 2;
    position.xy *= scale;

    VSOutput output;
    output.position = position;
    output.color = input.color;
    output.tex0 = input.texcoord;
    return output;
}
