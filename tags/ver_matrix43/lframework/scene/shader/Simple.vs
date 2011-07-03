static const float PI = 3.14159265358979323846;
static const float PI2 = 6.28318530717958647692;
static const float INV_PI = 0.31830988618379067153;
static const float INV_PI2 = 0.15915494309189533576;
static const int c_izero = 0;
static const int c_ione = 1;
static const int c_itwo = 2;
static const int c_ithree = 3;
static const float f_1_100 = 0.01;

uniform float4x4 mwvp; //World * View * Projection Matrix

#if defined(VDIFFUSE) && defined(PCOLOR)

uniform float3 dlDir;
uniform float4 dlColor;

//マテリアル設定
uniform float3 ambient;

#endif

#ifdef SKINNING

#define NUM_PALETTE_MATRICES 80
uniform float4x3 palette[NUM_PALETTE_MATRICES];

void skinning(uniform float4 position, uniform float3 normal, out float3 retPosition, out float3 retNormal, uniform float4 indices)
{
    float weight = indices.z * f_1_100;
    int index = int(indices.x);

    retPosition = mul(position * palette[index]) * weight;
    retNormal = mul(normal * palette[index]) * weight;

    weight = float(c_ione) - weight;
    index = int(indices.y);
    retPosition += mul(position * palette[index]) * weight;
    retNormal += mul(normal * palette[index]) * weight;
}
#endif

//--------------------------------
struct VSInput
{
    float4 position    : POSITION;

#ifdef VNORMAL
    float3 normal      : NORMAL;
#endif

#ifdef VTEX0
    float2 tex0   : TEXCOORD0;
#endif

#ifdef VTEX1
    float2 tex1   : TEXCOORD1;
#endif

#ifdef VBONE
    float4 bones       : BLENDINDICES;
#endif
};

struct VS_OUTPUT
{
    float4 position : POSITION0;

#ifdef PNORMAL
    float3 normal : TEXCOORD1;
#endif

#ifdef PCOLOR
    float3 color : COLOR0;
#endif

#ifdef PTEX0
    float2 tex0 : TEXCOORD0;
#endif

#ifdef PPOS
    float3 pos0 : TEXCOORD2;
#endif
};


VS_OUTPUT main(VSInput input)
{
    VS_OUTPUT output= (VS_OUTPUT)0;

    float3 position;
    float3 normal;
#ifdef SKINNING
    skinning(input.position, input.normal, position, normal, input.bones);
#else
    position = input.position.xyz;
#ifdef V_NORMAL
    normal = input.normal;
#endif
#endif

#ifdef PPOS
    output.pos0 = position;
#endif

//linear-z or non-linear-z
#if 1
    float4 vpos = mul(float4(position.xyz, 1.0f), mwvp);
    vpos.z *= vpos.w;
    output.position = vpos;
#else
    output.position = mul(float4(position.xyz, 1.0f), mwvp);
#endif

#ifdef PNORMAL
    output.normal = normal;
#endif

#ifdef PTEX0
    output.tex0 = input.tex0;
#endif

#if defined(VDIFFUSE) && defined(PCOLOR)
    // ライティング計算
    float3 L = dlDir;
    output.color = ambient + dlColor.w * max(dlColor.xyz * dot(normal, L), 0.0f);//Irradiance。たぶんこんな感じ
#endif

    return output;
}
