#ifndef INC_LCORE_NOISE_H__
#define INC_LCORE_NOISE_H__
/**
@file Noise.h
@author t-sakai
@date 2014/05/08 create
*/
#include "lcore.h"

namespace lcore
{
    //------------------------------------------------
    //---
    //--- Noise
    //---
    //------------------------------------------------
    class Noise
    {
    public:
        ~Noise()
        {}

        virtual f32 operator()(f32 x) const =0;
        virtual f32 operator()(f32 x, f32 y) const =0;
        virtual f32 operator()(f32 x, f32 y, f32 z) const =0;
        virtual f32 operator()(f32 x, f32 y, f32 z, f32 w) const =0;

    protected:
        Noise()
        {}

        inline static s32 fastFloor(f32 x);
        inline static void fastFloor2(s32* ret, f32 x, f32 y, f32 z);
        inline static f32 dot2(const s8 grad[2], f32 x, f32 y);
        inline static f32 dot3(const s8 grad[3], f32 x, f32 y, f32 z);
        inline static f32 dot4(const s8 grad[4], f32 x, f32 y, f32 z, f32 w);
        inline static f32 fade(f32 x);
    };

    //------------------------------------------------
    //---
    //--- Value
    //---
    //------------------------------------------------
    class Value : public Noise
    {
    public:
        static const s32 PermutationTableSize = 256*2;
        Value();
        ~Value();

        void random(u32 seed);

        virtual f32 operator()(f32 x) const;
        virtual f32 operator()(f32 x, f32 y) const;
        virtual f32 operator()(f32 x, f32 y, f32 z) const;
        virtual f32 operator()(f32 x, f32 y, f32 z, f32 w) const;

        static f32 getMin(){ return 0.0f;}
        static f32 getMax(){ return 1.0f;}
    private:
        u8 permutation_[PermutationTableSize];
    };

    //------------------------------------------------
    //---
    //--- Perlin
    //---
    //------------------------------------------------
    class Perlin : public Noise
    {
    public:
        static const s32 PermutationTableSize = 256*2;
        static const s32 NumGrad3 = 12;
        static const s32 NumGrad4 = 32;

        Perlin();
        ~Perlin();

        void random(u32 seed);

        virtual f32 operator()(f32 x) const;
        virtual f32 operator()(f32 x, f32 y) const;
        virtual f32 operator()(f32 x, f32 y, f32 z) const;
        virtual f32 operator()(f32 x, f32 y, f32 z, f32 w) const;

        f32 cpu_get(f32 x, f32 y, f32 z) const;

        static f32 getMin(){ return -1.0f;}
        static f32 getMax(){ return 1.0f;}
    private:
        static s8 grad3_[NumGrad3][3];
        static s8 grad4_[NumGrad4][4];
        static u8 simplex_[64][4];

        u8 permutation_[PermutationTableSize];
    };

    //------------------------------------------------
    //---
    //--- Fractal
    //---
    //------------------------------------------------
    class Fractal
    {
    public:
        struct Param
        {
            Param()
                :octaves_(5)
                ,frequency_(0.2f)
                ,persistence_(0.51f)
                ,lacunarity_(2.0f)
                ,offset_(0.0f)
            {}

            Param(
                s32 octaves,
                f32 frequency,
                f32 persistence,
                f32 lacunarity,
                f32 offset)
                :octaves_(octaves)
                ,frequency_(frequency)
                ,persistence_(persistence)
                ,lacunarity_(lacunarity)
                ,offset_(offset)
            {}

            s32 octaves_;
            f32 frequency_;
            f32 persistence_;
            f32 lacunarity_;
            f32 offset_;
        };

        struct RidgedParam
        {
            static const s32 MaxOctaves = 16;

            RidgedParam()
                :octaves_(5)
                ,frequency_(0.2f)
                ,gain_(2.0f)
                ,lacunarity_(2.0f)
                ,offset_(1.0f)
            {
                calcSpectralWeights();
            }

