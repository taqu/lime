cbuffer ConstantBuffer0 : register(b0)
{
    float4x4 mvp;
    float4x4 iview;
}

struct VSOutput
{
    float3 position : POSITION;
    float4 texcoord : TEXCOORD;
    float4 color : COLOR;
    float2 size : SIZE;
    float4 params : PARAMS;
    float4x4 mat : MATRIX;
};

struct GSOutput
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    nointerpolation float4 params : PARAMS;
};

[maxvertexcount(4)]
void main(point VSOutput input[1], inout TriangleStream<GSOutput> outputStream)
{
    float4 pos[4];
    pos[0] = float4(-input[0].size.x,  input[0].size.y, 0, 1);
    pos[1] = float4( input[0].size.x,  input[0].size.y, 0, 1);
    pos[2] = float4(-input[0].size.x, -input[0].size.y, 0, 1);
    pos[3] = float4( input[0].size.x, -input[0].size.y, 0, 1);

    float2 texcoords[4] =
    {
        input[0].texcoord.xy,
        input[0].texcoord.zy,
        input[0].texcoord.xw,
        input[0].texcoord.zw,
    };

    GSOutput output;
    output.color = input[0].color;
    output.params = input[0].params;
    for(int i=0; i<4; ++i){
        float4 position = mul(pos[i], input[0].mat);
        output.position = mul(position, mvp);
        output.texcoord = texcoords[i];
        outputStream.Append(output);
    }
    outputStream.RestartStrip();
}
