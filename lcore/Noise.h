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
    struct NoiseSample1
    {
    public:
        NoiseSample1()
        {}

        NoiseSample1(f32 value, f32 dx)
            :value_(value)
            ,dx_(dx)
        {}

        void add(f32 f, const NoiseSample1& x);

        friend NoiseSample1 operator+(const NoiseSample1& x0, const NoiseSample1& x1);
        friend NoiseSample1 operator+(const NoiseSample1& x, f32 f);
        friend NoiseSample1 operator+(f32 f, const NoiseSample1& x);
        friend NoiseSample1 operator-(const NoiseSample1& x0, const NoiseSample1& x1);
        friend NoiseSample1 operator-(const NoiseSample1& x, f32 f);
        friend NoiseSample1 operator-(f32 f, const NoiseSample1& x);
        friend NoiseSample1 operator*(const NoiseSample1& x0, const NoiseSample1& x1);
        friend NoiseSample1 operator*(const NoiseSample1& x, f32 f);
        friend NoiseSample1 operator*(f32 f, const NoiseSample1& x);

        f32 value_;
        f32 dx_; //derivative x
    };

    inline NoiseSample1 operator+(const NoiseSample1& x0, const NoiseSample1& x1)
    {
        return NoiseSample1(x0.value_ + x1.value_, x0.dx_ + x1.dx_);
    }

    inline NoiseSample1 operator+(const NoiseSample1& x, f32 f)
    {
        return NoiseSample1(x.value_ + f, x.dx_);
    }

    inline NoiseSample1 operator+(f32 f, const NoiseSample1& x)
    {
        return NoiseSample1(f + x.value_, x.dx_);
    }

    inline NoiseSample1 operator-(const NoiseSample1& x0, const NoiseSample1& x1)
    {
        return NoiseSample1(x0.value_ - x1.value_, x0.dx_ - x1.dx_);
    }

    inline NoiseSample1 operator-(const NoiseSample1& x, f32 f)
    {
        return NoiseSample1(x.value_ - f, x.dx_);
    }

    inline NoiseSample1 operator-(f32 f, const NoiseSample1& x)
    {
        return NoiseSample1(f - x.value_, -x.dx_);
    }

    NoiseSample1 operator*(const NoiseSample1& x0, const NoiseSample1& x1);
    NoiseSample1 operator*(const NoiseSample1& x, f32 f);
    NoiseSample1 operator*(f32 f, const NoiseSample1& x);

    struct NoiseSample2
    {
    public:
        NoiseSample2()
        {}

        NoiseSample2(f32 value, f32 dx, f32 dy)
            :value_(value)
            ,dx_(dx)
            ,dy_(dy)
        {}

        void add(f32 f, const NoiseSample2& x);

        friend NoiseSample2 operator+(const NoiseSample2& x0, const NoiseSample2& x1);
        friend NoiseSample2 operator+(const NoiseSample2& x, f32 f);
        friend NoiseSample2 operator+(f32 f, const NoiseSample2& x);
        friend NoiseSample2 operator-(const NoiseSample2& x0, const NoiseSample2& x1);
        friend NoiseSample2 operator-(const NoiseSample2& x, f32 f);
        friend NoiseSample2 operator-(f32 f, const NoiseSample2& x);
        friend NoiseSample2 operator*(const NoiseSample2& x0, const NoiseSample2& x1);
        friend NoiseSample2 operator*(const NoiseSample2& x, f32 f);
        friend NoiseSample2 operator*(f32 f, const NoiseSample2& x);

        f32 value_;
        f32 dx_; //derivative x
        f32 dy_; //derivative y
    };

    inline NoiseSample2 operator+(const NoiseSample2& x0, const NoiseSample2& x1)
    {
        return NoiseSample2(x0.value_ + x1.value_, x0.dx_ + x1.dx_, x0.dy_ + x1.dy_);
    }

    inline NoiseSample2 operator+(const NoiseSample2& x, f32 f)
    {
        return NoiseSample2(x.value_ + f, x.dx_, x.dy_);
    }

    inline NoiseSample2 operator+(f32 f, const NoiseSample2& x)
    {
        return NoiseSample2(f + x.value_, x.dx_, x.dy_);
    }

    inline NoiseSample2 operator-(const NoiseSample2& x0, const NoiseSample2& x1)
    {
        return NoiseSample2(x0.value_ - x1.value_, x0.dx_ - x1.dx_, x0.dy_ - x1.dy_);
    }

    inline NoiseSample2 operator-(const NoiseSample2& x, f32 f)
    {
        return NoiseSample2(x.value_ - f, x.dx_, x.dy_);
    }

    inline NoiseSample2 operator-(f32 f, const NoiseSample2& x)
    {
        return NoiseSample2(f - x.value_, -x.dx_, -x.dy_);
    }

    NoiseSample2 operator*(const NoiseSample2& x0, const NoiseSample2& x1);
    NoiseSample2 operator*(const NoiseSample2& x, f32 f);
    NoiseSample2 operator*(f32 f, const NoiseSample2& x);

    struct NoiseSample3
    {
    public:
        NoiseSample3()
        {}

        NoiseSample3(f32 value, f32 dx, f32 dy, f32 dz)
            :value_(value)
            ,dx_(dx)
            ,dy_(dy)
            ,dz_(dz)
        {}

        void add(f32 f, const NoiseSample3& x);

        friend NoiseSample3 operator+(const NoiseSample3& x0, const NoiseSample3& x1);
        friend NoiseSample3 operator+(const NoiseSample3& x, f32 f);
        friend NoiseSample3 operator+(f32 f, const NoiseSample3& x);
        friend NoiseSample3 operator-(const NoiseSample3& x0, const NoiseSample3& x1);
        friend NoiseSample3 operator-(const NoiseSample3& x, f32 f);
        friend NoiseSample3 operator-(f32 f, const NoiseSample3& x);
        friend NoiseSample3 operator*(const NoiseSample3& x0, const NoiseSample3& x1);
        friend NoiseSample3 operator*(const NoiseSample3& x, f32 f);
        friend NoiseSample3 operator*(f32 f, const NoiseSample3& x);

        f32 value_;
        f32 dx_; //derivative x
        f32 dy_; //derivative y
        f32 dz_; //derivative z
    };

    inline NoiseSample3 operator+(const NoiseSample3& x0, const NoiseSample3& x1)
    {
        return NoiseSample3(x0.value_ + x1.value_, x0.dx_ + x1.dx_, x0.dy_ + x1.dy_, x0.dz_ + x1.dz_);
    }

    inline NoiseSample3 operator+(const NoiseSample3& x, f32 f)
    {
        return NoiseSample3(x.value_ + f, x.dx_, x.dy_, x.dz_);
    }

    inline NoiseSample3 operator+(f32 f, const NoiseSample3& x)
    {
        return NoiseSample3(f + x.value_, x.dx_, x.dy_, x.dz_);
    }

    inline NoiseSample3 operator-(const NoiseSample3& x0, const NoiseSample3& x1)
    {
        return NoiseSample3(x0.value_ - x1.value_, x0.dx_ - x1.dx_, x0.dy_ - x1.dy_, x0.dz_ - x1.dz_);
    }

    inline NoiseSample3 operator-(const NoiseSample3& x, f32 f)
    {
        return NoiseSample3(x.value_ - f, x.dx_, x.dy_, x.dz_);
    }

    inline NoiseSample3 operator-(f32 f, const NoiseSample3& x)
    {
        return NoiseSample3(f - x.value_, -x.dx_, -x.dy_, -x.dz_);
    }

    NoiseSample3 operator*(const NoiseSample3& x0, const NoiseSample3& x1);
    NoiseSample3 operator*(const NoiseSample3& x, f32 f);
    NoiseSample3 operator*(f32 f, const NoiseSample3& x);

    //------------------------------------------------
    //---
    //--- Noise
    //---
    //------------------------------------------------
    class Noise
    {
    public:
        struct Vector2
        {
        public:
            Vector2()
            {}

            Vector2(f32 x, f32 y)
                :x_(x)
                ,y_(y)
            {}

            friend inline Vector2 operator+(const Vector2& x0, const Vector2& x1)
            {
                return Vector2(x0.x_+x1.x_, x0.y_+x1.y_);
            }

            friend inline Vector2 operator-(const Vector2& x0, const Vector2& x1)
            {
                return Vector2(x0.x_-x1.x_, x0.y_-x1.y_);
            }
            f32 x_;
            f32 y_;
        };

        struct Vector3
        {
        public:
            Vector3()
            {}

            Vector3(f32 x, f32 y, f32 z)
                :x_(x)
                ,y_(y)
                ,z_(z)
            {}

            friend inline Vector3 operator+(const Vector3& x0, const Vector3& x1)
            {
                return Vector3(x0.x_+x1.x_, x0.y_+x1.y_, x0.z_+x1.z_);
            }

            friend inline Vector3 operator-(const Vector3& x0, const Vector3& x1)
            {
                return Vector3(x0.x_-x1.x_, x0.y_-x1.y_, x0.z_-x1.z_);
            }
            f32 x_;
            f32 y_;
            f32 z_;
        };

        typedef NoiseSample1(Noise::*Func1D)(f32 px, f32 frequency) const;
        typedef NoiseSample2(Noise::*Func2D)(f32 px, f32 py, f32 frequency) const;
        typedef NoiseSample3(Noise::*Func3D)(f32 px, f32 py, f32 pz, f32 frequency) const;

        static const s32 PermutationTableSize = 256*2;
        static const u32 PermutationTableSizeMask = (PermutationTableSize>>1)-1;
        static const s32 NumGrad2 = 8;
        static const s32 NumGrad3 = 16;

        static s32 fastFloor(f32 x)
        {
            return (0.0f < x) ? (s32)x : (s32)x - 1;
        }

        //static f32 dot2(const s8** grad, s32 i, f32 x, f32 y)
        //{
        //    return (x * grad[i][0] + y * grad[i][1]);
        //}

        static f32 dot2(const Vector2& grad, f32 x, f32 y)
        {
            return (x * grad.x_ + y * grad.y_);
        }

        static f32 dot3(const s8 grad3[NumGrad3][3], s32 i, f32 x, f32 y, f32 z)
        {
            return (x * grad3[i][0] + y * grad3[i][1] + z * grad3[i][2]);
        }

        //static f32 dot4(const s8** grad, s32 i, f32 x, f32 y, f32 z, f32 w)
        //{
        //    return (x * grad[i][0] + y * grad[i][1] + z * grad[i][2] + w * grad[i][3]);
        //}

        static Vector3 grad3(const s8 grad3[NumGrad3][3], s32 i)
        {
            return Vector3(grad3[i][0], grad3[i][1], grad3[i][2]);
        }

        static f32 fade(f32 x)
        {
            return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
        }


        static f32 fadeDerivative(f32 x)
        {
            return 30.0f * x * x * (x * (x - 2.0f) + 1.0f);
        }

        explicit Noise(u64 seed=1U);

        void permutate(u64 seed);

        NoiseSample2 fbm(Func2D func2D, f32 px, f32 py, s32 octave, f32 frequency, f32 lacunarity=2.0f, f32 persistence=0.5f) const;
        NoiseSample3 fbm(Func3D func3D, f32 px, f32 py, f32 pz, s32 octave, f32 frequency, f32 lacunarity=2.0f, f32 persistence=0.5f) const;

        //NoiseSample2 turbulence(Func2D func2D, f32 px, f32 py, s32 octave, f32 frequency, f32 lacunarity=2.0f, f32 persistence=0.5f) const;
        //NoiseSample3 turbulence(Func3D func3D, f32 px, f32 py, f32 pz, s32 octave, f32 frequency, f32 lacunarity=2.0f, f32 persistence=0.5f) const;

        //NoiseSample2 multiFractal(Func2D func2D, f32 px, f32 py, s32 octave, f32 frequency, f32 lacunarity=2.0f, f32 persistence=0.5f) const;
        //NoiseSample3 multiFractal(Func3D func3D, f32 px, f32 py, f32 pz, s32 octave, f32 frequency, f32 lacunarity=2.0f, f32 persistence=0.5f) const;

        //NoiseSample2 ridgedMultiFractal(Func2D func2D, f32 px, f32 py, s32 octave, f32 frequency, f32 lacunarity=2.0f, f32 persistence=0.5f) const;
        //NoiseSample3 ridgedMultiFractal(Func3D func3D, f32 px, f32 py, f32 pz, s32 octave, f32 frequency, f32 lacunarity=2.0f, f32 persistence=0.5f) const;

        //-------------------------------------------------------
        NoiseSample1 value1D(f32 px, f32 frequency) const;
        NoiseSample2 value2D(f32 px, f32 py, f32 frequency) const;
        NoiseSample3 value3D(f32 px, f32 py, f32 pz, f32 frequency) const;

        NoiseSample1 perlin1D(f32 px, f32 frequency) const;
        NoiseSample2 perlin2D(f32 px, f32 py, f32 frequency) const;
        NoiseSample3 perlin3D(f32 px, f32 py, f32 pz, f32 frequency) const;

    private:
        static const Vector2 grad2_[NumGrad2];
        static const s8 grad3_[NumGrad3][3];
        //static const s8 simple_[64][4];

        u8 permutation_[PermutationTableSize];
    };
}

#endif //INC_LCORE_NOISE_H__
