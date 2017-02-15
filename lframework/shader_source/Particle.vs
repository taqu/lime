#include "Common.vs"

struct VSInput
{
    float3 position : POSITION;
    float4 texcoord : TEXCOORD;
    float4 color : COLOR;
    float2 size : SIZE;
    float2 rotation : ROTATION;
};

struct VSOutput
{
    float3 position : POSITION;
    float4 texcoord : TEXCOORD;
    float4 color : COLOR;
    float2 size : SIZE;
    float4x4 mat : MATRIX;
};

#if 0
void getBGRAAndParam(out float4 color, out uint param, float x)
{
    static const float Ratio = 1.0/254.0;
    uint i = asuint(x);

    uint ub = (i>>0)&0xFF;
    uint ug = (i>>8)&0xFF;
    uint ur = (i>>16)&0xFF;
    uint ua = (i>>24)&0xFF;
    color.b = Ratio * (ub&0xFE);
    color.g = Ratio * (ug&0xFE);
    color.r = Ratio * (ur&0xFE);
    color.a = Ratio * (ua&0xFE);
    param = ((ua&0x01)<<3) | ((ur&0x01)<<2) | ((ug&0x01)<<1) | (ub&0x01);
}
#endif

VSOutput main( VSInput input )
{
    float sn = input.rotation.x;
    float cs = input.rotation.y;
    float4x4 mat =
    {
        cs,  sn, 0, 0,
        -sn,  cs, 0, 0,
         0,   0, 1, 0,
         0,   0, 0, 1,
    };

    mat = mul(mat, minvview);

    mat[3][0] = input.position.x;
    mat[3][1] = input.position.y;
    mat[3][2] = input.position.z;

    VSOutput output;
    output.position = input.position;
    output.texcoord = input.texcoord;
    output.color = input.color;
    output.size = input.size * 0.5;
    output.mat = mat;

    return output;
}
