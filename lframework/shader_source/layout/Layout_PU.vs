struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex0 : TEXCOORD0;
};

float4 main( VS_INPUT input ) : SV_POSITION
{
    return float4(input.position, 1);
}
