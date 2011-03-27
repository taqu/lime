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
uniform float4 palette[NUM_PALETTE_MATRICES*3];

float4 skinning_position(uniform float4 position, uniform int index)
{
    float4 tmp;
    tmp.x = dot(position, palette[index      ]);
    tmp.y = dot(position, palette[index+c_ione]);
    tmp.z = dot(position, palette[index+c_itwo]);
    tmp.w = position.w;

    return tmp;
}

float3 skinning_normal(uniform float3 normal, uniform int index)
{
    float3 tmp;
    tmp.x = dot(normal, palette[index      ].xyz);
    tmp.y = dot(normal, palette[index+c_ione].xyz);
    tmp.z = dot(normal, palette[index+c_itwo].xyz);
    return tmp;
}

void skinning(uniform float4 position, uniform float3 normal, out float4 retPosition, out float3 retNormal, uniform float4 indices)
{
    float weight = indices.z * f_1_100;
    int index = int(indices.x) * c_ithree;

    retPosition = skinning_position(position, index) * weight;
    retNormal = skinning_normal(normal, index) * weight;

    weight = float(c_ione) - weight;
    index = int(indices.y) * c_ithree;
    retPosition += skinning_position(position, index) * weight;
    retNormal += skinning_normal(normal, index) * weight;
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

    float4 position;
    float3 normal;
#ifdef SKINNING
    skinning(input.position, input.normal, position, normal, input.bones);
#else
    position = input.position;
#ifdef V_NORMAL
    normal = input.normal;
#endif
#endif

#ifdef PPOS
    output.pos0 = position.xyz;
#endif

//linear-z or non-linear-z
#if 1
    float4 vpos = mul(position, mwvp);
    vpos.z *= vpos.w;
    output.position = vpos;
#else
    output.position = mul(position, mwvp);
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