            RidgedParam(
                s32 octaves,
                f32 frequency,
                f32 gain,
                f32 lacunarity,
                f32 offset)
                :octaves_(octaves)
                ,frequency_(frequency)
                ,gain_(gain)
                ,lacunarity_(lacunarity)
                ,offset_(offset)
            {
                calcSpectralWeights();
            }

            void calcSpectralWeights();

            s32 octaves_;
            f32 frequency_;
            f32 gain_;
            f32 lacunarity_;
            f32 offset_;

            f32 spectralWeights_[MaxOctaves];
        };

        static f32 fbm(const Noise& noise, const Param& param, f32 x, f32 y);
        static f32 fbm(const Noise& noise, const Param& param, f32 x, f32 y, f32 z);
        static f32 fbmConst(f32 noiseValue, const Param& param);

        static f32 turbulence(const Noise& noise, const Param& param, f32 x, f32 y);
        static f32 turbulence(const Noise& noise, const Param& param, f32 x, f32 y, f32 z);
        static f32 turbulenceConst(f32 noiseValue, const Param& param);

        static f32 multiFractal(const Noise& noise, const Param& param, f32 x, f32 y);
        static f32 multiFractal(const Noise& noise, const Param& param, f32 x, f32 y, f32 z);
        static f32 multiFractalConst(f32 noiseValue, const Param& param);

        static f32 ridgedMultiFractal(const Noise& noise, const RidgedParam& param, f32 x, f32 y);
        static f32 ridgedMultiFractal(const Noise& noise, const RidgedParam& param, f32 x, f32 y, f32 z);
        static f32 ridgedMultiFractalConst(f32 noiseValue, const RidgedParam& param);

        static inline f32 normalize(f32 val, f32 minVal, f32 invExtent);

        static f32 linearWrapAround(f32 x);

    };

    inline f32 Fractal::normalize(f32 val, f32 minVal, f32 invExtent)
    {
        return (val-minVal)*invExtent;
    }

#if 0
    
using UnityEngine;
namespace LUtil
{
    public class Noise
    {
        public delegate NoiseSample Func1D(float point, float frequency);
        public delegate NoiseSample Func2D(Vector2 point, float frequency);
        public delegate NoiseSample Func3D(Vector3 point, float frequency);

        public struct NoiseSample
        {
            public float value_;
            public Vector3 derivative_;

            public static NoiseSample operator+(NoiseSample x0, NoiseSample x1)
            {
                x0.value_ += x1.value_;
                x0.derivative_ += x1.derivative_;
                return x0;
            }

            public static NoiseSample operator+(NoiseSample x, float f)
            {
                x.value_ += f;
                return x;
            }

            public static NoiseSample operator+(float f, NoiseSample x)
            {
                x.value_ += f;
                return x;
            }

            public static NoiseSample operator-(NoiseSample x0, NoiseSample x1)
            {
                x0.value_ -= x1.value_;
                x0.derivative_ -= x1.derivative_;
                return x0;
            }

            public static NoiseSample operator-(NoiseSample x, float f)
            {
                x.value_ -= f;
                return x;
            }

            public static NoiseSample operator-(float f, NoiseSample x)
            {
                x.value_ = f-x.value_;
                x.derivative_ = -x.derivative_;
                return x;
            }

            public static NoiseSample operator*(NoiseSample x0, NoiseSample x1)
            {
                x0.derivative_ = x0.derivative_*x1.value_ + x1.derivative_*x0.value_;
                x0.value_ *= x1.value_;
                return x0;
            }

            public static NoiseSample operator*(NoiseSample x, float f)
            {
                x.value_ *= f;
                x.derivative_ *= f;
                return x;
            }

            public static NoiseSample operator*(float f, NoiseSample x)
            {
                x.value_ *= f;
                x.derivative_ *= f;
                return x;
            }
        };

        private const int PermutationTableSize = 256 * 2;
        private const int NumGrad2 = 8;
        private const int NumGrad3 = 16;

        private byte[] permutation_ = new byte[PermutationTableSize];

        private static readonly Vector2[] grad2_ = new Vector2[NumGrad2]
        {
            new Vector2( 1f, 0f),
            new Vector2(-1f, 0f),
            new Vector2( 0f, 1f),
            new Vector2( 0f,-1f),
            new Vector2( 1f, 1f).normalized,
            new Vector2(-1f, 1f).normalized,
            new Vector2( 1f,-1f).normalized,
            new Vector2(-1f,-1f).normalized,
        };


