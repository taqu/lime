
struct InOut
{
    nointerpolation uint vertexID : VERTEXID;
};

struct InOut2
{
    nointerpolation uint vertexID : VERTEXID;
    nointerpolation float3 coordu : COORDU;
    nointerpolation float3 coordv : COORDV;
};

struct DSOutput
{
    float4 position : SV_POSITION;
    float3 worldPos : POSITION;
    nointerpolation float3 vector0 : VECTOR0;
    nointerpolation float time : TIME; 
    nointerpolation int mode : MODE;
    nointerpolation float3 coordu : COORDU;
    nointerpolation float3 coordv : COORDV;
    noperspective float2 rayHitNearFar : RAYHIT;
    noperspective float3 rayDirection : RAYDIR;
};

struct Vertex
{
    float3 position;
    float time;
    float3 vector0;
    int mode;
};

struct VolumeConstant
{
    int hullSteps_;
    int hullOctaves_;
    int maxSteps_;
    float stepSize_;

    float radius_;
    float displacement_;
    float2 uvScaleBias_;

    float edgeSoftness_;
    float3 noiseAnimation_;

    float noiseScale_;
    float noiseAmplitude_;
    float noisePersistence_;
    float noiseFrequency_;

    int noiseOctaves_;
    float invMaxNoise_;
    float opacity_;
    float skinThickness_;
};

StructuredBuffer<Vertex> vertexBuffer : register(t2);

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

