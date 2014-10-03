/**
@file Noise.cpp
@author t-sakai
@date 2014/05/08 create
*/
#include "Noise.h"
#include "random/Random.h"
#include "clibrary.h"

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

    //------------------------------------------------
    //---
    //--- Perlin
    //---
    //------------------------------------------------
    inline s32 Perlin::fastFloor(f32 x)
    {
        return (0.0f<x)? (s32)x : (s32)x-1;
    }

    inline void Perlin::fastFloor2(s32* ret, f32 x, f32 y, f32 z)
    {
        __m128 v = _mm_set_ps(0.0f, z, y, x);
        __m128i x0 = _mm_cvttps_epi32(v);
        _mm_store_si128((__m128i*)ret, x0);

        ret[0] = (0.0f<x)? ret[0] : ret[0]-1;
        ret[1] = (0.0f<y)? ret[1] : ret[1]-1;
        ret[2] = (0.0f<z)? ret[2] : ret[2]-1;

        //__m128i x1 = _mm_sub_epi32(x0, _mm_set1_epi32(1));
        //__m128i mask = _mm_castps_si128(_mm_cmplt_ps(_mm_setzero_ps(), v));
        //__m128i r = _mm_or_si128(_mm_and_si128(mask, x0), _mm_andnot_si128(mask, x1));
        //_mm_store_si128((__m128i*)ret, r);
    }

    inline f32 Perlin::dot2(const s8 grad[2], f32 x, f32 y)
    {
        return (x*grad[0] + y*grad[1]);
    }

    inline f32 Perlin::dot3(const s8 grad[3], f32 x, f32 y, f32 z)
    {
        return (x*grad[0] + y*grad[1] + z*grad[2]);
    }

    inline f32 Perlin::dot4(const s8 grad[4], f32 x, f32 y, f32 z, f32 w)
    {
        return (x*grad[0] + y*grad[1] + z*grad[2] + w*grad[3]);
    }

    inline f32 Perlin::fade(f32 x)
    {
        return x*x*x*(x*(x*6.0f-15.0f)+10.0f);
    }

    s8 Perlin::grad3_[12][3] =
    {
        {1,1,0},{-1,1,0},{1,-1,0},
        {-1,-1,0},{1,0,1},{-1,0,1},
        {1,0,-1},{-1,0,-1},{0,1,1},
        {0,-1,1},{0,1,-1},{0,-1,-1},
    };

    u8 Perlin::simplex_[64][4] =
    {
        {0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
        {0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
        {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
        {1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
        {1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
        {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
        {2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
        {2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}
    };

    Perlin::Perlin()
    {
        s32 size = PermutationTableSize>>1;
        for(s32 i=0; i<size; ++i){
            permutation_[i] = static_cast<u8>(i);
        }

        for(s32 i=size; i<PermutationTableSize; ++i){
            permutation_[i] = permutation_[i-size];
        }
    }

    Perlin::~Perlin()
    {
    }

    void Perlin::random(u32 seed)
    {
        lcore::RandomWELL rand;
        rand.srand(seed);

        s32 size = PermutationTableSize>>1;
        for(s32 i=0; i<size; ++i){
            s32 index = lcore::random::range(rand, i, size-1);
            lcore::swap(permutation_[i], permutation_[index]);
        }

        for(s32 i=size; i<PermutationTableSize; ++i){
            permutation_[i] = permutation_[i-size];
        }
    }

    f32 Perlin::operator()(f32 x, f32 y) const
    {
        const f32 F2 = 0.366025403f; //F2 = 0.5*(sqrt(3.0)-1.0)
        const f32 G2 = 0.211324865f; //G2 = (3.0-Math.sqrt(3.0))/6.0

        f32 s = (x+y)*F2;
        s32 ix0 = fastFloor(x + s);
        s32 iy0 = fastFloor(y + s);

        f32 t = (f32)(ix0+iy0)*G2;
        f32 x0 = x-(ix0-t);
        f32 y0 = y-(iy0-t);

        s32 ix1, iy1;
        if(y0<x0){
            ix1 = 1;
            iy1 = 0;
        }else{
            ix1 = 0;
            iy1 = 1;
        }

        f32 x1 = x0 - ix1 + G2;
        f32 y1 = y0 - iy1 + G2;
        f32 x2 = x0 - 1.0f + 2.0f*G2;
        f32 y2 = y0 - 1.0f + 2.0f*G2;

        s32 ixx = ix0 & 0xFFU;
        s32 iyy = iy0 & 0xFFU;

        f32 n0, n1, n2;

        f32 t0 = 0.5f - x0*x0 - y0*y0;
        if(t0<0.0f){
            n0 = 0.0f;
        }else{
            s32 g0 = permutation_[ixx+      permutation_[iyy]] % NumGrad3;

            t0 *= t0;
            n0 = t0*t0*dot2(grad3_[g0], x0, y0);
        }

        f32 t1 = 0.5f - x1*x1 - y1*y1;
        if(t1<0.0f){
            n1 = 0.0f;
        }else{
            s32 g1 = permutation_[ixx+ix1 + permutation_[iyy+iy1]] % NumGrad3;

            t1 *= t1;
            n1 = t1*t1*dot2(grad3_[g1], x1, y1);
        }

        f32 t2 = 0.5f - x2*x2 - y2*y2;
        if(t2<0.0f){
            n2 = 0.0f;
        }else{
            s32 g2 = permutation_[ixx+1+    permutation_[iyy+1]] % NumGrad3;

            t2 *= t2;
            n2 = t2*t2*dot2(grad3_[g2], x2, y2);
        }

        return 70.0f * (n0 + n1 + n2);
    }

    f32 Perlin::cpu_get(f32 x, f32 y, f32 z) const
    {
        static const f32 F3 = (1.0f/3.0f);
        static const f32 G3 = (1.0f/6.0f);

        f32 s = (x+y+z)*F3;

        LIME_ALIGN16 s32 iv0[4];
        fastFloor2(iv0, x+s, y+s, z+s);
        const s32& ix0 = iv0[0];//fastFloor2(x + s);
        const s32& iy0 = iv0[1];//fastFloor2(y + s);
        const s32& iz0 = iv0[2];//fastFloor2(z + s);
        
        f32 t = (f32)(ix0+iy0+iz0)*G3;
        f32 x0 = x-(ix0-t);
        f32 y0 = y-(iy0-t);
        f32 z0 = z-(iz0-t);

        s32 ix1, iy1, iz1;
        s32 ix2, iy2, iz2;

        if(y0<=x0){
            if(z0<=y0){
                ix1=1; iy1=0; iz1=0; ix2=1; iy2=1; iz2=0;
            }else if(z0<=x0){
                ix1=1; iy1=0; iz1=0; ix2=1; iy2=0; iz2=1;
            }else{
                ix1=0; iy1=0; iz1=1; ix2=1; iy2=0; iz2=1;
            }
        }else{
            if(y0<z0){
                ix1=0; iy1=0; iz1=1; ix2=0; iy2=1; iz2=1;
            }else if(x0<z0){
                ix1=0; iy1=1; iz1=0; ix2=0; iy2=1; iz2=1;
            }else{
                ix1=0; iy1=1; iz1=0; ix2=1; iy2=1; iz2=0;
            }
        }


        f32 x1 = x0 - ix1 + G3;
        f32 y1 = y0 - iy1 + G3;
        f32 z1 = z0 - iz1 + G3;
        f32 x2 = x0 - ix2 + 2.0f*G3;
        f32 y2 = y0 - iy2 + 2.0f*G3;
        f32 z2 = z0 - iz2 + 2.0f*G3;
        f32 x3 = x0 - 1.0f + 3.0f*G3;
        f32 y3 = y0 - 1.0f + 3.0f*G3;
        f32 z3 = z0 - 1.0f + 3.0f*G3;

        s32 ixx = ix0 & 0xFFU;
        s32 iyy = iy0 & 0xFFU;
        s32 izz = iz0 & 0xFFU;

        f32 n0, n1, n2, n3;

        f32 t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;
        if(t0<0.0f){
            n0 = 0.0f;
        }else{
            s32 g0 = permutation_[ixx     + permutation_[iyy     + permutation_[izz]]] % NumGrad3;

            t0 *= t0;
            n0 = t0*t0*dot3(grad3_[g0], x0, y0, z0);
        }

        f32 t1 = 0.6f - x1*x1 - y1*y1 - z1*z1;
        if(t1<0.0f){
            n1 = 0.0f;
        }else{
            s32 g1 = permutation_[ixx+ix1 + permutation_[iyy+iy1 + permutation_[izz+iz1]]] % NumGrad3;

            t1 *= t1;
            n1 = t1*t1*dot3(grad3_[g1], x1, y1, z1);
        }

        f32 t2 = 0.6f - x2*x2 - y2*y2 - z2*z2;
        if(t2<0.0f){
            n2 = 0.0f;
        }else{
            s32 g2 = permutation_[ixx+ix2 + permutation_[iyy+iy2 + permutation_[izz+iz2]]] % NumGrad3;

            t2 *= t2;
            n2 = t2*t2*dot3(grad3_[g2], x2, y2, z2);
        }

        f32 t3 = 0.6f - x3*x3 - y3*y3 - z3*z3;
        if(t3<0.0f){
            n3 = 0.0f;
        }else{
            s32 g3 = permutation_[ixx+1   + permutation_[iyy+1   + permutation_[izz+1]]] % NumGrad3;

            t3 *= t3;
            n3 = t3*t3*dot3(grad3_[g3], x3, y3, z3);
        }

        return 32.0f * (n0 + n1 + n2 + n3);
    }

#if 1
    f32 Perlin::operator()(f32 x, f32 y, f32 z) const
    {
        static const f32 F3 = (1.0f/3.0f);
        static const f32 G3 = (1.0f/6.0f);

        f32 s = (x+y+z)*F3;

        s32 ix0 = fastFloor(x + s);
        s32 iy0 = fastFloor(y + s);
        s32 iz0 = fastFloor(z + s);
        
        f32 t = (f32)(ix0+iy0+iz0)*G3;
        f32 x0 = x-(ix0-t);
        f32 y0 = y-(iy0-t);
        f32 z0 = z-(iz0-t);

        s32 ix1, iy1, iz1;
        s32 ix2, iy2, iz2;

        if(y0<=x0){
            if(z0<=y0){
                ix1=1; iy1=0; iz1=0; ix2=1; iy2=1; iz2=0;
            }else if(z0<=x0){
                ix1=1; iy1=0; iz1=0; ix2=1; iy2=0; iz2=1;
            }else{
                ix1=0; iy1=0; iz1=1; ix2=1; iy2=0; iz2=1;
            }
        }else{
            if(y0<z0){
                ix1=0; iy1=0; iz1=1; ix2=0; iy2=1; iz2=1;
            }else if(x0<z0){
                ix1=0; iy1=1; iz1=0; ix2=0; iy2=1; iz2=1;
            }else{
                ix1=0; iy1=1; iz1=0; ix2=1; iy2=1; iz2=0;
            }
        }


        f32 x1 = x0 - ix1 + G3;
        f32 y1 = y0 - iy1 + G3;
        f32 z1 = z0 - iz1 + G3;
        f32 x2 = x0 - ix2 + 2.0f*G3;
        f32 y2 = y0 - iy2 + 2.0f*G3;
        f32 z2 = z0 - iz2 + 2.0f*G3;
        f32 x3 = x0 - 1.0f + 3.0f*G3;
        f32 y3 = y0 - 1.0f + 3.0f*G3;
        f32 z3 = z0 - 1.0f + 3.0f*G3;

        s32 ixx = ix0 & 0xFFU;
        s32 iyy = iy0 & 0xFFU;
        s32 izz = iz0 & 0xFFU;

        f32 n0, n1, n2, n3;

        f32 t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;
        if(t0<0.0f){
            n0 = 0.0f;
        }else{
            s32 g0 = permutation_[ixx     + permutation_[iyy     + permutation_[izz]]] % NumGrad3;

            t0 *= t0;
            n0 = t0*t0*dot3(grad3_[g0], x0, y0, z0);
        }

        f32 t1 = 0.6f - x1*x1 - y1*y1 - z1*z1;
        if(t1<0.0f){
            n1 = 0.0f;
        }else{
            s32 g1 = permutation_[ixx+ix1 + permutation_[iyy+iy1 + permutation_[izz+iz1]]] % NumGrad3;

            t1 *= t1;
            n1 = t1*t1*dot3(grad3_[g1], x1, y1, z1);
        }

        f32 t2 = 0.6f - x2*x2 - y2*y2 - z2*z2;
        if(t2<0.0f){
            n2 = 0.0f;
        }else{
            s32 g2 = permutation_[ixx+ix2 + permutation_[iyy+iy2 + permutation_[izz+iz2]]] % NumGrad3;

            t2 *= t2;
            n2 = t2*t2*dot3(grad3_[g2], x2, y2, z2);
        }

        f32 t3 = 0.6f - x3*x3 - y3*y3 - z3*z3;
        if(t3<0.0f){
            n3 = 0.0f;
        }else{
            s32 g3 = permutation_[ixx+1   + permutation_[iyy+1   + permutation_[izz+1]]] % NumGrad3;

            t3 *= t3;
            n3 = t3*t3*dot3(grad3_[g3], x3, y3, z3);
        }

        return 32.0f * (n0 + n1 + n2 + n3);
    }

#else
    f32 Perlin::operator()(f32 x, f32 y, f32 z) const
    {
        const f32 F3 = 1.0f/3.0f;
        //const f32 G3 = 1.0f/6.0f;
        static LIME_ALIGN16 const f32 G3[4] = {1.0f/6.0f,1.0f/6.0f,1.0f/6.0f,1.0f/6.0f};
        static LIME_ALIGN16 const f32 G3_2[4] = {2.0f/6.0f,2.0f/6.0f,2.0f/6.0f,2.0f/6.0f};
        static LIME_ALIGN16 const f32 G3_3[4] = {3.0f/6.0f,3.0f/6.0f,3.0f/6.0f,3.0f/6.0f};

        f32 s = (x+y+z)*F3;

        s32 ix0 = fastFloor(x + s);
        s32 iy0 = fastFloor(y + s);
        s32 iz0 = fastFloor(z + s);

        f32 t = (f32)(ix0+iy0+iz0)*G3[0];
        
        LIME_ALIGN16 f32 tv0[4];
        tv0[0] = x-(ix0-t);
        tv0[1] = y-(iy0-t);
        tv0[2] = z-(iz0-t);
        const f32& x0 = tv0[0];
        const f32& y0 = tv0[1];
        const f32& z0 = tv0[2];

        s32 ix1, iy1, iz1;
        s32 ix2, iy2, iz2;

        __m128 v1;
        __m128 v2 = _mm_set_ss(1.0f);

        if(y0<=x0){
            if(z0<=y0){
                ix1=1; iy1=0; iz1=0; v1 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,1,1,0));
                ix2=1; iy2=1; iz2=0; v2 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,1,0,0));
            }else if(z0<=x0){
                ix1=1; iy1=0; iz1=0; v1 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,1,1,0));
                ix2=1; iy2=0; iz2=1; v2 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,0,1,0));
            }else{
                ix1=0; iy1=0; iz1=1; v1 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,0,1,1));
                ix2=1; iy2=0; iz2=1; v2 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,0,1,0));
            }
        }else{
            if(y0<z0){
                ix1=0; iy1=0; iz1=1; v1 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,0,1,1));
                ix2=0; iy2=1; iz2=1; v2 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,0,0,1));
            }else if(x0<z0){
                ix1=0; iy1=1; iz1=0; v1 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,1,0,1));
                ix2=0; iy2=1; iz2=1; v2 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,0,0,1));
            }else{
                ix1=0; iy1=1; iz1=0; v1 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,1,0,1));
                ix2=1; iy2=1; iz2=0; v2 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(1,1,0,0));
            }
        }

        //__m128 VG3 = _mm_load_ps(G3);
        __m128 v0 = _mm_load_ps(tv0);
        v1 = _mm_add_ps(_mm_sub_ps(v0, v1), _mm_load_ps(G3));
        //__m128 VG3_2 = _mm_add_ps(VG3, VG3);
        v2 = _mm_add_ps(_mm_sub_ps(v0, v2), _mm_load_ps(G3_2));
        __m128 v3 = _mm_add_ps(_mm_sub_ps(v0, _mm_set_ps1(1.0f)), _mm_load_ps(G3_3));

        LIME_ALIGN16 f32 tv1[4]; _mm_store_ps(tv1, v1);
        LIME_ALIGN16 f32 tv2[4]; _mm_store_ps(tv2, v2);
        LIME_ALIGN16 f32 tv3[4]; _mm_store_ps(tv3, v3);

        LIME_ALIGN16 f32 tv0_2[4]; _mm_store_ps(tv0_2, _mm_mul_ps(v0,v0));
        LIME_ALIGN16 f32 tv1_2[4]; _mm_store_ps(tv1_2, _mm_mul_ps(v1,v1));
        LIME_ALIGN16 f32 tv2_2[4]; _mm_store_ps(tv2_2, _mm_mul_ps(v2,v2));
        LIME_ALIGN16 f32 tv3_2[4]; _mm_store_ps(tv3_2, _mm_mul_ps(v3,v3));

        s32 ixx = ix0 & 0xFFU;
        s32 iyy = iy0 & 0xFFU;
        s32 izz = iz0 & 0xFFU;

        f32 n0, n1, n2, n3;

        f32 t0 = 0.6f - tv0_2[0] - tv0_2[1] - tv0_2[2];
        if(t0<0.0f){
            n0 = 0.0f;
        }else{
            s32 g0 = permutation_[ixx     + permutation_[iyy     + permutation_[izz]]] % NumGrad3;

            t0 *= t0;
            n0 = t0*t0*dot3(grad3_[g0], x0, y0, z0);
        }

        f32 t1 = 0.6f - tv1_2[0] - tv1_2[1] - tv1_2[2];
        if(t1<0.0f){
            n1 = 0.0f;
        }else{
            s32 g1 = permutation_[ixx+ix1 + permutation_[iyy+iy1 + permutation_[izz+iz1]]] % NumGrad3;

            t1 *= t1;
            n1 = t1*t1*dot3(grad3_[g1], tv1[0], tv1[1], tv1[2]);
        }

        f32 t2 = 0.6f - tv2_2[0] - tv2_2[1] - tv2_2[2];
        if(t2<0.0f){
            n2 = 0.0f;
        }else{
            s32 g2 = permutation_[ixx+ix2 + permutation_[iyy+iy2 + permutation_[izz+iz2]]] % NumGrad3;

            t2 *= t2;
            n2 = t2*t2*dot3(grad3_[g2], tv2[0], tv2[1], tv2[2]);
        }

        f32 t3 = 0.6f - tv3_2[0] - tv3_2[1] - tv3_2[2];
        if(t3<0.0f){
            n3 = 0.0f;
        }else{
            s32 g3 = permutation_[ixx+1   + permutation_[iyy+1   + permutation_[izz+1]]] % NumGrad3;

            t3 *= t3;
            n3 = t3*t3*dot3(grad3_[g3], tv3[0], tv3[1], tv3[2]);
        }

        return 32.0f * (n0 + n1 + n2 + n3);
    }
