/**
@file Noise.cpp
@author t-sakai
@date 2014/05/08 create
*/
#include "Noise.h"
#include "Random.h"

#include <math.h>
#include <xmmintrin.h> //SSE命令セット
#include <emmintrin.h> //SSE2命令セット

namespace lcore
{
namespace
{
    inline s32 toInt( f32 x )
    {
        return _mm_cvtt_ss2si(_mm_load_ss(&x));
    }
}


#if 0
    //------------------------------------------------
    //---
    //--- Fractal
    //---
    //------------------------------------------------
    f32 Fractal::fbm(const Noise& noise, const Param& param, f32 x, f32 y)
    {
        f32 total = 0.0f;

        f32 frequency = param.frequency_;
        f32 amplitude = 1.0f;
        f32 maxAmplitude = 0.0f;

        for(s32 i=0; i<param.octaves_; ++i){
            total += noise(x*frequency, y*frequency) * amplitude;

            frequency *= param.lacunarity_;
            maxAmplitude += amplitude;
            amplitude *= param.persistence_;
        }
        LASSERT(0.00001f<lcore::absolute(maxAmplitude));

        return total/maxAmplitude;
    }

    f32 Fractal::fbm(const Noise& noise, const Param& param, f32 x, f32 y, f32 z)
    {
        f32 total = 0.0f;

        f32 frequency = param.frequency_;
        f32 amplitude = 1.0f;
        f32 maxAmplitude = 0.0f;

        for(s32 i=0; i<param.octaves_; ++i){
            total += noise(x*frequency, y*frequency, z*frequency) * amplitude;

            frequency *= param.lacunarity_;
            maxAmplitude += amplitude;
            amplitude *= param.persistence_;
        }
        LASSERT(0.00001f<lcore::absolute(maxAmplitude));

        return total/maxAmplitude;
    }

    f32 Fractal::fbmConst(f32 noiseValue, const Param& param)
    {
        f32 total = 0.0f;

        f32 frequency = param.frequency_;
        f32 amplitude = 1.0f;
        f32 maxAmplitude = 0.0f;

        for(s32 i=0; i<param.octaves_; ++i){
            total += noiseValue * amplitude;

            frequency *= param.lacunarity_;
            maxAmplitude += amplitude;
            amplitude *= param.persistence_;
        }
        LASSERT(0.00001f<lcore::absolute(maxAmplitude));
        return total/maxAmplitude;
    }


    f32 Fractal::turbulence(const Noise& noise, const Param& param, f32 x, f32 y)
    {
        f32 total = 0.0f;

        f32 frequency = param.frequency_;
        f32 amplitude = 1.0f;
        f32 maxAmplitude = 0.0f;

        for(s32 i=0; i<param.octaves_; ++i){
            f32 v = 2.0f*noise(x*frequency, y*frequency) - 1.0f;
            total += lcore::absolute(v) * amplitude;

            frequency *= param.lacunarity_;
            maxAmplitude += amplitude;
            amplitude *= param.persistence_;
        }
        LASSERT(0.00001f<lcore::absolute(maxAmplitude));
        return total/maxAmplitude;
    }

    f32 Fractal::turbulence(const Noise& noise, const Param& param, f32 x, f32 y, f32 z)
    {
        f32 total = 0.0f;

        f32 frequency = param.frequency_;
        f32 amplitude = 1.0f;
        f32 maxAmplitude = 0.0f;

        for(s32 i=0; i<param.octaves_; ++i){
            f32 v = 2.0f*noise(x*frequency, y*frequency, z*frequency) - 1.0f;
            total += lcore::absolute(v) * amplitude;

            frequency *= param.lacunarity_;
            maxAmplitude += amplitude;
            amplitude *= param.persistence_;
        }
        LASSERT(0.00001f<lcore::absolute(maxAmplitude));
        return total/maxAmplitude;
    }

    f32 Fractal::turbulenceConst(f32 noiseValue, const Param& param)
    {
        f32 total = 0.0f;

        f32 frequency = param.frequency_;
        f32 amplitude = 1.0f;
        f32 maxAmplitude = 0.0f;

        for(s32 i=0; i<param.octaves_; ++i){
            f32 v = 2.0f*noiseValue - 1.0f;
            total += lcore::absolute(v) * amplitude;

            frequency *= param.lacunarity_;
            maxAmplitude += amplitude;
            amplitude *= param.persistence_;
        }
        LASSERT(0.00001f<lcore::absolute(maxAmplitude));
        return total/maxAmplitude;
    }

