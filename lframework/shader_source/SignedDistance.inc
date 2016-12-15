
Texture3D<float> texNoiseVolume : register(t0);
SamplerState samLinearWrap: register(s0);

//Signed distance functions
//------------------------------------------------------
float sphere(float3 pos, float radius)
{
    return length(pos) - radius;
}

float box(float3 pos, float3 size)
{
    float3 d = abs(pos) - size;
    return min(max(d.x, max(d.y, d.z)), 0) + length( max(d, 0));
}

float coneY(float3 pos, float radius)
{
    float d = length(pos.xz) - lerp(radius*0.5, 0, (radius+pos.y)/radius);
    d = max(d, -pos.y - radius);
    d = max(d, pos.y - radius);
    return d;
}

float cylinder(float3 pos, float radius)
{
    float2 h = radius.xx * float2(0.7f, 1);
    float2 d = abs(float2(length(pos.xz), pos.y)) - h;
}

float torus(float3 pos, float radius)
{
    float2 t = radius * float2(1, 0.01);
    float2 q = float2(length(pos.xz) - t.x, pos.y);
    return length(q) - t.y;
}

float4 blend(float4 src, float4 dst)
{
    return mad(float4(src.rgb, 1), mad(src.a, -dst.a, src.a), dst);
}

float4 blendAdd(float4 src, float4 dst)
{
    return dst + float4(src.rgb*src.a, src.a);
}

float sampleNoise(float3 uvw)
{
    return texNoiseVolume.SampleLevel(samLinearWrap, uvw, 0);
}

float fractalNoise(float3 pos, int octaves, float time, in const VolumeConstant volumeConstant)
{
    float3 animation = volumeConstant.noiseAnimation_ * time;
    float3 uvw = pos * volumeConstant.noiseScale_ + animation;
    float amplitude = volumeConstant.noiseAmplitude_;
    float noiseValue = 0;
    for(int i=0; i<octaves; ++i){
        noiseValue += abs(amplitude * sampleNoise(uvw));
        amplitude *= volumeConstant.noisePersistence_;
        uvw *= volumeConstant.noiseFrequency_;
    }
    return noiseValue * volumeConstant.invMaxNoise_;
}

float signedDistancePrimitive(float3 pos, float3 spherePos, float radius)
{
    float3 diff = pos-spherePos;
    float signedDistance = sphere(diff, radius);
    return signedDistance;
}

float signedDistancePrimitiveDisplaced(
    float3 pos,
    float3 spherePos,
    float radius,
    int octaves,
    float time,
    in const VolumeConstant volumeConstant,
    out float outDisplacement)
{
    float3 diff = pos-spherePos;
    outDisplacement = fractalNoise(pos, octaves, time, volumeConstant);

    float signedDistance = sphere(diff, radius);
    return signedDistance - outDisplacement * volumeConstant.displacement_;
}