        private static readonly sbyte[,] grad3_ = new sbyte[NumGrad3, 3]
        {
            {1,1,0},{-1,1,0},{1,-1,0},
            {-1,-1,0},{1,0,1},{-1,0,1},
            {1,0,-1},{-1,0,-1},{0,1,1},
            {0,-1,1},{0,1,-1},{0,-1,-1},

            {1,1,0},{-1,1,0},{0,-1,1},{0,-1,-1},
        };

        //private static readonly byte[,] simplex_ = new byte[64, 4]
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

        static void swap<T>(ref T x0, ref T x1)
        {
            T tmp = x0;
            x0 = x1;
            x1 = tmp;
        }

        static int fastFloor(float x)
        {
            return (0.0f < x) ? (int)x : (int)x - 1;
        }

        static float dot2(sbyte[,] grad, int i, float x, float y)
        {
            return (x * grad[i, 0] + y * grad[i, 1]);
        }

        static float dot2(Vector2 grad, float x, float y)
        {
            return (x * grad.x + y * grad.y);
        }

        static float dot3(sbyte[,] grad, int i, float x, float y, float z)
        {
            return (x * grad[i, 0] + y * grad[i, 1] + z * grad[i, 2]);
        }

        static float dot4(sbyte[,] grad, int i, float x, float y, float z, float w)
        {
            return (x * grad[i, 0] + y * grad[i, 1] + z * grad[i, 2] + w * grad[i, 3]);
        }

        static Vector3 grad3(sbyte[,] grad, int i)
        {
            return new Vector3(grad[i, 0], grad[i, 1], grad[i, 2]);
        }

        private static float fade(float x)
        {
            return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
        }


        private static float fadeDerivative(float x)
        {
            return 30.0f * x * x * (x * (x - 2.0f) + 1.0f);
        }

        public Noise(int seed=1)
        {
            random(seed);
        }

        public void random(int seed)
        {
            int prevSeed = Random.seed;
            Random.seed = seed;
            int size = PermutationTableSize >> 1;

            for(int i = 0; i < size; ++i) {
                permutation_[i] = (byte)i;
            }

            for(int i = 0; i < size; ++i) {
                int index = Random.Range(i, size);
                swap<byte>(ref permutation_[i], ref permutation_[index]);
            }

            for(int i = size; i < PermutationTableSize; ++i) {
                permutation_[i] = permutation_[i - size];
            }
            Random.seed = prevSeed;
        }

        public static NoiseSample fbm(Func2D func2D, Vector2 point, int octave, float frequency, float lacunarity=2.0f, float persistence=0.5f)
        {
            NoiseSample total = func2D(point, frequency);
            float scale = 1.0f;
            float amplitude = 1.0f;

            for(int i = 1; i < octave; ++i) {
                frequency *= lacunarity;
                amplitude *= persistence;
                scale += amplitude;
                NoiseSample v = func2D(point, frequency);
                total += v * amplitude;
            }

            return total * (1.0f/scale);
        }

        public static NoiseSample fbm(Func3D func3D, Vector3 point, int octave, float frequency, float lacunarity=2.0f, float persistence=0.5f)
        {
            NoiseSample total = func3D(point, frequency);
            float scale = 1.0f;
            float amplitude = 1.0f;

            for(int i = 1; i < octave; ++i) {
                frequency *= lacunarity;
                amplitude *= persistence;
                scale += amplitude;
                NoiseSample v = func3D(point, frequency);
                total += v * amplitude;
            }

            return total * (1.0f/scale);
        }

        //public float turbulence(float x, float y, int octave, float scale, float frequency)
        //{
        //    float total = 1.0f;

        //    float amplitude = 1.0f;

        //    for(int i = 0; i < octave; ++i) {
        //        float v = 2.0f*value2d(x * frequency, y * frequency)-1.0f;
        //        total += Mathf.Abs(v) * amplitude;
        //        frequency *= 2.0013f;
        //        amplitude *= 0.5f;
        //    }