    f32 Fractal::multiFractal(const Noise& noise, const Param& param, f32 x, f32 y)
    {
        f32 total = 1.0f;

        f32 frequency = param.frequency_;
        f32 amplitude = 1.0f;
        f32 maxAmplitude = 0.0f;

        for(s32 i=0; i<param.octaves_; ++i){
            f32 n = 2.0f*noise(x*frequency, y*frequency) - 1.0f;
            total *= (n+param.offset_) * amplitude;

            frequency *= param.lacunarity_;
            maxAmplitude += amplitude;
            amplitude *= param.persistence_;
        }
        LASSERT(0.00001f<lcore::absolute(maxAmplitude));
        return total/maxAmplitude;
    }

    f32 Fractal::multiFractal(const Noise& noise, const Param& param, f32 x, f32 y, f32 z)
    {
        f32 total = 1.0f;

        f32 frequency = param.frequency_;
        f32 amplitude = 1.0f;
        f32 maxAmplitude = 0.0f;

        for(s32 i=0; i<param.octaves_; ++i){
            f32 n = 2.0f*noise(x*frequency, y*frequency, z*frequency) - 1.0f;
            total *= (n+param.offset_) * amplitude;

            frequency *= param.lacunarity_;
            maxAmplitude += amplitude;
            amplitude *= param.persistence_;
        }
        LASSERT(0.00001f<lcore::absolute(maxAmplitude));
        return total/maxAmplitude;
    }

    f32 Fractal::multiFractalConst(f32 noiseValue, const Param& param)
    {
        f32 total = 1.0f;

        f32 frequency = param.frequency_;
        f32 amplitude = 1.0f;
        f32 maxAmplitude = 0.0f;

        for(s32 i=0; i<param.octaves_; ++i){
            f32 n = 2.0f*noiseValue - 1.0f;
            total *= (n+param.offset_) * amplitude;

            frequency *= param.lacunarity_;
            maxAmplitude += amplitude;
            amplitude *= param.persistence_;
        }
        LASSERT(0.00001f<lcore::absolute(maxAmplitude));
        return total/maxAmplitude;
    }

    void Fractal::RidgedParam::calcSpectralWeights()
    {
        offset_ = lcore::clamp01(offset_);
        octaves_ = lcore::clamp(octaves_, 0, MaxOctaves);

        f32 frequency = 1.0f;
        for(s32 i=0; i<MaxOctaves; ++i){
            spectralWeights_[i] = powf(frequency, -1.0f);
            frequency *= lacunarity_;
        }
    }

    f32 Fractal::ridgedMultiFractal(const Noise& noise, const RidgedParam& param, f32 x, f32 y)
    {
        x *= param.frequency_;
        y *= param.frequency_;

        f32 signal = 0.0f;
        f32 value = 0.0f;
        f32 weight = 1.0f;
        f32 offset = 1.0f;
        f32 gain = 2.0f;
        for(s32 i=0; i<param.octaves_; ++i){
            signal = noise(x, y);
            signal = offset - lcore::absolute(signal);
            signal *= signal;
            signal *= weight;

            weight = lcore::clamp01(signal * gain);
            value += (signal * param.spectralWeights_[i]);
            x *= param.lacunarity_;
            y *= param.lacunarity_;
        }
        return value * 1.25f - 1.0f;
    }

    f32 Fractal::ridgedMultiFractal(const Noise& noise, const RidgedParam& param, f32 x, f32 y, f32 z)
    {
        x *= param.frequency_;
        y *= param.frequency_;
        z *= param.frequency_;

        f32 signal = 0.0f;
        f32 value = 0.0f;
        f32 weight = 1.0f;
        f32 offset = 1.0f;
        f32 gain = 2.0f;
        for(s32 i=0; i<param.octaves_; ++i){
            signal = noise(x, y, z);
            signal = offset - lcore::absolute(signal);
            signal *= signal;
            signal *= weight;

            weight = lcore::clamp01(signal * gain);
            value += (signal * param.spectralWeights_[i]);
            x *= param.lacunarity_;
            y *= param.lacunarity_;
            z *= param.lacunarity_;
        }
        return value * 1.25f - 1.0f;
    }

