#include "Common.vs"

struct VSOutput
{
    float3 position : POSITION;
    float4 texcoord : TEXCOORD;
    float4 color : COLOR;
    float2 size : SIZE;
    float4x4 mat : MATRIX;
};

struct GSOutput
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float4 texcoord : TEXCOORD0;
    float depth : TEXCOORD1;
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
    float4x4 mvp = mul(input[0].mat, mvp1);
    for(int i=0; i<4; ++i){
        float4 position = mul(pos[i], mvp);
        output.position = position;
        output.texcoord.xy = texcoords[i];
        output.texcoord.zw = output.position.xy/output.position.w;
        output.depth = position.z/position.w;//distance(position.xyz, cameraPos.xyz);
        outputStream.Append(output);
    }
    outputStream.RestartStrip();
}
