/**
@file lmathcore.cpp
@author t-sakai
@date 2009/01/18 create
*/
#include "lmathcore.h"
#include "Vector3.h"

namespace lmath
{
    const f32 SphereRadiusEpsilon =1.0e-5f;

#if defined(LMATH_USE_SSE)
    namespace
    {
        inline lm128 rcp_22bit_core(const lm128& x)
        {
            lm128 x0 = x;
            lm128 x1 = _mm_rcp_ss(x0);
            x0 = _mm_mul_ss(_mm_mul_ss(x0,x1), x1);
            x1 = _mm_add_ss(x1, x1);
            return _mm_sub_ss(x1, x0);
        }

        inline lm128 rcp_22bit_packed_core(const lm128& x)
        {
            lm128 x0 = x;
            lm128 x1 = _mm_rcp_ps(x0);
            x0 = _mm_mul_ps(_mm_mul_ps(x0,x1), x1);
            x1 = _mm_add_ps(x1, x1);
            return _mm_sub_ps(x1, x0);
        }
    }

    /**

    11bit近似値をNewton-Raphson法で22bitに高精度化
    */
    f32 rcp_22bit(f32 x)
    {
        f32 ret;
        lm128 tmp = _mm_load_ss(&x);
        _mm_store_ss(&ret, rcp_22bit_core(tmp));
        return ret;
    }

    /**

    11bit近似値をNewton-Raphson法で22bitに高精度化
    */
    void rcp_22bit(f32 ix1, f32 ix2, f32& x1, f32& x2)
    {
        LIME_ALIGN16 f32 ret[4];
        lm128 tmp = set_m128(ix1, ix2, ix1, ix2);
        _mm_store_ps(ret, rcp_22bit_packed_core(tmp));
        x1 = ret[0];
        x2 = ret[1];
    }

    /**

    11bit近似値をNewton-Raphson法で22bitに高精度化
    */
    void rcp_22bit(
        f32 ix1, f32 ix2, f32 ix3, f32 ix4,
        f32& x1, f32& x2, f32& x3, f32& x4)
    {
        LIME_ALIGN16 f32 ret[4];
        lm128 tmp = set_m128(ix1, ix2, ix3, ix4);
        _mm_store_ps(ret, rcp_22bit_packed_core(tmp));
        x1 = ret[0];
        x2 = ret[1];
        x3 = ret[2];
        x4 = ret[3];
    }



    namespace
    {
        inline lm128 rsqrt_22bit_core(const lm128& x)
        {
            const lm128 m_s0p5 = _mm_set1_ps(-0.5f);
            const lm128 m_s1p5 = _mm_set1_ps(1.5f);

            lm128 x0 = x;
            lm128 x1 = _mm_rsqrt_ss(x0);
            lm128 x2 = _mm_mul_ss(x0, x1);

            x2 = _mm_mul_ss(x2, x1);
            x2 = _mm_mul_ss(x2, x1);
            x2 = _mm_mul_ss(x2, m_s0p5);
            x0 = _mm_mul_ss(x1, m_s1p5);
            return _mm_add_ss(x0, x2);
        }

        inline __m128 rsqrt_22bit_packed_core(const __m128& x)
        {
            const lm128 m_s0p5 = _mm_set1_ps(-0.5f);
            const lm128 m_s1p5 = _mm_set1_ps(1.5f);

            lm128 x0 = x;
            lm128 x1 = _mm_rsqrt_ps(x0);
            lm128 x2 = _mm_mul_ps(x0, x1);

            x2 = _mm_mul_ps(x2, x1);
            x2 = _mm_mul_ps(x2, x1);
            x2 = _mm_mul_ps(x2, m_s0p5);
            x0 = _mm_mul_ps(x1, m_s1p5);
            return _mm_add_ps(x0, x2);
        }
    }

    /**

    11bit近似値をNewton-Raphson法で22bitに高精度化
    */
    f32 rsqrt_22bit(f32 x)
    {
        f32 ret;
        lm128 tmp = _mm_load_ss(&x);
        _mm_store_ss(&ret, rsqrt_22bit_core(tmp));
        return ret;
    }

    /**

    11bit近似値をNewton-Raphson法で22bitに高精度化
    */
    void rsqrt_22bit(f32 ix1, f32 ix2, f32& x1, f32& x2)
    {
        LIME_ALIGN16 f32 ret[4];
        lm128 tmp = set_m128(ix1, ix2, ix1, ix2);
        _mm_store_ps(ret, rsqrt_22bit_packed_core(tmp));
        x1 = ret[0];
        x2 = ret[1];
    }

    /**

    11bit近似値をNewton-Raphson法で22bitに高精度化
    */
    void rsqrt_22bit(
        f32 ix1, f32 ix2, f32 ix3, f32 ix4,
        f32& x1, f32& x2, f32& x3, f32& x4)
    {
        LIME_ALIGN16 f32 ret[4];
        lm128 tmp = set_m128(ix1, ix2, ix3, ix4);
        _mm_store_ps(ret, rsqrt_22bit_packed_core(tmp));
        x1 = ret[0];
        x2 = ret[1];
        x3 = ret[2];
        x4 = ret[3];
    }

#endif

    namespace
    {
        static const f64 cos_coef[] = 
        {
            -5.00000000000000000000e-1,
            4.16666666666666666667e-2,
            -1.38888888888888888889e-3,
            2.48015873015873015873e-5,
            -2.75573192239858906526e-7,
             2.08767569878680989792e-9,
        };

        static const f64 sin_coef[] = 
        {
            -1.66666666666666666667e-1,
            8.33333333333333333333e-3,
            -1.98412698412698412698e-4,
            2.75573192239858906526e-6,
            -2.50521083854417187751e-8,
             1.60590438368216145994e-10,
        };
    }