    f32 Fractal::ridgedMultiFractalConst(f32 noiseValue, const RidgedParam& param)
    {
        f32 signal = 0.0f;
        f32 value = 0.0f;
        f32 weight = 1.0f;
        f32 offset = 1.0f;
        f32 gain = 2.0f;
        for(s32 i=0; i<param.octaves_; ++i){
            signal = noiseValue;
            signal = offset - lcore::absolute(signal);
            signal *= signal;
            signal *= weight;

            weight = lcore::clamp01(signal * gain);
            value += (signal * param.spectralWeights_[i]);
        }
        return value * 1.25f - 1.0f;
    }

    f32 Fractal::linearWrapAround(f32 x)
    {
        LASSERT(-1.0f<=x && x<=1.0f);

        if(0.5f<=x){
            return 2.0f*(1.0f - x);
        }
        if(-0.5f<=x){
            return 2.0f*x;
        }
        return -2.0f*(1.0f + x);
    }
#endif

    void NoiseSample1::add(f32 f, const NoiseSample1& x)
    {
        value_ += f*x.value_;
        dx_ += f*x.dx_;
    }

    NoiseSample1 operator*(const NoiseSample1& x0, const NoiseSample1& x1)
    {
        f32 value = x0.value_ * x1.value_;
        f32 dx = x0.dx_*x1.value_ + x0.value_*x1.dx_;

        return NoiseSample1(value, dx);
    }

    NoiseSample1 operator*(const NoiseSample1& x, f32 f)
    {
        f32 value = x.value_ * f;
        f32 dx = x.dx_ * f;

        return NoiseSample1(value, dx);
    }

    NoiseSample1 operator*(f32 f, const NoiseSample1& x)
    {
        f32 value = f * x.value_;
        f32 dx = f * x.dx_;

        return NoiseSample1(value, dx);
    }

    void NoiseSample2::add(f32 f, const NoiseSample2& x)
    {
        value_ += f*x.value_;
        dx_ += f*x.dx_;
        dy_ += f*x.dy_;
    }

    NoiseSample2 operator*(const NoiseSample2& x0, const NoiseSample2& x1)
    {
        f32 value = x0.value_ * x1.value_;
        f32 dx = x0.dx_*x1.value_ + x0.value_*x1.dx_;
        f32 dy = x0.dy_*x1.value_ + x0.value_*x1.dy_;

        return NoiseSample2(value, dx, dy);
    }

    NoiseSample2 operator*(const NoiseSample2& x, f32 f)
    {
        f32 value = x.value_ * f;
        f32 dx = x.dx_ * f;
        f32 dy = x.dy_ * f;

        return NoiseSample2(value, dx, dy);
    }

    NoiseSample2 operator*(f32 f, const NoiseSample2& x)
    {
        f32 value = f * x.value_;
        f32 dx = f * x.dx_;
        f32 dy = f * x.dy_;

        return NoiseSample2(value, dx, dy);
    }

    void NoiseSample3::add(f32 f, const NoiseSample3& x)
    {
        value_ += f*x.value_;
        dx_ += f*x.dx_;
        dy_ += f*x.dy_;
        dz_ += f*x.dz_;
    }

    NoiseSample3 operator*(const NoiseSample3& x0, const NoiseSample3& x1)
    {
        f32 value = x0.value_ * x1.value_;
        f32 dx = x0.dx_*x1.value_ + x0.value_*x1.dx_;
        f32 dy = x0.dy_*x1.value_ + x0.value_*x1.dy_;
        f32 dz = x0.dz_*x1.value_ + x0.value_*x1.dz_;

        return NoiseSample3(value, dx, dy, dz);
    }

    NoiseSample3 operator*(const NoiseSample3& x, f32 f)
    {
        f32 value = x.value_ * f;
        f32 dx = x.dx_ * f;
        f32 dy = x.dy_ * f;
        f32 dz = x.dz_ * f;

        return NoiseSample3(value, dx, dy, dz);
    }

    NoiseSample3 operator*(f32 f, const NoiseSample3& x)
    {
        f32 value = f * x.value_;
        f32 dx = f * x.dx_;
        f32 dy = f * x.dy_;
        f32 dz = f * x.dz_;

        return NoiseSample3(value, dx, dy, dz);
    }

