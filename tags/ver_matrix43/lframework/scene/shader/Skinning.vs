static const float PI = 3.14159265358979323846;
static const float PI2 = 6.28318530717958647692;
static const float INV_PI = 0.31830988618379067153;
static const float INV_PI2 = 0.15915494309189533576;

static const int NumBlendBones = 2;

uniform float4x4 mwvp; //World * View * Projection Matrix
uniform float3 dLightDirection;
uniform float4 dLightColor;
//uniform float3 cameraPosition;
//マテリアル設定
uniform float3 ambient;

uniform sampler g_texMatPallet;

struct VSInput
{
    float4 position    : POSITION;
    float3 normal      : NORMAL;
//    float3 tangent     : TANGENT;
    float2 tex0   : TEXCOORD0;
    float4 bones       : BLENDINDICES;
    float weights     : BLENDWEIGHT;
};

struct VS_OUTPUT
{
    float4 position : POSITION0;
    float3 color : COLOR0;
    float3 normal : TEXCOORD0;
    float2 tex0 : TEXCOORD1;
    float3 pos0 : TEXCOORD2;
};

#define MaxMatrixPalletsWidth (256*3)

float4x3 sampleMatrix(float index)
{
    float4x3 mat;
    float offset = (1.0f/MaxMatrixPalletsWidth);
    index *= 3.0f*offset;
    index += 0.5f*offset;
    float4 t = float4(index, 0.5f/MaxMatrixPalletsWidth, 0.0f, 1.0f);
    float4 m0 = tex2Dlod(g_texMatPallet, t);
    mat[0][0] = m0.x; mat[0][1] = m0.y; mat[0][2] = m0.z; mat[1][0] = m0.w;

    t.x += offset;
    float4 m1 = tex2Dlod(g_texMatPallet, t);
    mat[1][1] = m1.x; mat[1][2] = m1.y; mat[2][0] = m1.z; mat[2][1] = m1.w;

    t.x += offset;
    float4 m2 = tex2Dlod(g_texMatPallet, t);
    mat[2][2] = m2.x; mat[3][0] = m2.y; mat[3][1] = m2.z; mat[3][2] = m2.w;

    return mat;
}

VS_OUTPUT main(VSInput input)
{
    VS_OUTPUT output= (VS_OUTPUT)0;

    float3 position = 0.0f;
    float3 normal = 0.0f;
    float4 tmpNormal = float4(input.normal, 1.0f);

    float4x3 mat = sampleMatrix(input.bones.x);
    position += mul(input.position, mat) * input.weights;

    normal += mul(tmpNormal, mat) * input.weights;

    mat = sampleMatrix(input.bones.y);
    float weight2 = 1.0f - input.weights;
    position += mul(input.position, mat) * weight2;
    normal += mul(tmpNormal, mat) * weight2;

    normal = normalize(normal);

    //normal = input.normal;
    //position = input.position;

    output.position = mul(float4(position, 1.0f), mwvp);
    output.normal = normal.xyz;
    output.tex0 = input.tex0;
    output.pos0 = position.xyz;

    // ライティング計算
    float3 L = -dLightDirection;
    output.color = ambient + dLightColor.w * max(dLightColor.xyz * dot(normal, L), 0.0f);//Irradiance。たぶんこんな感じ
    //output.color = float3(mat[0][0], mat[0][1], mat[0][2]);
    //output.color = float3(mat[1][0], mat[1][1], mat[1][2]);
    //output.color = float3(mat[3][0], abs(mat[3][1])/20, mat[3][2]);
    return output;
}
