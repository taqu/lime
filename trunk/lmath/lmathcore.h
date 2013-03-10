#ifndef INC_LMATHCORE_H__
#define INC_LMATHCORE_H__
/**
@file lmathcore.h
@author t-sakai
@date 2009/01/17 create
*/
#include <lcore/lcore.h>
#include <lcore/clibrary.h>

#include <stdlib.h>
#include <math.h>

#if defined(ANDROID)

#include <limits.h>

#else

#endif //defined(ANDROID)


#if defined(LMATH_USE_SSE)
//#include <mmintrin.h>  //MMX命令セット
#include <xmmintrin.h> //SSE命令セット
//#include <emmintrin.h> //SSE2命令セット

namespace lmath
{
    typedef __m128 lm128; /// XMMレジスタに対応した単精度浮動小数点型
    typedef __m64 lm64;
}

#endif

namespace lmath
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::f32;
    using lcore::f64;

    class Vector3;

#define PI      static_cast<lmath::f32>(3.14159265358979323846)
#define PI2     static_cast<lmath::f32>(6.28318530717958647692)
#define INV_PI  static_cast<lmath::f32>(0.31830988618379067153)
#define INV_PI2 static_cast<lmath::f32>(0.15915494309189533576)
#define PI_2    static_cast<lmath::f32>(1.57079632679489661923)
#define INV_PI_2 static_cast<lmath::f32>(0.63661977236758134308)
#define LOG2    static_cast<lmath::f32>(0.693147180559945309417)
#define INV_LOG2    static_cast<lmath::f32>(1.0/0.693147180559945309417)

#if defined(ANDROID)
#define F32_EPSILON (1.192092896e-07F)
#define F64_EPSILON (2.2204460492503131e-016)
#else
#define F32_EPSILON (FLT_EPSILON)
#define F64_EPSILON (DBL_EPSILON)
#endif

#define DEG_TO_RAD (static_cast<lmath::f32>(1.57079632679489661923/90.0))
#define RAD_TO_DEG (static_cast<lmath::f32>(90.0/1.57079632679489661923))

    extern const f32 SphereRadiusEpsilon; //=1.0e-5f;

// 三角関数において、１とみなす下限
#define F32_ANGLE_LIMIT1 (0.9999f)
    //extern const f64 PI;
    //extern const f64 PI2;

    //extern const f32 F32_EPSILON;

    inline f32 tan(f32 radian)
    {
        return ::tanf(radian);
    }


    template<class T>
    inline T distance(T v0, T v1)
    {
        return (v0>v1)? (v0-v1) : (v1-v0);
    }

    inline bool isEqual(f32 x1, f32 x2)
    {
        return (lcore::absolute<f32>(x1 - x2) < F32_EPSILON);
    }

    inline bool isEqual(f32 x1, f32 x2, f32 epsilon)
    {
        return (lcore::absolute<f32>(x1 - x2) < epsilon);
    }

    inline bool isEqual(f64 x1, f64 x2)
    {
        return (lcore::absolute<f64>(x1 - x2) < F64_EPSILON);
    }

    inline bool isEqual(f64 x1, f64 x2, f64 epsilon)
    {
        return (lcore::absolute<f64>(x1 - x2) < epsilon);
    }


    inline bool isZero(f32 x1)
    {
        return (lcore::absolute<f32>(x1) < F32_EPSILON);
    }

    inline bool isZero(f32 x1, f32 epsilon)
    {
        return (lcore::absolute<f32>(x1) < epsilon);
    }

    inline bool isZero(f64 x1)
    {
        return (lcore::absolute<f64>(x1) < F64_EPSILON);
    }

    inline bool isZero(f64 x1, f64 epsilon)
    {
        return (lcore::absolute<f64>(x1) < epsilon);
    }


    inline bool isZeroPositive(f32 x1)
    {
        LASSERT(0.0f<=x1);
        return (x1 < F32_EPSILON);
    }

    inline bool isZeroPositive(f32 x1, f32 epsilon)
    {
        LASSERT(0.0f<=x1);
        return (x1 < epsilon);
    }

    inline bool isZeroPositive(f64 x1)
    {
        LASSERT(0.0f<=x1);
        return (x1 < F64_EPSILON);
    }

    inline bool isZeroPositive(f64 x1, f64 epsilon)
    {
        LASSERT(0.0f<=x1);
        return (x1 < epsilon);
    }

