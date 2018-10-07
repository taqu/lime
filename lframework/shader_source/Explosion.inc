
struct InOut
{
    nointerpolation float3 position : POSITION;
    nointerpolation float2 timescale : TIMESCALE;
};

struct DSOutput
{
    float4 position : SV_POSITION;
    float3 worldPos : POSITION;
    nointerpolation float2 timescale : TIMESCALE;
    noperspective float2 rayHitNearFar : RAYHIT;
    noperspective float3 rayDirection : RAYDIR;
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

