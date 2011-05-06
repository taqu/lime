#ifndef INC_LMATHCORE_H__
#define INC_LMATHCORE_H__
/**
@file lmathcore.h
@author t-sakai
@date 2009/01/17 create
*/
#include <lcore/lcore.h>

#include <stdlib.h>
#include <math.h>

#if defined(ANDROID)

#include <limits.h>

#else

#define LMATH_USE_SSE2
#include <limits>

#endif //defined(ANDROID)


#if defined(LMATH_USE_SSE2)
//#include <mmintrin.h>  //MMX命令セット
//#include <xmmintrin.h> //SSE命令セット
#include <emmintrin.h> //SSE2命令セット
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

#define PI      static_cast<lmath::f32>(3.14159265358979323846)
#define PI2     static_cast<lmath::f32>(6.28318530717958647692)
#define INV_PI  static_cast<lmath::f32>(0.31830988618379067153)
#define INV_PI2 static_cast<lmath::f32>(0.15915494309189533576)
#define PI_2    static_cast<lmath::f32>(1.57079632679489661923)
#define INV_PI_2 static_cast<lmath::f32>(0.63661977236758134308)

#if defined(ANDROID)
#define F32_EPSILON (1.192092896e-07F)
#define F64_EPSILON (2.2204460492503131e-016)
#else
#define F32_EPSILON (FLT_EPSILON)
#define F64_EPSILON (DBL_EPSILON)
#endif

// 三角関数において、１とみなす下限
#define F32_ANGLE_LIMIT1 (0.9999f)
    //extern const f64 PI;
    //extern const f64 PI2;

    //extern const f32 F32_EPSILON;

    inline f32 tan(f32 radian)
    {
        return ::tanf(radian);
    }

#if defined(ANDROID)
    template<class T>
    inline T absolute(T val)
    {
        return fabs(val);
    }

    template<>
    inline s8 absolute<s8>(s8 val)
    {
        return abs(val);
    }

    template<>
    inline s16 absolute<s16>(s16 val)
    {
        return abs(val);
    }

    template<>
    inline s32 absolute<s32>(s32 val)
    {
        return labs(val);
    }

    template<>
    inline u8 absolute<u8>(u8 val)
    {
        return val;
    }

    template<>
    inline u16 absolute<u16>(u16 val)
    {
        return val;
    }

    template<>
    inline u32 absolute<u32>(u32 val)
    {
        return val;
    }
#else
    template<class T>
    inline T absolute(T val)
    {
        return ::abs(val);
    }
#endif

    inline bool isEqual(f32 x1, f32 x2)
    {
        return (absolute<f32>(x1 - x2) < F32_EPSILON);
    }

    inline bool isEqual(f32 x1, f32 x2, f32 epsilon)
    {
        return (absolute<f32>(x1 - x2) < epsilon);
    }

    inline bool isEqual(f64 x1, f64 x2)
    {
        return (absolute<f64>(x1 - x2) < F64_EPSILON);
    }

    inline bool isEqual(f64 x1, f64 x2, f64 epsilon)
    {
        return (absolute<f64>(x1 - x2) < epsilon);
    }

#if defined(LMATH_USE_SSE2)
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
        __m128 tmp = _mm_set_ps(ix2, ix1, ix2, ix1);
        _mm_store_ps(ret, _mm_sqrt_ps(tmp));
        x1 = ret[0];
        x2 = ret[1];
    }

    inline void sqrt(
        float ix1, float ix2, float ix3, float ix4,
        float& x1, float& x2, float& x3, float& x4)
    {
        LIME_ALIGN16 float ret[4];
        __m128 tmp = _mm_set_ps(ix4, ix3, ix2, ix1);
        _mm_store_ps(ret, _mm_sqrt_ps(tmp));
        x1 = ret[0];
        x2 = ret[1];
        x3 = ret[2];
        x4 = ret[3];
    }

    void sqrt(f64 ix1, f64 ix2, f64& x1, f64& x2);
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
#if defined(LMATH_USE_SSE2)
        f32 ret;
        __m128 tmp = _mm_set_ss(x);
        _mm_store_ss(&ret, _mm_sqrt_ss(tmp));
        return ret;
#else
        return sqrtf(x);
#endif
    }


    
    inline f64 sqrt(f64 x)
    {
#if defined(LMATH_USE_SSE2)
        f64 ret;
        __m128d tmp = _mm_set_sd(x);
        _mm_store_sd(&ret, _mm_sqrt_sd(tmp, tmp));
        return ret;
#else
        return sqrt(x);
#endif
    }


    f32 sinf_fast(f32 x);
    f32 cosf_fast(f32 x);

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

    template<class T>
    inline void swap(T& a, T& b)
    {
        lcore::swap<T>(a, b);
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

    template<class T>
    inline T clamp(T val, T low, T high)
    {
        if (val <= low) return low;
        else if (val >= high) return high;
        else return val;
    }

    //inline float Lerp(float t, float v1, float v2) {
    //    return (1.f - t) * v1 + t * v2;
    //}
    //inline float Clamp(float val, float low, float high) {
    //    if (val < low) return low;
    //    else if (val > high) return high;
    //    else return val;
    //}
    //inline int Clamp(int val, int low, int high) {
    //    if (val < low) return low;
    //    else if (val > high) return high;
    //    else return val;
    //}
    //inline int Mod(int a, int b) {
    //    int n = int(a/b);
    //    a -= n*b;
    //    if (a < 0)
    //        a += b;
    //    return a;
    //}
    //inline float Radians(float deg) {
    //    return ((float)M_PI/180.f) * deg;
    //}
    //inline float Degrees(float rad) {
    //    return (180.f/(float)M_PI) * rad;
    //}
    //inline float Log2(float x) {
    //    static float invLog2 = 1.f / logf(2.f);
    //    return logf(x) * invLog2;
    //}
    //inline int Log2Int(float v) {
    //    return ((*(int *) &v) >> 23) - 127;
    //}
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
//inline float SmoothStep(float min, float max, float value) {
//	float v = Clamp((value - min) / (max - min), 0.f, 1.f);
//	return v * v * (-2.f * v  + 3.f);
//}
//inline float ExponentialAverage(float avg,
//                              float val, float alpha) {
//	return (1.f - alpha) * val + alpha * avg;
//}
}

#endif //INC_LMATHCORE_H__
