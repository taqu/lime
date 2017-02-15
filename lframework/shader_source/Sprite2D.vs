cbuffer Constant2D : register(b0)
{
    float2 invScreenSize2;
    float2 topLeft;
}

struct VSInput
{
    float2 position : POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    float4 position = float4(input.position, 0, 1);
    position.xy = (position.xy+topLeft)*invScreenSize2 + float2(-1,1);

    VSOutput output;
    output.position = position;
    output.color = input.color;
    output.texcoord = input.texcoord;
    return output;
}
