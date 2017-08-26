#include "Common.vs"

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

VSOutput main(uint id : SV_VertexID) 
{
    VSOutput output;
    output.tex = float2((id<<1)&0x02U, id&0x02U);
    output.position = float4(output.tex * float2(2,-2) + float2(-1, 1), 1.0e-6, 1);
    return output;
}
