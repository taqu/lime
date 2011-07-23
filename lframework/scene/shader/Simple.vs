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

// 
#ifdef SKINNING

#define NUM_PALETTE_MATRICES 80
uniform float4x3 palette[NUM_PALETTE_MATRICES];

void skinning(uniform float4 position, uniform float3 normal, out float3 retPosition, out float3 retNormal, uniform float4 indices)
{
    float weight = indices.z * f_1_100;
    int index = int(indices.x);

    retPosition = mul(position, palette[index]) * weight;
    retNormal = mul(normal, palette[index]) * weight;

    weight = float(c_ione) - weight;
    index = int(indices.y);
    retPosition += mul(position, palette[index]) * weight;
    retNormal += mul(normal, palette[index]) * weight;
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
    float3 normal : TEXCOORD0;
#endif

#ifdef PCOLOR
    float4 color : COLOR0;
#endif

#ifdef PTEX0
    float4 tex0 : TEXCOORD1;
#endif

#ifdef PPOS
    float3 pos0 : TEXCOORD2;
#endif

#ifdef LIGHTVS
    float4 specular0 : TEXCOORD3;

#ifdef TEXSHADE
    float2 tex2 : TEXCOORD4;
#endif
#endif //LIGHTVS
};


VS_OUTPUT main(VSInput input)
{
    VS_OUTPUT output= (VS_OUTPUT)0;

    float3 position;
    float3 normal;
#ifdef SKINNING
    //position = input.position;
    //normal = input.normal;
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

#ifdef LIGHTVS
    float3 posLighting = position.xyz;
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
    output.tex0.xy = input.tex0;
#endif

#ifdef PTEX1
    output.tex0.zw = input.tex1;
#endif

///////////////////////////////////////////////////
#ifdef LIGHTVS

#ifdef VNORMAL

    float3 N = normalize(normal);
    float3 E = normalize(camPos - posLighting);
    float3 H = normalize(dlDir+E);

    float cosNH = max(c_fzero, dot(N,H));

#ifdef TEXSHADE

    output.tex2 = vec2( (c_fone + dot(N, dlDir))*c_fhalf );

#endif //TEXSHADE
    output.specular0.xyz = specular.xyz * pow(cosNH, specular.w);
    output.specular0.w = cosNH;
#else

    output.specular0.xyz = vec3(c_fzero);
    output.specular0.w = c_fone;
#endif

#endif //LIGHTVS

    return output;
}
