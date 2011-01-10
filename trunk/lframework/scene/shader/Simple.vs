static const float PI = 3.14159265358979323846;
static const float PI2 = 6.28318530717958647692;
static const float INV_PI = 0.31830988618379067153;
static const float INV_PI2 = 0.15915494309189533576;
static const int c_izero = 0;
static const int c_ione = 1;
static const int c_itwo = 2;
static const int c_ithree = 3;

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

float4 skinning_position(uniform float4 position, uniform float weight, uniform int index)
{
    float4 tmp;
    //tmp.xyz = mul(position, palette[index]);
    tmp.x = dot(position, palette[index      ]);
    tmp.y = dot(position, palette[index+c_ione]);
    tmp.z = dot(position, palette[index+c_itwo]);
    tmp.w = position.w;

    return (weight * tmp);
}

float3 skinning_normal(uniform float3 normal, uniform float weight, uniform int index)
{
    //float3 tmp = mul(float4(normal,0.0), palette[index]);
    float3 tmp;
    tmp.x = dot(normal, palette[index      ].xyz);
    tmp.y = dot(normal, palette[index+c_ione].xyz);
    tmp.z = dot(normal, palette[index+c_itwo].xyz);
    return (weight * tmp);
}

void skinning(in float4 position, in float3 normal, out float4 retPosition, out float3 retNormal, uniform float4 indices, uniform float weight)
{
    retPosition = (float4)float(c_izero);
    retNormal = (float3)float(c_izero);

    int index = int(indices.x) * c_ithree;
    retPosition += skinning_position(position, weight, index);
    retNormal += skinning_normal(normal, weight, index);

    float weight1 = float(c_ione) - weight;
    index = int(indices.y) * c_ithree;
    retPosition += skinning_position(position, weight1, index);
    retNormal += skinning_normal(normal, weight1, index);
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

#ifdef VBONE
    float4 bones       : BLENDINDICES;
#endif

#ifdef VWEIGHT
    float weights     : BLENDWEIGHT;
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
    skinning(input.position, input.normal, position, normal, input.bones, input.weights);

#else
    position = input.position;
    normal = input.normal;
#endif
    output.position = mul(position, mwvp);

#ifdef PPOS
    output.pos0 = position.xyz;
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