#endif

    f32 Perlin::operator()(f32 x, f32 y, f32 z, f32 w) const
    {
        const f32 F4 = 0.309016994f; //F4 = (Math.sqrt(5.0)-1.0)/4.0
        const f32 G4 = 0.138196601f; //G4 = (5.0-Math.sqrt(5.0))/20.0

        f32 s = (x + y + z + w) * F4;

        s32 ix0 = fastFloor(x + s);
        s32 iy0 = fastFloor(y + s);
        s32 iz0 = fastFloor(z + s);
        s32 iw0 = fastFloor(w + s);

        f32 t = (f32)(ix0+iy0+iz0+iw0)*G4;
        f32 x0 = x-(ix0-t);
        f32 y0 = y-(iy0-t);
        f32 z0 = z-(iz0-t);
        f32 w0 = w-(iw0-t);

        s32 c1 = (y0<x0) ? 32 : 0;
        s32 c2 = (z0<x0) ? 16 : 0;
        s32 c3 = (z0<y0) ? 8 : 0;
        s32 c4 = (w0<x0) ? 4 : 0;
        s32 c5 = (w0<y0) ? 2 : 0;
        s32 c6 = (w0<z0) ? 1 : 0;
        s32 c = c1 + c2 + c3 + c4 + c5 + c6;

        s32 ix1, iy1, iz1, iw1;
        s32 ix2, iy2, iz2, iw2;
        s32 ix3, iy3, iz3, iw3;

        ix1 = (3<=simplex_[c][0]) ? 1 : 0;
        iy1 = (3<=simplex_[c][1]) ? 1 : 0;
        iz1 = (3<=simplex_[c][2]) ? 1 : 0;
        iw1 = (3<=simplex_[c][3]) ? 1 : 0;

        ix2 = (2<=simplex_[c][0]) ? 1 : 0;
        iy2 = (2<=simplex_[c][1]) ? 1 : 0;
        iz2 = (2<=simplex_[c][2]) ? 1 : 0;
        iw2 = (2<=simplex_[c][3]) ? 1 : 0;

        ix3 = (1<=simplex_[c][0]) ? 1 : 0;
        iy3 = (1<=simplex_[c][1]) ? 1 : 0;
        iz3 = (1<=simplex_[c][2]) ? 1 : 0;
        iw3 = (1<=simplex_[c][3]) ? 1 : 0;


        f32 x1 = x0 - ix1 + G4;
        f32 y1 = y0 - iy1 + G4;
        f32 z1 = z0 - iz1 + G4;
        f32 w1 = w0 - iw1 + G4;

        f32 x2 = x0 - ix2 + 2.0f*G4;
        f32 y2 = y0 - iy2 + 2.0f*G4;
        f32 z2 = z0 - iz2 + 2.0f*G4;
        f32 w2 = w0 - iw2 + 2.0f*G4;

        f32 x3 = x0 - ix3 + 3.0f*G4;
        f32 y3 = y0 - iy3 + 3.0f*G4;
        f32 z3 = z0 - iz3 + 3.0f*G4;
        f32 w3 = z0 - iw3 + 3.0f*G4;

        f32 x4 = x0 - 1.0f + 4.0f*G4;
        f32 y4 = y0 - 1.0f + 4.0f*G4;
        f32 z4 = z0 - 1.0f + 4.0f*G4;
        f32 w4 = z0 - 1.0f + 4.0f*G4;

        s32 ixx = ix0 & 0xFFU;
        s32 iyy = iy0 & 0xFFU;
        s32 izz = iz0 & 0xFFU;
        s32 iww = iw0 & 0xFFU;

        f32 n0, n1, n2, n3, n4;

        f32 t0 = 0.6f - x0*x0 - y0*y0 - z0*z0 - w0*w0;
        if(t0<0.0f){
            n0 = 0.0f;
        }else{
            s32 g0 = permutation_[ixx     + permutation_[iyy     + permutation_[izz + permutation_[iww]]]] % NumGrad4;

            t0 *= t0;
            n0 = t0*t0*dot4(grad3_[g0], x0, y0, z0, w0);
        }

        f32 t1 = 0.6f - x1*x1 - y1*y1 - z1*z1 - w1*w1;
        if(t1<0.0f){
            n1 = 0.0f;
        }else{
            s32 g1 = permutation_[ixx+ix1 + permutation_[iyy+iy1 + permutation_[izz+iz1 + permutation_[iww+iw1]]]] % NumGrad4;

            t1 *= t1;
            n1 = t1*t1*dot4(grad3_[g1], x1, y1, z1, w1);
        }

        f32 t2 = 0.6f - x2*x2 - y2*y2 - z2*z2 - w2*w2;
        if(t2<0.0f){
            n2 = 0.0f;
        }else{
            s32 g2 = permutation_[ixx+ix2 + permutation_[iyy+iy2 + permutation_[izz+iz2 + permutation_[iww+iw2]]]] % NumGrad4;

            t2 *= t2;
            n2 = t2*t2*dot4(grad3_[g2], x2, y2, z2, w2);
        }

        f32 t3 = 0.6f - x3*x3 - y3*y3 - z3*z3 - w3*w3;
        if(t3<0.0f){
            n3 = 0.0f;
        }else{
            s32 g3 = permutation_[ixx+ix3   + permutation_[iyy+iy3   + permutation_[izz+iz3 + permutation_[iww+iw3]]]] % NumGrad4;

            t3 *= t3;
            n3 = t3*t3*dot4(grad3_[g3], x3, y3, z3, w3);
        }

        f32 t4 = 0.6f - x4*x4 - y4*y4 - z4*z4 - w4*w4;
        if(t4<0.0f){
            n4 = 0.0f;
        }else{
            s32 g4 = permutation_[ixx+1   + permutation_[iyy+1   + permutation_[izz+1 + permutation_[iww+1]]]] % NumGrad4;

            t4 *= t4;
            n4 = t4*t4*dot4(grad3_[g4], x4, y4, z4, w4);
        }

        return 27.0f * (n0 + n1 + n2 + n3 + n4);
    }

    //------------------------------------------------
    //---
    //--- Fractal
    //---
    //------------------------------------------------
    f32 Fractal::fbm(const Perlin& noise, const Param& param, f32 x, f32 y)
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

    f32 Fractal::fbm(const Perlin& noise, const Param& param, f32 x, f32 y, f32 z)
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


    f32 Fractal::turbulence(const Perlin& noise, const Param& param, f32 x, f32 y)
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

    f32 Fractal::turbulence(const Perlin& noise, const Param& param, f32 x, f32 y, f32 z)
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

    f32 Fractal::multiFractal(const Perlin& noise, const Param& param, f32 x, f32 y)
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

    f32 Fractal::multiFractal(const Perlin& noise, const Param& param, f32 x, f32 y, f32 z)
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

    f32 Fractal::ridgedMultiFractal(const Perlin& noise, const RidgedParam& param, f32 x, f32 y)
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

    f32 Fractal::ridgedMultiFractal(const Perlin& noise, const RidgedParam& param, f32 x, f32 y, f32 z)
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
}
