struct VS_INPUT
{
    float2 position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

float4 main( VS_INPUT input ) : SV_POSITION
{
    return float4(input.position, 0, 1);
}
