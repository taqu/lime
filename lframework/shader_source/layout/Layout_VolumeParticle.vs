struct VSInput
{
    nointerpolation float3 position : POSITION;
    nointerpolation float2 timescale : TIMESCALE;
    nointerpolation float2 radius : RADIUS;
};

float4 main(VSInput input) : SV_POSITION
{
    return float4(input.position, 1);
}
