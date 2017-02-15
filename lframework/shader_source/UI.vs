
cbuffer PerModelConstant0 : register(b0)
{
    float4x4 mproj;
};

struct VSInput
{
    float2 position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};
            
struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};
            
PSInput main(VSInput input)
{
    PSInput output;
    output.position = mul(mproj, float4(input.position.xy, 0.0f, 1.0f));
    output.texcoord  = input.texcoord;
    output.color = input.color;
    return output;
}
