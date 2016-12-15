struct VSInput
{
    float3 position : POSITION;
    float4 texcoord : TEXCOORD;
    float4 color : COLOR;
    float2 size : SIZE;
    float2 rotation : ROTATION;
    float4 params : PARAMS;
};

float4 main( VSInput input ) : SV_POSITION
{
    return float4(input.position, 1);
}
