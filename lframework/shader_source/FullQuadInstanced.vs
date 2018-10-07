
//--------------------------------
struct VSInput
{
    uint id : SV_VertexID;
    uint instanceId : SV_InstanceID;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    uint id : SV_InstanceID;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.tex = float2((input.id<<1)&0x02U, input.id&0x02U);
    output.position = float4(output.tex * float2(2,-2) + float2(-1, 1), 1.0e-6, 1);
    output.id = input.instanceId;
    return output;
}
