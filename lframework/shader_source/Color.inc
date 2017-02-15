#ifndef INC_COLOR_INC_
#define INC_COLOR_INC_

//static const float3 RGB2Y  = float3( 0.299, 0.587, 0.114);
//static const float3 RGB2Cb = float3(-0.16874, -0.33126, 0.5);
//static const float3 RGB2Cr = float3( 0.5, -0.41869, -0.08131);

//static const float3 YCbCr2R = float3(1, 0,        1.402);
//static const float3 YCbCr2G = float3(1,-0.34414, -0.71414);
//static const float3 YCbCr2B = float3(1, 1.772,    0);

static const float ColorEpsilon = 1.0e-10;

float3 HueToRGB(float H)
{
    H *= 6;
    float R = abs(H-3)-1;
    float G = 2 - abs(H-2);
    float B = 2 - abs(H-4);
    return saturate(float3(R,G,B));
}

float3 RGBToHCV(in float3 RGB)
{
    float4 P = (RGB.g < RGB.b)? float4(RGB.bg, -1.0, 2.0/3.0) : float4(RGB.gb, 0.0, -1.0/3.0);
    float4 Q = (RGB.r < P.x)? float4(P.xyw, RGB.r) : float4(RGB.r, P.yzx);
    float C = Q.x - min(Q.w, Q.y);
    float H = abs((Q.w-Q.y)/(6*C + ColorEpsilon) + Q.z);
    return float3(H,C,Q.x);
}

float3 HSVToRGB(in float3 HSV)
{
    float3 RGB = HueToRGB(HSV.x);
    return ((RGB-1)) * HSV.y + 1) * HSV.z;
}

float3 RGBToHSV(in float3 RGB)
{
    float3 HCV = RGBToHCV(RGB);
    float L = HCV.z - HCV.y*0.5;
    float S = HCV.y/(1 - abs(L*2-1) + ColorEpsilon);
    return float3(HCV.x, S, L);
}

float3 RGBToXYZ(in float3 RGB)
{
    float X = dot(RGB, float3(0.4898, 0.3101, 0.2001));
    float Y = dot(RGB, float3(0.1769, 0.8124, 0.0107));
    float Z = dot(RGB, float3(0.0000, 0.0100, 0.9903));
    return float3(X,Y,Z);
}

float3 XYZToRGB(in float3 RGB)
{
    float R = dot(RGB, float3( 2.3655, -0.8971, -0.4683));
    float G = dot(RGB, float3(-0.5151,  1.4264,  0.0887));
    float B = dot(RGB, float3( 0.0052, -0.0144,  1.0089));
    return float3(R,G,B);
}

float RGBToGray(in float3 RGB)
{
    return dot(RGB, float3(0.299, 0.587, 0.114));
}
#endif //INC_COLOR_INC