    //---------------------------------------------------
    //---
    //--- Noise
    //---
    //---------------------------------------------------
    Noise::Noise(u64 seed)
    {
        permutate(seed);
    }

    void Noise::permutate(u64 seed)
    {
        lcore::RandXorshift128Plus32 rand(seed);

        s32 size = PermutationTableSize >> 1;

        for(s32 i=0; i<size; ++i){
            permutation_[i] = static_cast<u8>(i);
        }

        for(s32 i=0; i<size; ++i){
            s32 index = range_ropen(rand, i, size);
            lcore::swap(permutation_[i], permutation_[index]);
        }

        for(s32 i=size; i<PermutationTableSize; ++i){
            permutation_[i] = permutation_[i - size];
        }
    }

    NoiseSample2 Noise::fbm(Func2D func2D, f32 px, f32 py, s32 octave, f32 frequency, f32 lacunarity, f32 persistence) const
    {
        NoiseSample2 total = (this->*func2D)(px, py, frequency);
        f32 scale = 1.0f;
        f32 amplitude = 1.0f;

        for(s32 i=1; i<octave; ++i){
            frequency *= lacunarity;
            amplitude *= persistence;
            scale += amplitude;
            NoiseSample2 v = (this->*func2D)(px, py, frequency);
            total.add(amplitude, v);
        }

        return total * (1.0f/scale);
    }

    NoiseSample3 Noise::fbm(Func3D func3D, f32 px, f32 py, f32 pz, s32 octave, f32 frequency, f32 lacunarity, f32 persistence) const
    {
        NoiseSample3 total = (this->*func3D)(px, py, pz, frequency);
        f32 scale = 1.0f;
        f32 amplitude = 1.0f;

        for(s32 i=1; i<octave; ++i){
            frequency *= lacunarity;
            amplitude *= persistence;
            scale += amplitude;
            NoiseSample3 v = (this->*func3D)(px, py, pz, frequency);
            total.add(amplitude, v);
        }

        return total * (1.0f/scale);
    }

    NoiseSample1 Noise::value1D(f32 px, f32 frequency) const
    {
        px += frequency;
        s32 ix0 = fastFloor(px);
        f32 t = px-ix0;
        f32 dt = fadeDerivative(t);
        t = fade(t);
        ix0 &= PermutationTableSizeMask;
        s32 ix1 = ix0+1;
        s32 h0 = permutation_[ix0];
        s32 h1 = permutation_[ix1];

        NoiseSample1 sample;
        sample.value_ = lerpf(h0, h1, t);
        sample.dx_ = dt * (h0-h1);
        sample.dx_ *= frequency;
        return sample * (1.0f / PermutationTableSizeMask);
    }

    NoiseSample2 Noise::value2D(f32 px, f32 py, f32 frequency) const
    {
        px *= frequency;
        py *= frequency;
        s32 ix0 = fastFloor(px);
        s32 iy0 = fastFloor(py);
        f32 tx = px-ix0;
        f32 ty = py-iy0;
        f32 dtx = fadeDerivative(tx);
        f32 dty = fadeDerivative(ty);
        tx = fade(tx);
        ty = fade(ty);
        ix0 &= PermutationTableSizeMask;
        iy0 &= PermutationTableSizeMask;
        s32 ix1 = ix0+1;
        s32 iy1 = iy0+1;

        s32 h0 = permutation_[ix0];
        s32 h1 = permutation_[ix1];
        s32 h00 = permutation_[h0+iy0];
        s32 h10 = permutation_[h1+iy0];
        s32 h01 = permutation_[h0+iy1];
        s32 h11 = permutation_[h1+iy1];

        f32 b = static_cast<f32>(h10 - h00);
        f32 c = static_cast<f32>(h01 - h00);
        f32 d = static_cast<f32>(h11 - h01 - h10 + h00);

        NoiseSample2 sample;
        sample.value_ = lerpf(lerpf(h00, h10, tx), lerpf(h01, h11, tx), ty);
        sample.dx_ = (b + d*ty) * dtx;
        sample.dy_ = (c + d*tx) * dty;
        sample.dx_ *= frequency;
        sample.dy_ *= frequency;
        return sample * (1.0f / PermutationTableSizeMask);
    }

