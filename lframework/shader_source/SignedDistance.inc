#ifndef INC_SIGNEDDISTANCE_INC_
#define INC_SIGNEDDISTANCE_INC_

//Signed distance functions
//------------------------------------------------------
float sdSphere(float3 pos, float radius)
{
    return length(pos) - radius;
}

float udBox(float3 pos, float3 size)
{
    return length(max(abs(pos)-size, 0));
}

float udRoundBox(float3 pos, float3 size, float r)
{
    return length(max(abs(pos)-size, 0))-r;
}

float sdBox(float3 pos, float3 size)
{
    float3 d = abs(pos) - size;
    return min(max(d.x, max(d.y, d.z)), 0) + length(max(d, 0));
}

float sdTorus(float3 pos, float2 t)
{
    float2 q = float2(length(pos.xz)-t.x, pos.y);
    return length(q) - t.y;
}

float sdCylinder(float3 pos, float3 c)
{
    return length(pos.xz - c.xy) - c.z;
}

/// c --- normalized
float sdCone(float3 pos, float2 c)
{
    float q = length(pos.xy);
    return dot(c, float2(q,pos.z));
}

/// n --- normalized
float sdPlane(float3 pos, float4 p)
{
    return dot(pos, p.xyz) + p.w;
}

float sdHexPrism(float3 pos, float2 h)
{
    float3 q =abs(pos);
    return max(q.z-h.y, max((q.x*0.866025+q.y*0.5), q.y)-h.x);
}

float sdCapsule(float3 pos, float3 a, float3 b, float r)
{
    float3 pa = pos-a;
    float3 ba = b-a;
    float h = clamp(dot(pa,ba)/dot(ba,ba), 0, 1);
    return length(pa-ba*h)-r;
}

float sdCappedCylinder(float3 pos, float2 c)
{
    float2 d = abs(float2(length(pos.xz),c.y)) - c;
    return min(max(d.x,d.y),0) + length(max(d,0));
}

float sdCappedCone(float3 pos, float3 c)
{
    float2 q = float2(length(pos.xz), pos.y);
    float2 v = float2(c.z*c.y/c.x, -c.z);
    float2 w = v-q;
    float2 vv = float2(dot(v,v), v.x*v.x);
    float2 qv = float2(dot(v,w), v.x*w.x);
    float2 d = max(qv, 0)*qv/vv;
    return sqrt(dot(w,w) - max(d.x,d.y)) * sign(max(q.y*v.x - q.x*v.y, w.y));
}

float sdEllipsoid(float3 pos, float3 r)
{
    return (length(pos/r) - 1) * min(min(r.x,r.y),r.z);
}

float opUnion(float d0, float d1)
{
    return min(d0,d1);
}

float opSub(float d0, float d1)
{
    return max(-d0,d1);
}

float opIntersect(float d0, float d1)
{
    return max(d0, d1);
}

#if 0
float opRepeat(float3 p, float3 c)
{
    float3 q = mod(p,c) - 0.5*c;
    return primitive(q);
}

float3 opRotTrans(float3 p, float4x4 m)
{
    float3 q = p*m;
    return ptimitive(q);
}

float opScale(float3 p, float s)
{
    return primitive(p/s)*s;
}

float opTwist(float3 p)
{
    float c = cos(20.0*p.y);
    float s = sin(20.0*p.y);
    float2x2 = float2x2(c,-s,s,c);
    float3 q = float3(m*p.xz, p.y);
    return primitive(q);
}

float opCheapBend(float3 p)
{
    float c = cos(20.0*p.y);
    float s = sin(20.0*p.y);
    float2x2 = float2x2(c,-s,s,c);
    float3 q = float3(m*p.xy, p.z);
    return primitive(q);
}
#endif

float4 blend(float4 base, float4 color)
{
    //return base + color*(1.0 - base.a);
    return mad(float4(color.rgb, 1), mad(color.a, -base.a, color.a), base);
}

float4 blendAlpha(float4 base, float4 color)
{
    float4 c;
    c.rgb = base.rgb*(1.0-color.a) + color.rgb * color.a;
    c.a = min(base.a + color.a, 1);
    return c;
}

float4 blendAdd(float4 src, float4 dst)
{
    return dst + float4(src.rgb*src.a, src.a);
}

#if 0
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
#endif

#endif //INC_SIGNEDDISTANCE_INC_
