#ifndef INC_VOLUME_INC_
#define INC_VOLUME_INC_

#define NUM_CASCADES (2)

struct VSInOut
{
    nointerpolation float3 position : POSITION;
    nointerpolation float2 timescale : TIMESCALE; //time, scale
    nointerpolation float2 radius : RADIUS; //radius, invRadius
};

struct DSOutput
{
    float4 position : SV_POSITION;
    float3 worldPos : POSITION;
    nointerpolation float2 timescale : TIMESCALE;
    nointerpolation float2 radius : RADIUS;
    noperspective float2 rayHitNearFar : RAYHIT;
    noperspective float3 rayDirection : RAYDIR;
};

struct NoiseConstant
{
    int noiseOctaves_;
    float3 noiseAnimation_;

    float noiseScale_;
    float noiseAmplitude_;
    float noisePersistence_;
    float noiseFrequency_;
};

#ifndef DEF_PERFRAMECONSTANT
#define DEF_PERFRAMECONSTANT
cbuffer PerFrameConstant : register(b0)
{
    float velocityScale;
    float velocityMaxMagnitude;
    float reserved0;
    float reserved1;
};
#endif

#ifndef DEF_PERCAMERACONSTANT
#define DEF_PERCAMERACONSTANT
cbuffer PerCameraConstant : register(b2)
{
    float4x4 mview;
    float4x4 minvview;
    float4x4 mproj;
    float4x4 minvproj;
    float4x4 mvp0; //previous vew projection
    float4x4 mvp1; //current view projection
    float4 cameraPos;
    int2 screenSize; //(screenWidth, screenHeight)
    float2 screenInvSize; //(screenInvWidth, screenInvHeight)
}
#endif

cbuffer VolumeConstant : register(b3)
{
    int maxSteps_;
    float stepSize_;
    float uvExtent_;
    float uvDisplacement_;

    float skinThickness_;
    float displacement_;
    float edgeSoftness_;
    float opacity_;

    NoiseConstant noise_;
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