    NoiseSample3 Noise::value3D(f32 px, f32 py, f32 pz, f32 frequency) const
    {
        px *= frequency;
        py *= frequency;
        pz *= frequency;
        s32 ix0 = fastFloor(px);
        s32 iy0 = fastFloor(py);
        s32 iz0 = fastFloor(pz);
        f32 tx = px-ix0;
        f32 ty = py-iy0;
        f32 tz = pz-iz0;
        f32 dtx = fadeDerivative(tx);
        f32 dty = fadeDerivative(ty);
        f32 dtz = fadeDerivative(tz);
        tx = fade(tx);
        ty = fade(ty);
        tz = fade(tz);
        ix0 &= PermutationTableSizeMask;
        iy0 &= PermutationTableSizeMask;
        iz0 &= PermutationTableSizeMask;
        s32 ix1 = ix0+1;
        s32 iy1 = iy0+1;
        s32 iz1 = iz0+1;

        s32 h0 = permutation_[ix0];
        s32 h1 = permutation_[ix1];
        s32 h00 = permutation_[h0+iy0];
        s32 h10 = permutation_[h1+iy0];
        s32 h01 = permutation_[h0+iy1];
        s32 h11 = permutation_[h1+iy1];

        s32 h000 = permutation_[h00+iz0];
        s32 h100 = permutation_[h10+iz0];
        s32 h010 = permutation_[h01+iz0];
        s32 h110 = permutation_[h11+iz0];
        s32 h001 = permutation_[h00+iz1];
        s32 h101 = permutation_[h10+iz1];
        s32 h011 = permutation_[h01+iz1];
        s32 h111 = permutation_[h11+iz1];

        f32 b = static_cast<f32>(h100 - h000);
        f32 c = static_cast<f32>(h010 - h000);
        f32 d = static_cast<f32>(h001 - h000);
        f32 e = static_cast<f32>(h110 - h010 - h100 + h000);
        f32 f = static_cast<f32>(h101 - h001 - h100 + h000);
        f32 g = static_cast<f32>(h011 - h001 - h010 + h000);
        f32 h = static_cast<f32>(h111 - h011 - h101 + h001 - h110 + h010 + h100 - h000);

        NoiseSample3 sample;
        sample.value_ = lerpf(
            lerpf(lerpf(h000, h100, tx), lerpf(h010, h110, tx), ty),
            lerpf(lerpf(h001, h101, tx), lerpf(h011, h111, tx), ty),
            tz);
        sample.dx_ = (b + e * ty + (f + h * ty) * tz) * dtx;
        sample.dy_ = (c + e * tx + (g + h * tx) * tz) * dty;
        sample.dz_ = (d + f * tx + (g + h * tx) * ty) * dtz;
        sample.dx_ *= frequency;
        sample.dy_ *= frequency;
        sample.dz_ *= frequency;
        return sample * (1.0f / PermutationTableSizeMask);
    }

    NoiseSample1 Noise::perlin1D(f32 px, f32 frequency) const
    {
        px *= frequency;
        s32 ix0 = fastFloor(px);
        f32 t0 = px-ix0;
        f32 t1 = t0-1.0f;
        ix0 &= PermutationTableSizeMask;
        s32 ix1 = ix0+1;

        f32 g0 = grad3_[permutation_[ix0]&0x01U][0];
        f32 g1 = grad3_[permutation_[ix1]&0x01U][0];
        f32 v0 = g0*t0;
        f32 v1 = g1*t1;

        f32 dt = fadeDerivative(t0);
        f32 t = fade(t0);

        f32 a = v0;
        f32 b = v1 - v0;

        f32 da = g0;
        f32 db = g1 - g0;

        NoiseSample1 sample;
        sample.value_ = a + b*t;
        sample.dx_ = da + db * t + b*dt;
        sample.dx_ *= frequency;

        return sample*2.0f;
    }