        //    return total * scale;
        //}

        //public float multiFractal(float x, float y, int octave, float scale, float frequency)
        //{
        //    float total = 1.0f;

        //    float amplitude = 1.0f;

        //    for(int i = 0; i < octave; ++i) {
        //        float v = 2.0f * value2d(x * frequency, y * frequency) - 1.0f;
        //        total *= v * amplitude;
        //        frequency *= 2.0013f;
        //        amplitude *= 0.5f;
        //    }

        //    return total * scale;
        //}


        //-------------------------------------------------------
        private const int HashSize = 256;
        private const int HashMask = HashSize-1;
        private static byte[] hash_ = new byte[HashSize*2];
        public static void init()
        {
            for(int i = 0; i < HashSize; ++i) {
                hash_[i] = (byte)i;
            }
            for(int i = 0; i < HashSize; ++i) {
                int index = Random.Range(i, HashSize);
                byte tmp = hash_[index];
                hash_[index] = hash_[i];
                hash_[i] = tmp;
            }
            for(int i = HashSize; i < (HashSize*2); ++i) {
                hash_[i] = hash_[i-HashSize];
            }
        }

#if false
        public static float value1D(float point, float frequency)
        {
            point *= frequency;
            int ix0 = Mathf.FloorToInt(point);
            float t = fade(point-ix0);
            ix0 &= HashMask;
            int ix1 = ix0+1;
            int h0 = hash_[ix0];
            int h1 = hash_[ix1];
            return Mathf.Lerp(h0, h1, t) * (1.0f/HashMask);
        }

        public static float value2D(Vector2 point, float frequency)
        {
            point *= frequency;
            int ix0 = Mathf.FloorToInt(point.x);
            int iy0 = Mathf.FloorToInt(point.y);
            float tx = fade(point.x-ix0);
            float ty = fade(point.y-iy0);
            ix0 &= HashMask;
            iy0 &= HashMask;
            int ix1 = ix0+1;
            int iy1 = iy0+1;

            int h0 = hash_[ix0];
            int h1 = hash_[ix1];
            int h00 = hash_[h0+iy0];
            int h10 = hash_[h1+iy0];
            int h01 = hash_[h0+iy1];
            int h11 = hash_[h1+iy1];

            return Mathf.Lerp(Mathf.Lerp(h00, h10, tx), Mathf.Lerp(h01, h11, tx), ty) * (1.0f/HashMask);
        }

        public static float value3D(Vector3 point, float frequency)
        {
            point *= frequency;
            int ix0 = Mathf.FloorToInt(point.x);
            int iy0 = Mathf.FloorToInt(point.y);
            int iz0 = Mathf.FloorToInt(point.z);
            float tx = fade(point.x-ix0);
            float ty = fade(point.y-iy0);
            float tz = fade(point.z-iz0);
            ix0 &= HashMask;
            iy0 &= HashMask;
            iz0 &= HashMask;
            int ix1 = ix0+1;
            int iy1 = iy0+1;
            int iz1 = iz0+1;

            int h0 = hash_[ix0];
            int h1 = hash_[ix1];
            int h00 = hash_[h0+iy0];
            int h10 = hash_[h1+iy0];
            int h01 = hash_[h0+iy1];
            int h11 = hash_[h1+iy1];

            int h000 = hash_[h00+iz0];
            int h100 = hash_[h10+iz0];
            int h010 = hash_[h01+iz0];
            int h110 = hash_[h11+iz0];
            int h001 = hash_[h00+iz1];
            int h101 = hash_[h10+iz1];
            int h011 = hash_[h01+iz1];
            int h111 = hash_[h11+iz1];
            return Mathf.Lerp(
                Mathf.Lerp(Mathf.Lerp(h000, h100, tx), Mathf.Lerp(h010, h110, tx), ty),
                Mathf.Lerp(Mathf.Lerp(h001, h101, tx), Mathf.Lerp(h011, h111, tx), ty),
                tz) * (1.0f/HashMask);
        }


