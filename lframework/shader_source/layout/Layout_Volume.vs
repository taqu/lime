struct VS_INPUT
{
    nointerpolation float3 position : POSITION;
    nointerpolation float2 timescale : TIMESCALE;
};

float4 main( VS_INPUT input ) : SV_POSITION
{
    return float4(input.position, 1);
}