    NoiseSample2 Noise::perlin2D(f32 px, f32 py, f32 frequency) const
    {
        px *= frequency;
        py *= frequency;
        s32 ix0 = fastFloor(px);
        s32 iy0 = fastFloor(py);
        f32 tx0 = px-ix0;
        f32 ty0 = py-iy0;
        f32 tx1 = tx0-1.0f;
        f32 ty1 = ty0-1.0f;
        ix0 &= PermutationTableSizeMask;
        iy0 &= PermutationTableSizeMask;
        s32 ix1 = ix0+1;
        s32 iy1 = iy0+1;

        s32 h0 = permutation_[ix0];
        s32 h1 = permutation_[ix1];
        s32 h00 = permutation_[h0+iy0] & 0x07;
        s32 h10 = permutation_[h1+iy0] & 0x07;
        s32 h01 = permutation_[h0+iy1] & 0x07;
        s32 h11 = permutation_[h1+iy1] & 0x07;

        Vector2 g00 = grad2_[h00];
        Vector2 g10 = grad2_[h10];
        Vector2 g01 = grad2_[h01];
        Vector2 g11 = grad2_[h11];

        f32 v00 = dot2(g00, tx0, ty0);
        f32 v10 = dot2(g10, tx1, ty0);
        f32 v01 = dot2(g01, tx0, ty1);
        f32 v11 = dot2(g11, tx1, ty1);

        f32 dtx = fadeDerivative(tx0);
        f32 dty = fadeDerivative(ty0);
        f32 tx = fade(tx0);
        f32 ty = fade(ty0);

        f32 a = v00;
        f32 b = v10 - v00;
        f32 c = v01 - v00;
        f32 d = v11 - v01 - v10 + v00;

        Vector2 da = g00;
        Vector2 db = g10 - g00;
        Vector2 dc = g01 - g00;
        Vector2 dd = g11 - g01 - g10 + g00;

        NoiseSample2 sample;
        sample.value_ = a + b*tx + (c + d*tx)*ty;
        sample.dx_ = da.x_ + db.x_*tx + (dc.x_ + dd.x_*tx)*ty;
        sample.dy_ = da.y_ + db.y_*tx + (dc.y_ + dd.y_*tx)*ty;
        sample.dx_ += (b + d*ty) * dtx;
        sample.dy_ += (c + d*tx) * dty;
        sample.dx_ *= frequency;
        sample.dy_ *= frequency;

        return sample * 1.41421356f;
    }