        public static float perlin1D(float point, float frequency)
        {
            point *= frequency;
            int ix0 = Mathf.FloorToInt(point);
            float t0 = point-ix0;
            float t1 = t0-1.0f;
            ix0 &= HashMask;
            int ix1 = ix0+1;

            float g0 = grad3_[hash_[ix0]&0x01, 0];
            float g1 = grad3_[hash_[ix1]&0x01, 0];
            float v0 = g0*t0;
            float v1 = g1*t1;
            float t = fade(t0);
            return Mathf.Lerp(v0, v1, t)*2.0f;
        }

        public static float perlin2D(Vector2 point, float frequency)
        {
            point *= frequency;
            int ix0 = Mathf.FloorToInt(point.x);
            int iy0 = Mathf.FloorToInt(point.y);
            float tx0 = point.x-ix0;
            float ty0 = point.y-iy0;
            float tx1 = tx0-1.0f;
            float ty1 = ty0-1.0f;
            ix0 &= HashMask;
            iy0 &= HashMask;
            int ix1 = ix0+1;
            int iy1 = iy0+1;

            int h0 = hash_[ix0];
            int h1 = hash_[ix1];
            int h00 = hash_[h0+iy0] & 0x07;
            int h10 = hash_[h1+iy0] & 0x07;
            int h01 = hash_[h0+iy1] & 0x07;
            int h11 = hash_[h1+iy1] & 0x07;

            float v00 = dot2(grad2_[h00], tx0, ty0);
            float v10 = dot2(grad2_[h10], tx1, ty0);
            float v01 = dot2(grad2_[h01], tx0, ty1);
            float v11 = dot2(grad2_[h11], tx1, ty1);

            float tx = fade(tx0);
            float ty = fade(ty0);

            return Mathf.Lerp(Mathf.Lerp(v00, v10, tx), Mathf.Lerp(v01, v11, tx), ty) * 1.41421356f;
        }

        public static float perlin3D(Vector3 point, float frequency)
        {
            point *= frequency;
            int ix0 = Mathf.FloorToInt(point.x);
            int iy0 = Mathf.FloorToInt(point.y);
            int iz0 = Mathf.FloorToInt(point.z);
            float tx0 = point.x-ix0;
            float ty0 = point.y-iy0;
            float tz0 = point.z-iz0;
            float tx1 = tx0-1.0f;
            float ty1 = ty0-1.0f;
            float tz1 = tz0-1.0f;
            ix0 &= HashMask;
            iy0 &= HashMask;
            iz0 &= HashMask;
            int ix1 = ix0+1;
            int iy1 = iy0+1;
            int iz1 = iz0+1;

            int h0 = hash_[ix0];
            int h1 = hash_[ix1];
            int h00 = hash_[h0+iy0];
            int h10 = hash_[h1+iy0];
            int h01 = hash_[h0+iy1];
            int h11 = hash_[h1+iy1];

            int h000 = hash_[h00+iz0] & 0x0F;
            int h100 = hash_[h10+iz0] & 0x0F;
            int h010 = hash_[h01+iz0] & 0x0F;
            int h110 = hash_[h11+iz0] & 0x0F;
            int h001 = hash_[h00+iz1] & 0x0F;
            int h101 = hash_[h10+iz1] & 0x0F;
            int h011 = hash_[h01+iz1] & 0x0F;
            int h111 = hash_[h11+iz1] & 0x0F;

            float v000 = dot3(grad3_, h000, tx0, ty0, tz0);
            float v100 = dot3(grad3_, h100, tx1, ty0, tz0);
            float v010 = dot3(grad3_, h010, tx0, ty1, tz0);
            float v110 = dot3(grad3_, h110, tx1, ty1, tz0);
            float v001 = dot3(grad3_, h001, tx0, ty0, tz1);
            float v101 = dot3(grad3_, h101, tx1, ty0, tz1);
            float v011 = dot3(grad3_, h011, tx0, ty1, tz1);
            float v111 = dot3(grad3_, h111, tx1, ty1, tz1);

            float tx = fade(tx0);
            float ty = fade(ty0);
            float tz = fade(tz0);

            return Mathf.Lerp(
                Mathf.Lerp(Mathf.Lerp(v000, v100, tx), Mathf.Lerp(v010, v110, tx), ty),
                Mathf.Lerp(Mathf.Lerp(v001, v101, tx), Mathf.Lerp(v011, v111, tx), ty),
                tz);
        }

#else
        public static NoiseSample value1D(float point, float frequency)
        {
            point *= frequency;
            int ix0 = Mathf.FloorToInt(point);
            float t = point-ix0;
            float dt = fadeDerivative(t);
            t = fade(t);
            ix0 &= HashMask;
            int ix1 = ix0+1;
            int h0 = hash_[ix0];
            int h1 = hash_[ix1];

            NoiseSample sample;
            sample.value_ = Mathf.Lerp(h0, h1, t);
            sample.derivative_.x = dt * (h0-h1);
            sample.derivative_.y = 0.0f;
            sample.derivative_.z = 0.0f;
            sample.derivative_ *= frequency;
            return sample * (1.0f / HashMask);
        }

