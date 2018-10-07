#ifndef INC_VOLUME_INC_
#define INC_VOLUME_INC_
#include "Constants.inc"

struct VSInOut
{
    nointerpolation float3 position : POSITION;
    nointerpolation float2 timescale : TIMESCALE; //time, scale
    nointerpolation float2 radius : RADIUS; //radius, frequency scale
};

struct DSOutput
{
    float4 position : SV_POSITION;
    float3 worldPos : POSITION;
    float2 texcoord : TEXCOORD0;
    nointerpolation float2 timescale : TIMESCALE;
    nointerpolation float2 radius : RADIUS;
    noperspective float2 rayHitNearFar : RAYHIT;
    noperspective float3 rayDirection : RAYDIR;
};

cbuffer HullConstant : register(b4)
{
    float tesselationFactor;
    float minDistance;
    float invRange;
    float tesselationMinFactor;
};

static const float3 CoordVectorTable[]=
{
    float3(1,0,0),
    float3(0,1,0),
    float3(0,0,1),
};

void calcCoordinate(out float3 u, out float3 v, float3 dir)
{
    float3 d = abs(dir);
    int d0 = (d[0] < d[1])? 0 : 1;
    d0 = (d[d0] < d[2])? d0 : 2;
    v = CoordVectorTable[d0];
    u = normalize(cross(v, dir));
    v = normalize(cross(dir, u));
}
#endif //INC_VOLUME_INC_