    NoiseSample3 Noise::perlin3D(f32 px, f32 py, f32 pz, f32 frequency) const
    {
        px *= frequency;
        py *= frequency;
        pz *= frequency;
        s32 ix0 = fastFloor(px);
        s32 iy0 = fastFloor(py);
        s32 iz0 = fastFloor(pz);
        f32 tx0 = px-ix0;
        f32 ty0 = py-iy0;
        f32 tz0 = pz-iz0;
        f32 tx1 = tx0-1.0f;
        f32 ty1 = ty0-1.0f;
        f32 tz1 = tz0-1.0f;
        ix0 &= PermutationTableSizeMask;
        iy0 &= PermutationTableSizeMask;
        iz0 &= PermutationTableSizeMask;
        s32 ix1 = ix0+1;
        s32 iy1 = iy0+1;
        s32 iz1 = iz0+1;

        s32 h0 = permutation_[ix0];
        s32 h1 = permutation_[ix1];
        s32 h00 = permutation_[h0+iy0];
        s32 h10 = permutation_[h1+iy0];
        s32 h01 = permutation_[h0+iy1];
        s32 h11 = permutation_[h1+iy1];

        s32 h000 = permutation_[h00+iz0] & 0x0F;
        s32 h100 = permutation_[h10+iz0] & 0x0F;
        s32 h010 = permutation_[h01+iz0] & 0x0F;
        s32 h110 = permutation_[h11+iz0] & 0x0F;
        s32 h001 = permutation_[h00+iz1] & 0x0F;
        s32 h101 = permutation_[h10+iz1] & 0x0F;
        s32 h011 = permutation_[h01+iz1] & 0x0F;
        s32 h111 = permutation_[h11+iz1] & 0x0F;

        Vector3 g000 = grad3(grad3_, h000);
        Vector3 g100 = grad3(grad3_, h100);
        Vector3 g010 = grad3(grad3_, h010);
        Vector3 g110 = grad3(grad3_, h110);
        Vector3 g001 = grad3(grad3_, h001);
        Vector3 g101 = grad3(grad3_, h101);
        Vector3 g011 = grad3(grad3_, h011);
        Vector3 g111 = grad3(grad3_, h111);

        f32 v000 = dot3(grad3_, h000, tx0, ty0, tz0);
        f32 v100 = dot3(grad3_, h100, tx1, ty0, tz0);
        f32 v010 = dot3(grad3_, h010, tx0, ty1, tz0);
        f32 v110 = dot3(grad3_, h110, tx1, ty1, tz0);
        f32 v001 = dot3(grad3_, h001, tx0, ty0, tz1);
        f32 v101 = dot3(grad3_, h101, tx1, ty0, tz1);
        f32 v011 = dot3(grad3_, h011, tx0, ty1, tz1);
        f32 v111 = dot3(grad3_, h111, tx1, ty1, tz1);

        f32 dtx = fadeDerivative(tx0);
        f32 dty = fadeDerivative(ty0);
        f32 dtz = fadeDerivative(tz0);
        f32 tx = fade(tx0);
        f32 ty = fade(ty0);
        f32 tz = fade(tz0);

        f32 a = v000;
        f32 b = v100 - v000;
        f32 c = v010 - v000;
        f32 d = v001 - v000;
        f32 e = v110 - v010 - v100 + v000;
        f32 f = v101 - v001 - v100 + v000;
        f32 g = v011 - v001 - v010 + v000;
        f32 h = v111 - v011 - v101 + v001 - v110 + v010 + v100 - v000;

        Vector3 da = g000;
        Vector3 db = g100 - g000;
        Vector3 dc = g010 - g000;
        Vector3 dd = g001 - g000;
        Vector3 de = g110 - g010 - g100 + g000;
        Vector3 df = g101 - g001 - g100 + g000;
        Vector3 dg = g011 - g001 - g010 + g000;
        Vector3 dh = g111 - g011 - g101 + g001 - g110 + g010 + g100 - g000;

        NoiseSample3 sample;
        sample.value_ = a + b * tx + (c + e * tx) * ty + (d + f * tx + (g + h * tx) * ty) * tz;
        sample.dx_ = da.x_ + db.x_ * tx + (dc.x_ + de.x_ * tx) * ty + (dd.x_ + df.x_ * tx + (dg.x_ + dh.x_ * tx) * ty) * tz;
        sample.dy_ = da.y_ + db.y_ * tx + (dc.y_ + de.y_ * tx) * ty + (dd.y_ + df.y_ * tx + (dg.y_ + dh.y_ * tx) * ty) * tz;
        sample.dz_ = da.z_ + db.z_ * tx + (dc.z_ + de.z_ * tx) * ty + (dd.z_ + df.z_ * tx + (dg.z_ + dh.x_ * tx) * ty) * tz;

        sample.dx_ += (b + e * ty + (f + h * ty) * tz) * dtx;
        sample.dy_ += (c + e * tx + (g + h * tx) * tz) * dty;
        sample.dz_ += (d + f * tx + (g + h * tx) * ty) * dtz;
        sample.dx_ *= frequency;
        sample.dy_ *= frequency;
        sample.dz_ *= frequency;
        return sample;
    }


    const Noise::Vector2 Noise::grad2_[Noise::NumGrad2] =
    {
        Vector2(1.0f, 0.0f),
        Vector2(-1.0f, 0.0f),
        Vector2(0.0f, 1.0f),
        Vector2(0.0f, -1.0f),
        Vector2(0.70710678118f, 0.70710678118f),
        Vector2(-0.70710678118f, 0.70710678118f),
        Vector2(0.70710678118f, -0.70710678118f),
        Vector2(-0.70710678118f, -0.70710678118f),
    };

    const s8 Noise::grad3_[NumGrad3][3] =
    {
        {1, 1, 0}, {-1, 1, 0}, {1, -1, 0},
        {-1, -1, 0}, {1, 0, 1}, {-1, 0, 1},
        {1, 0, -1}, {-1, 0, -1}, {0, 1, 1},
        {0, -1, 1}, {0, 1, -1}, {0, -1, -1},

        {1, 1, 0}, {-1, 1, 0}, {0, -1, 1}, {0, -1, -1},
    };

    //const s8 Noise::simplex_[64][4] =
    //{
    //    {0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
    //    {0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
    //    {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
    //    {1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
    //    {1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
    //    {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
    //    {2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
    //    {2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0},
    //};
}

