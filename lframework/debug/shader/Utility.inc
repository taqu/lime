#ifndef INC_UTILITY_INC_
#define INC_UTILITY_INC_
#include "Constants.inc"

static const float PI = 3.14159265;
static const float PI2 = 6.2831853;
static const float INV_PI = 0.31830988618379067153;

//Color space conversion
//--------------------------
// http://chilliant.blogspot.com.au/2012/08/srgb-approximations-for-hlsl.html?m=1
float3 gammaToLinear(float3 srgb)
{
    return srgb*(0.012522878 + srgb*(0.682171111 + srgb*0.305306011));
}

float3 linearToGamma(float3 linearC)
{
    return max(1.055 * pow(max(linearC,float3(0,0,0)), 0.416666667) - 0.055, 0);
}

#if 0
//=================================================================================================
//
//  Baking Lab
//  by MJP and David Neubelt
//  http://mynameismjp.wordpress.com/
//
//  All code licensed under the MIT license
//
//=================================================================================================

// The code in this file was originally written by Stephen Hill (@self_shadow), who deserves all
// credit for coming up with this fit and implementing it. Buy him a beer next time you see him. :)

// sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
static const float3x3 ACESInputMat =
{
    {0.59719, 0.35458, 0.04823},
    {0.07600, 0.90834, 0.01566},
    {0.02840, 0.13383, 0.83777}
};

// ODT_SAT => XYZ => D60_2_D65 => sRGB
static const float3x3 ACESOutputMat =
{
    { 1.60475, -0.53108, -0.07367},
    {-0.10208,  1.10813, -0.00605},
    {-0.00327, -0.07276,  1.07602}
};

float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float3 ACESFitted(float3 color)
{
    color = mul(ACESInputMat, color);

    // Apply RRT and ODT
    color = RRTAndODTFit(color);

    color = mul(ACESOutputMat, color);

    // Clamp to [0, 1]
    color = saturate(color);

    return color;
}
#endif

float3 tonemapReinhard(float3 x)
{
    return pow(x/(1.0+x), 1.0/2.2);
}

//includes pow(x, 1/2.2)
float3 tonemapFilimic(float3 x)
{
    x = max(0, x-0.004);
    return (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
}

//includes pow(x, 1/2.2)
float3 tonemapACESFilm(float3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return pow(saturate((x*(a*x+b))/(x*(c*x+d)+e)), 1/2.2);
}

//includes pow(x, 1/2.2)
//----------------------------
float3 helperUncharted2(float3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float helperUncharted2(float x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float3 tonemapUncharted2(float3 x)
{
   float W = 11.2; //white point
   return pow(saturate(helperUncharted2(x*16)/helperUncharted2(W)), 1/2.2);
}

float2 encode8bit(float x)
{
    float2 v = frac(float2(x, x*255.0));
    v.x -= v.y*(1.0/255.0);
    return v;
}

float decode8bit(float2 v)
{
    return dot(v, float2(1, 1.0/255.0));
}

float2 encode16bit(float x)
{
    float2 v = frac(float2(x, x*65535.0));
    v.x -= v.y*(1.0/65535.0);
    return v;
}

float decode16bit(float2 v)
{
    return dot(v, float2(1, 1.0/65535.0));
}

float2 lightSpaceUV(in float2 uv)
{
    return uv*float2(0.5,-0.5)+float2(0.5,0.5);
}

float2 UVToScreen(in float2 uv)
{
    return uv*float2(2.0,-2.0)+float2(-1.0,1.0); 
}

float3 screenToView(float3 screen, float4x4 invproj)
{
    float4 view = mul(float4(screen, 1), invproj);
    return view.xyz/view.w;
}

float3 viewToWorld(float3 view, float4x4 invview)
{
    return mul(float4(view,1), invview).xyz;
}

float3 screenToWorld(float3 screen, float4x4 invvp)
{
    float4 world = mul(float4(screen,1), invvp);
    return world.xyz/world.w;
}

float2 encodeVelocity(float2 velocity)
{
    return velocity;
}

float2 decodeVelocity(float2 encoded)
{
    return encoded;
}

float toLinearZ(in float depth, in float4 param)
{
    return param.z/(param.x*depth - param.y) + param.w;
}

float toDistance(in float depth, in float4 param)
{
    return (param.z/(param.x*depth - param.y) + param.w-1) * param.x + param.y;
}

float linearZToDistance(in float z, in float4 param)
{
    return (z-1)*param.x + param.y;
}

#if 0
//Stereographic projection
float2 encodeNormal(in float3 n)
{
    const float scale = 1.7777;
    float2 enc = n.xy/(n.z+1.0);
    enc /= scale;
    enc = enc*0.5+0.5;
    return enc;
}

float3 decodeNormal(in float2 enc)
{
    const float scale = 1.7777;
    float3 nn;
    nn.xy = enc * scale * 2.0 - float2(scale, scale);
    nn.z = 1;
    float g = 2.0 / dot(nn.xyz,nn.xyz);
    float3 n;
    n.xy = g*nn.xy;
    n.z = g-1;
    return n;
}
#else
// Spheremap transform
float2 encodeNormal(in float3 n)
{
    float f = sqrt(n.z*8.0 + 8.0);
    return n.xy / f + 0.5;
}

float3 decodeNormal(in float2 enc)
{
    float2 fenc = enc*4.0 - 2.0;
    float f = dot(fenc,fenc);
    float g = sqrt(1-f/4.0);
    float3 n;
    n.xy = fenc*g;
    n.z = 1.0 - f/2.0;
    return n;
}
#endif

uint hashUInt(in float3 wpos, in float2 uv) 
{
    float2 xy = wpos.xy * uv;
    uint n = asuint(xy.x+xy.y);
	n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    return n;
}

uint hashUInt(in float2 uv) 
{
    uint n = asuint(uv.x+uv.y);
	n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    return n;
}

float hashFloat(in float3 wpos, in float2 uv) 
{
    float2 xy = wpos.xy * uv;
    uint n = asuint(xy.x+xy.y);
	n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    return asfloat( (n>>9U) | 0x3f800000U ) - 1.0;
}

void calcShadowTexcoord(out float4 texS[NUM_CASCADES], in float4 worldPos, in float4x4 lvp[NUM_CASCADES])
{
    [unroll(NUM_CASCADES)]
    for(int i=0; i<NUM_CASCADES; ++i){
        texS[i] = mul(worldPos, lvp[i]);
    }
}

#endif //INC_UTILITY_INC_