        public static NoiseSample value2D(Vector2 point, float frequency)
        {
            point *= frequency;
            int ix0 = Mathf.FloorToInt(point.x);
            int iy0 = Mathf.FloorToInt(point.y);
            float tx = point.x-ix0;
            float ty = point.y-iy0;
            float dtx = fadeDerivative(tx);
            float dty = fadeDerivative(ty);
            tx = fade(tx);
            ty = fade(ty);
            ix0 &= HashMask;
            iy0 &= HashMask;
            int ix1 = ix0+1;
            int iy1 = iy0+1;

            int h0 = hash_[ix0];
            int h1 = hash_[ix1];
            int h00 = hash_[h0+iy0];
            int h10 = hash_[h1+iy0];
            int h01 = hash_[h0+iy1];
            int h11 = hash_[h1+iy1];

            float b = h10 - h00;
            float c = h01 - h00;
            float d = h11 - h01 - h10 + h00;

            NoiseSample sample;
            sample.value_ = Mathf.Lerp(Mathf.Lerp(h00, h10, tx), Mathf.Lerp(h01, h11, tx), ty);
            sample.derivative_.x = (b + d*ty) * dtx;
            sample.derivative_.y = (c + d*tx) * dty;
            sample.derivative_.z = 0.0f;
            sample.derivative_ *= frequency;
            return sample * (1.0f / HashMask);
        }

        public static NoiseSample value3D(Vector3 point, float frequency)
        {
            point *= frequency;
            int ix0 = Mathf.FloorToInt(point.x);
            int iy0 = Mathf.FloorToInt(point.y);
            int iz0 = Mathf.FloorToInt(point.z);
            float tx = point.x-ix0;
            float ty = point.y-iy0;
            float tz = point.z-iz0;
            float dtx = fadeDerivative(tx);
            float dty = fadeDerivative(ty);
            float dtz = fadeDerivative(tz);
            tx = fade(tx);
            ty = fade(ty);
            tz = fade(tz);
            ix0 &= HashMask;
            iy0 &= HashMask;
            iz0 &= HashMask;
            int ix1 = ix0+1;
            int iy1 = iy0+1;
            int iz1 = iz0+1;

            int h0 = hash_[ix0];
            int h1 = hash_[ix1];
            int h00 = hash_[h0+iy0];
            int h10 = hash_[h1+iy0];
            int h01 = hash_[h0+iy1];
            int h11 = hash_[h1+iy1];

            int h000 = hash_[h00+iz0];
            int h100 = hash_[h10+iz0];
            int h010 = hash_[h01+iz0];
            int h110 = hash_[h11+iz0];
            int h001 = hash_[h00+iz1];
            int h101 = hash_[h10+iz1];
            int h011 = hash_[h01+iz1];
            int h111 = hash_[h11+iz1];

            float b = h100 - h000;
            float c = h010 - h000;
            float d = h001 - h000;
            float e = h110 - h010 - h100 + h000;
            float f = h101 - h001 - h100 + h000;
            float g = h011 - h001 - h010 + h000;
            float h = h111 - h011 - h101 + h001 - h110 + h010 + h100 - h000;

            NoiseSample sample;
            sample.value_ = Mathf.Lerp(
                Mathf.Lerp(Mathf.Lerp(h000, h100, tx), Mathf.Lerp(h010, h110, tx), ty),
                Mathf.Lerp(Mathf.Lerp(h001, h101, tx), Mathf.Lerp(h011, h111, tx), ty),
                tz);
            sample.derivative_.x = (b + e * ty + (f + h * ty) * tz) * dtx;
            sample.derivative_.y = (c + e * tx + (g + h * tx) * tz) * dty;
            sample.derivative_.z = (d + f * tx + (g + h * tx) * ty) * dtz;
            sample.derivative_ *= frequency;
            return sample * (1.0f / HashMask);
        }


