cbuffer ConstantBuffer : register(b0)
{
    float4 pos[4];
    float4 color;
}

struct VSInput
{
    uint vertexID : SV_VertexID;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VSOutput main(VSInput input)
{
    VSOutput output= (VSOutput)0;

    output.position = float4(pos[input.vertexID].xy, 0.0f, 1.0f);
    output.color = color;

    return output;
}