#if defined(LMATH_USE_SSE)
    inline lm128 set_m128(f32 x, f32 y, f32 z, f32 w)
    {
        lm128 t0 = _mm_load_ss(&x);
        lm128 t1 = _mm_load_ss(&z);

        lm128 ret = _mm_unpacklo_ps(t0, t1);
        t0 = _mm_load_ss(&y);
        t1 = _mm_load_ss(&w);
        return _mm_unpacklo_ps(ret, _mm_unpacklo_ps(t0, t1));
    }

    /**

    11bit近似値をNewton-Raphson法で22bitに高精度化
    */
    f32 rcp_22bit(f32 x);

    /**

    11bit近似値をNewton-Raphson法で22bitに高精度化
    */
    void rcp_22bit(f32 ix1, f32 ix2, f32& x1, f32& x2);

    /**

    11bit近似値をNewton-Raphson法で22bitに高精度化
    */
    void rcp_22bit(
        f32 ix1, f32 ix2, f32 ix3, f32 ix4,
        f32& x1, f32& x2, f32& x3, f32& x4);


    /**

    11bit近似値をNewton-Raphson法で22bitに高精度化
    */
    f32 rsqrt_22bit(f32 x);

    /**

    11bit近似値をNewton-Raphson法で22bitに高精度化
    */
    void rsqrt_22bit(f32 ix1, f32 ix2, f32& x1, f32& x2);
    /**

    11bit近似値をNewton-Raphson法で22bitに高精度化
    */
    void rsqrt_22bit(
        f32 ix1, f32 ix2, f32 ix3, f32 ix4,
        f32& x1, f32& x2, f32& x3, f32& x4);


    inline void sqrt(float ix1, float ix2, float& x1, float& x2)
    {
        LIME_ALIGN16 float ret[4];
        __m128 tmp = set_m128(ix1, ix2, ix1, ix2);
        _mm_store_ps(ret, _mm_sqrt_ps(tmp));
        x1 = ret[0];
        x2 = ret[1];
    }

    inline void sqrt(
        float ix1, float ix2, float ix3, float ix4,
        float& x1, float& x2, float& x3, float& x4)
    {
        LIME_ALIGN16 float ret[4];
        __m128 tmp = set_m128(ix1, ix2, ix3, ix4);
        _mm_store_ps(ret, _mm_sqrt_ps(tmp));
        x1 = ret[0];
        x2 = ret[1];
        x3 = ret[2];
        x4 = ret[3];
    }

#else

    /**
    普通のsqrtの逆数
    */
    inline f32 rsqrt_22bit(f32 x)
    {
        return 1.0f/sqrtf(x);
    }