        public static NoiseSample perlin1D(float point, float frequency)
        {
            point *= frequency;
            int ix0 = Mathf.FloorToInt(point);
            float t0 = point-ix0;
            float t1 = t0-1.0f;
            ix0 &= HashMask;
            int ix1 = ix0+1;

            float g0 = grad3_[hash_[ix0]&0x01, 0];
            float g1 = grad3_[hash_[ix1]&0x01, 0];
            float v0 = g0*t0;
            float v1 = g1*t1;

            float dt = fadeDerivative(t0);
            float t = fade(t0);

            float a = v0;
            float b = v1 - v0;

            float da = g0;
            float db = g1 - g0;

            NoiseSample sample;
            sample.value_ = a + b*t;
            sample.derivative_.x = da + db * t + b*dt;
            sample.derivative_.y = 0.0f;
            sample.derivative_.z = 0.0f;
            sample.derivative_ *= frequency;

            return sample*2.0f;
        }

        public static NoiseSample perlin2D(Vector2 point, float frequency)
        {
            point *= frequency;
            int ix0 = Mathf.FloorToInt(point.x);
            int iy0 = Mathf.FloorToInt(point.y);
            float tx0 = point.x-ix0;
            float ty0 = point.y-iy0;
            float tx1 = tx0-1.0f;
            float ty1 = ty0-1.0f;
            ix0 &= HashMask;
            iy0 &= HashMask;
            int ix1 = ix0+1;
            int iy1 = iy0+1;

            int h0 = hash_[ix0];
            int h1 = hash_[ix1];
            int h00 = hash_[h0+iy0] & 0x07;
            int h10 = hash_[h1+iy0] & 0x07;
            int h01 = hash_[h0+iy1] & 0x07;
            int h11 = hash_[h1+iy1] & 0x07;

            Vector2 g00 = grad2_[h00];
            Vector2 g10 = grad2_[h10];
            Vector2 g01 = grad2_[h01];
            Vector2 g11 = grad2_[h11];

            float v00 = dot2(g00, tx0, ty0);
            float v10 = dot2(g10, tx1, ty0);
            float v01 = dot2(g01, tx0, ty1);
            float v11 = dot2(g11, tx1, ty1);

            float dtx = fadeDerivative(tx0);
            float dty = fadeDerivative(ty0);
            float tx = fade(tx0);
            float ty = fade(ty0);

            float a = v00;
            float b = v10 - v00;
            float c = v01 - v00;
            float d = v11 - v01 - v10 + v00;

            Vector2 da = g00;
            Vector2 db = g10 - g00;
            Vector2 dc = g01 - g00;
            Vector2 dd = g11 - g01 - g10 + g00;

            NoiseSample sample;
            sample.value_ = a + b*tx + (c + d*tx)*ty;
            sample.derivative_ = da + db*tx + (dc + dd*tx)*ty;
            sample.derivative_.x += (b + d*ty) * dtx;
            sample.derivative_.y += (c + d*tx) * dty;
            sample.derivative_.z = 0.0f;
            sample.derivative_ *= frequency;

            return sample * 1.41421356f;
        }

