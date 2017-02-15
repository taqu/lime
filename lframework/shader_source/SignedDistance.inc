#ifndef INC_SIGNEDDISTANCE_INC_
#define INC_SIGNEDDISTANCE_INC_

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
    float4 c;
    c.rgb = src.a*src.rgb + (1-src.a)*dst.rgb;
    c.a = src.a + dst.a;
    return c;
}

float4 blendAdd(float4 src, float4 dst)
{
    return dst + float4(src.rgb*src.a, src.a);
}

float sampleNoise(float3 uvw)
{
    return texNoiseVolume.SampleLevel(samLinearWrap, uvw, 0);
}

float fractalNoise(float3 pos, int octaves, float time, in const NoiseConstant noise)
{
    float3 animation = noise.noiseAnimation_ * time;
    float3 uvw = pos * noise.noiseScale_ + animation;
    float amplitude = noise.noiseAmplitude_;
    float noiseValue = 0;
    for(int i=0; i<octaves; ++i){
        noiseValue += amplitude * abs(sampleNoise(uvw));
        amplitude *= noise.noisePersistence_;
        uvw *= noise.noiseFrequency_;
    }
    return noiseValue;
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
    float invRadius,
    float displacement,
    int octaves,
    float time,
    in const NoiseConstant noise)
{
    float outDisplacement = fractalNoise(pos, octaves, time, noise) * displacement * radius;
    return outDisplacement;
    float signedDistance = sphere(pos-spherePos, radius) + outDisplacement;
    return signedDistance * invRadius;
}

#endif //INC_SIGNEDDISTANCE_INC_
