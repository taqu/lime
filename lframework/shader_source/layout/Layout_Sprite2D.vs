struct VSInput
{
    float2 position : POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

float4 main(VSInput input) : SV_POSITION
{
    return float4(input.position, 0, 1);
}