    /**
    @brief cos係数
    */
    f64 cos_series(f64 x)
    {
        f64 x2 = x * x;
        f64 ret = cos_coef[4];
        //f64 ret = cos_coef[3];
        ret = cos_coef[3] + ret * x2;
        ret = cos_coef[2] + ret * x2;
        ret = cos_coef[1] + ret * x2;
        ret = cos_coef[0] + ret * x2;
        ret = 1.0 + ret * x2;
        return ret;
    };


    /**
    @brief cos係数
    */
    f64 cos_series_x2(f64 x2)
    {
        f64 ret = cos_coef[4];
        //f64 ret = cos_coef[3];
        ret = cos_coef[3] + ret * x2;
        ret = cos_coef[2] + ret * x2;
        ret = cos_coef[1] + ret * x2;
        ret = cos_coef[0] + ret * x2;
        ret = 1.0 + ret * x2;
        return ret;
    };


    /**
    @brief sin係数
    */
    f64 sin_series(f64 x)
    {
        f64 x2 = x * x;
        f64 ret = sin_coef[4];
        //f64 ret = sin_coef[3];
        ret = sin_coef[3] + ret * x2;
        ret = sin_coef[2] + ret * x2;
        ret = sin_coef[1] + ret * x2;
        ret = sin_coef[0] + ret * x2;
        ret = x + ret * x2 * x;
        return ret;
    };

    /**
    @brief sin係数
    */
    f64 sin_series_x2(f64 x, f64 x2)
    {
        f64 ret = sin_coef[4];
        //f64 ret = sin_coef[3];
        ret = sin_coef[3] + ret * x2;
        ret = sin_coef[2] + ret * x2;
        ret = sin_coef[1] + ret * x2;
        ret = sin_coef[0] + ret * x2;
        ret = x + ret * x2 * x;
        return ret;
    };

    /**
    @brief sin演算
    */
    __inline f64 sin_core(f64 val)
    {
        const f64 pi2     = 6.28318530717958647692;
        const f64 inv_pi2 = 0.15915494309189533576;

        //sin = -sin
        f64 x = val;
        x = lcore::absolute(x);

        //pi/2の範囲に収める
        x *= inv_pi2;
        long v = (long)(x);
        x -= v;

        //2*piの範囲を８つに分割して、符号とsin,cosどちらの係数で計算するか判別
        long s = (long)(x*8.0);
        ++s;
        long s2 = s >> 1;

        f64 offset = -0.25 * s2;
        x = (x+offset)*pi2;

        bool sign = (s & 0x04U) == 0;
        sign = (val<0.0)? !sign : sign;
        bool isSin = (s2 & 0x01U) == 0;

        if(isSin){
            return (sign)? sin_series(x) : -sin_series(x);
        }else{
            return (sign)? cos_series(x) : -cos_series(x);
        }
    }
    

    //
    f32 sinf_fast(f32 f)
    {
        return static_cast<f32>( sin_core(f) );
    }

    //
    f32 cosf_fast(f32 f)
    {
        const f64 pi_2     = 1.57079632679489661923;

        return static_cast<f32>( sin_core(pi_2 + f) );
    }


    void sincos(f32& dsn, f32& dcs, f32 val)
    {
        const f64 pi2     = 6.28318530717958647692;
        const f64 inv_pi2 = 0.15915494309189533576;

        f64 x = val;
        x = lcore::absolute(x);

        //pi/2の範囲に収める
        x *= inv_pi2;
        long v = (long)(x);
        x -= v;

        //2*piの範囲を８つに分割して、符号とsin,cosどちらの係数で計算するか判別
        long s = (long)(x*8.0);
        ++s;
        long s2 = s >> 1;

        f64 offset = -0.25 * s2;
        x = (x+offset)*pi2;

        bool sign = (s & 0x04U) == 0;
        sign = (val<0.0)? !sign : sign;

        bool sign2 = (s<2 || 5<s);

        bool isSin = (s2 & 0x01U) == 0;

        f64 x2 = x*x;

        if(isSin){
            dsn = (sign)? sin_series_x2(x, x2) : -sin_series_x2(x, x2);
            dcs = (sign2)? cos_series_x2(x2) : -cos_series_x2(x2);
        }else{
            dsn = (sign)? cos_series_x2(x2) : -cos_series_x2(x2);
            dcs = (sign2)? sin_series_x2(x, x2) : -sin_series_x2(x, x2);
        }
    }


    void randomOnSphere(f32& vx, f32& vy, f32& vz, f32 x0, f32 x1)
    {
        LASSERT(0.0f<=x0 && x0<=1.0f);
        LASSERT(0.0f<=x1 && x1<=1.0f);

        x1 = PI2*x1 - PI;
        f32 cs = lmath::cosf(x1);
        f32 sn = lmath::sinf(x1);

        f32 rx0 = lmath::sqrt(1.0f - x0*x0);
        x0 = 2.0f*x0 - 1.0f;

        vx = rx0 * cs;
        vy = rx0 * sn;
        vz = x0;
    }

    void consineWeightedRandomOnHemiSphere(
        Vector3& v,
        const Vector3& n,
        f32 x0, f32 x1)
    {
        Vector3 projected;
        randomOnSphere(projected.x_, projected.y_, projected.z_, x0, x1);

        f32 t = projected.dot(n);
        Vector3 tn(n);
        tn *= t;
        projected -= tn;

        f32 sn = projected.length();
        f32 cs = lmath::sqrt(1.0f - sn * sn);
        v = n;
        v *= cs;
        v += projected;
    }
}