        public static NoiseSample perlin3D(Vector3 point, float frequency)
        {
            point *= frequency;
            int ix0 = Mathf.FloorToInt(point.x);
            int iy0 = Mathf.FloorToInt(point.y);
            int iz0 = Mathf.FloorToInt(point.z);
            float tx0 = point.x-ix0;
            float ty0 = point.y-iy0;
            float tz0 = point.z-iz0;
            float tx1 = tx0-1.0f;
            float ty1 = ty0-1.0f;
            float tz1 = tz0-1.0f;
            ix0 &= HashMask;
            iy0 &= HashMask;
            iz0 &= HashMask;
            int ix1 = ix0+1;
            int iy1 = iy0+1;
            int iz1 = iz0+1;

            int h0 = hash_[ix0];
            int h1 = hash_[ix1];
            int h00 = hash_[h0+iy0];
            int h10 = hash_[h1+iy0];
            int h01 = hash_[h0+iy1];
            int h11 = hash_[h1+iy1];

            int h000 = hash_[h00+iz0] & 0x0F;
            int h100 = hash_[h10+iz0] & 0x0F;
            int h010 = hash_[h01+iz0] & 0x0F;
            int h110 = hash_[h11+iz0] & 0x0F;
            int h001 = hash_[h00+iz1] & 0x0F;
            int h101 = hash_[h10+iz1] & 0x0F;
            int h011 = hash_[h01+iz1] & 0x0F;
            int h111 = hash_[h11+iz1] & 0x0F;

            Vector3 g000 = grad3(grad3_, h000);
            Vector3 g100 = grad3(grad3_, h100);
            Vector3 g010 = grad3(grad3_, h010);
            Vector3 g110 = grad3(grad3_, h110);
            Vector3 g001 = grad3(grad3_, h001);
            Vector3 g101 = grad3(grad3_, h101);
            Vector3 g011 = grad3(grad3_, h011);
            Vector3 g111 = grad3(grad3_, h111);

            float v000 = dot3(grad3_, h000, tx0, ty0, tz0);
            float v100 = dot3(grad3_, h100, tx1, ty0, tz0);
            float v010 = dot3(grad3_, h010, tx0, ty1, tz0);
            float v110 = dot3(grad3_, h110, tx1, ty1, tz0);
            float v001 = dot3(grad3_, h001, tx0, ty0, tz1);
            float v101 = dot3(grad3_, h101, tx1, ty0, tz1);
            float v011 = dot3(grad3_, h011, tx0, ty1, tz1);
            float v111 = dot3(grad3_, h111, tx1, ty1, tz1);

            float dtx = fadeDerivative(tx0);
            float dty = fadeDerivative(ty0);
            float dtz = fadeDerivative(tz0);
            float tx = fade(tx0);
            float ty = fade(ty0);
            float tz = fade(tz0);

            float a = v000;
            float b = v100 - v000;
            float c = v010 - v000;
            float d = v001 - v000;
            float e = v110 - v010 - v100 + v000;
            float f = v101 - v001 - v100 + v000;
            float g = v011 - v001 - v010 + v000;
            float h = v111 - v011 - v101 + v001 - v110 + v010 + v100 - v000;

            Vector3 da = g000;
            Vector3 db = g100 - g000;
            Vector3 dc = g010 - g000;
            Vector3 dd = g001 - g000;
            Vector3 de = g110 - g010 - g100 + g000;
            Vector3 df = g101 - g001 - g100 + g000;
            Vector3 dg = g011 - g001 - g010 + g000;
            Vector3 dh = g111 - g011 - g101 + g001 - g110 + g010 + g100 - g000;

            NoiseSample sample;
            sample.value_ = a + b * tx + (c + e * tx) * ty + (d + f * tx + (g + h * tx) * ty) * tz;
            sample.derivative_ = da + db * tx + (dc + de * tx) * ty + (dd + df * tx + (dg + dh * tx) * ty) * tz;
            sample.derivative_.x += (b + e * ty + (f + h * ty) * tz) * dtx;
            sample.derivative_.y += (c + e * tx + (g + h * tx) * tz) * dty;
            sample.derivative_.z += (d + f * tx + (g + h * tx) * ty) * dtz;
            sample.derivative_ *= frequency;
            return sample;
        }
#endif
    };
#endif
}

#endif //INC_LCORE_NOISE_H__
