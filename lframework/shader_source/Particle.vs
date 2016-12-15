cbuffer ConstantBuffer1 : register(b1)
{
    float4x4 mvp;
    float4x4 iview;
}

struct VSInput
{
    float3 position : POSITION;
    float4 texcoord : TEXCOORD;
    float4 color : COLOR;
    float2 size : SIZE;
    float2 rotation : ROTATION;
    float4 params : PARAMS;
};

struct VSOutput
{
    float3 position : POSITION;
    float4 texcoord : TEXCOORD;
    float4 color : COLOR;
    float2 size : SIZE;
    nointerpolation float4 params : PARAMS;
    float4x4 mat : MATRIX;
};

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
    mat = mul(mat, iview);
    mat[3][0] = input.position.x;
    mat[3][1] = input.position.y;
    mat[3][2] = input.position.z;

    VSOutput output;
    output.position = input.position;
    output.texcoord = input.texcoord;
    output.color = input.color;
    output.size = input.size * 0.5;
    output.params.x = asfloat((int)(input.params.x*255));
    output.params.yzw = input.params.yzw;
    output.mat = mat;

    return output;
}