#endif


    inline f32 sqrt(f32 x)
    {
#if defined(LMATH_USE_SSE)
        f32 ret;
        __m128 tmp = _mm_set_ss(x);
        _mm_store_ss(&ret, _mm_sqrt_ss(tmp));
        return ret;
#else
        return sqrtf(x);
#endif
    }

    f32 sinf_fast(f32 x);
    f32 cosf_fast(f32 x);
    void sincos(f32& dsn, f32& dcs, f32 x);

    inline f32 sinf(f32 x)
    {
        return ::sinf(x);
    }

    inline f32 cosf(f32 x)
    {
        return ::cosf(x);
    }

    inline f32 acos(f32 x)
    {
        return ::acosf(x);
    }

    inline f32 asin(f32 x)
    {
        return ::asinf(x);
    }

    inline f32 atan(f32 x)
    {
        return ::atanf(x);
    }

    inline f32 atan2(f32 x, f32 y)
    {
        return ::atan2f(x, y);
    }

    inline f32 exp(f32 x)
    {
        return ::expf(x);
    }

    inline f32 log(f32 x)
    {
        return ::logf(x);
    }
    
    inline f32 pow(f32 x, f32 y)
    {
        return ::powf(x, y);
    }

    inline s32 round2S32(f64 x)
    {
        const f64 doublemagic = 6755399441055744.0;
        x += doublemagic;
        return ((s32*)&x)[0];
    }

    inline s32 float2S32(f32 x)
    {
        const f64 doublemagicroundeps = .5-1.4e-11;
        return (x<0)? round2S32(x+doublemagicroundeps)
            : round2S32(x - doublemagicroundeps);
    }

    inline f32 floor(f32 val)
    {
        return floorf(val);
    }

    inline f32 ceil(f32 val)
    {
        return ceilf(val);
    }


    inline s32 floorS32(f32 val)
    {
        return static_cast<s32>( floorf(val) );
    }

    inline s32 ceilS32(f32 val)
    {
        return static_cast<s32>( ceilf(val) );
    }

    inline s32 floorS32(f64 val)
    {
        const f64 doublemagicroundeps = .5-1.4e-11;
        return round2S32(val - doublemagicroundeps);
    }

    inline s32 ceilS32(f64 val)
    {
        const f64 doublemagicroundeps = .5-1.4e-11;
        return round2S32(val + doublemagicroundeps);
    }


    inline f32 smoothstep(f32 x, f32 e0, f32 e1)
    {
        x = lcore::clamp01((x - e0) / (e1 - e0));
    	return x * x * (3.0f - 2.0f*x);
    }

    void randomOnSphere(f32& vx, f32& vy, f32& vz, f32 x0, f32 x1);

    template<class T>
    void randomOnSphere(f32& vx, f32& vy, f32& vz, T& random)
    {
        f32 x0, x1;
        f32 d;
        do{
            x0 = random.frand();
            x1 = random.frand();
            d = x0*x0 + x1*x1;
        }while(d>1.0f);

        f32 rx = lmath::sqrt(1.0f - d);
        x0 = 2.0f*x0 - 1.0f;
        x1 = 2.0f*x1 - 1.0f;

        vx = 2.0f*x0*rx;
        vy = 2.0f*x1*rx;
        vz = 1.0f - 2.0f*d;
    }

    void consineWeightedRandomOnHemiSphere(
        Vector3& v,
        const Vector3& n,
        f32 x0, f32 x1);



    //inline bool IsPowerOf2(int v) {
    //    return (v & (v - 1)) == 0;
    //}
    //inline u_int RoundUpPow2(u_int v) {
    //    v--;
    //    v |= v >> 1;
    //    v |= v >> 2;
    //    v |= v >> 4;
    //    v |= v >> 8;
    //    v |= v >> 16;
    //    return v+1;
    //}

//    inline bool Quadratic(float A, float B, float C, float *t0,
//		float *t1) {
//	// Find quadratic discriminant
//	float discrim = B * B - 4.f * A * C;
//	if (discrim < 0.) return false;
//	float rootDiscrim = sqrtf(discrim);
//	// Compute quadratic _t_ values
//	float q;
//	if (B < 0) q = -.5f * (B - rootDiscrim);
//	else       q = -.5f * (B + rootDiscrim);
//	*t0 = q / A;
//	*t1 = C / q;
//	if (*t0 > *t1) swap(*t0, *t1);
//	return true;
//}
//inline float ExponentialAverage(float avg,
//                              float val, float alpha) {
//	return (1.f - alpha) * val + alpha * avg;
//}
}

#endif //INC_LMATHCORE_H__