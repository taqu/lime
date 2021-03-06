﻿#ifndef INC_LMATH_H__
#define INC_LMATH_H__
/**
@file lmath.h
@author t-sakai
@date 2008/06/30 create
*/
#include <lcore/lcore.h>
#include <math.h>

//#include <mmintrin.h>  //MMX intrinsics
//#include <xmmintrin.h> //SSE intrinsics
//#include <emmintrin.h> //SSE2 intrinsics
#include <immintrin.h> //AVX intrinsics

namespace lmath
{
#ifndef LMATH_USE_SSE
#define LMATH_USE_SSE
#endif

    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::s64;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::u64;
    using lcore::f32;
    using lcore::f64;
    using lcore::Char;

    class Vector2;
    class Vector3;
    class Vector4;
    class Matrix34;
    class Matrix44;
    class Quaternion;

    typedef __m128 lm128; /// XMMレジスタに対応した単精度浮動小数点型
    typedef __m128i lm128i;
    typedef __m64 lm64;

    enum Result
    {
        Result_Fail = 0,
        Result_Success = (0x01U<<0),
        Result_Front = (0x01U<<0)|(0x01U<<1),
        Result_Back = (0x01U<<0)|(0x01U<<2),
    };

#define PI      static_cast<lmath::f32>(3.14159265358979323846)
#define PI2     static_cast<lmath::f32>(6.28318530717958647692)
#define INV_PI  static_cast<lmath::f32>(0.31830988618379067153)
#define INV_PI2 static_cast<lmath::f32>(0.15915494309189533576)
#define PI_2    static_cast<lmath::f32>(1.57079632679489661923)
#define INV_PI_2 static_cast<lmath::f32>(0.63661977236758134308)
#define LOG2    static_cast<lmath::f32>(0.693147180559945309417)
#define INV_LOG2    static_cast<lmath::f32>(1.0/0.693147180559945309417)

#define PI_64      static_cast<lmath::f64>(3.14159265358979323846)
#define PI2_64     static_cast<lmath::f64>(6.28318530717958647692)
#define INV_PI_64  static_cast<lmath::f64>(0.31830988618379067153)
#define INV_PI2_64 static_cast<lmath::f64>(0.15915494309189533576)
#define PI_2_64    static_cast<lmath::f64>(1.57079632679489661923)
#define INV_PI_2_64 static_cast<lmath::f64>(0.63661977236758134308)
#define LOG2_64    static_cast<lmath::f64>(0.693147180559945309417)
#define INV_LOG2_64    static_cast<lmath::f64>(1.0/0.693147180559945309417)

#if defined(ANDROID)
#define LMATH_F32_EPSILON (1.192092896e-07F)
#define LMATH_F64_EPSILON (2.2204460492503131e-016)
#else
#define LMATH_F32_EPSILON (FLT_EPSILON)
#define LMATH_F64_EPSILON (DBL_EPSILON)
#endif

#define DEG_TO_RAD (static_cast<lmath::f32>(1.57079632679489661923/90.0))
#define RAD_TO_DEG (static_cast<lmath::f32>(90.0/1.57079632679489661923))

    /// 三角関数において、１とみなす下限
#define LMATH_F32_ANGLE_LIMIT1 (0.9999f)

#define LMATH_F32_DOT_EPSILON (1.0e-6f)

#define LMATH_INFINITY lcore::numeric_limits<f32>::maximum()

    inline f32 tan(f32 radian)
    {
        return ::tanf(radian);
    }


    template<class T>
    inline T distance(T v0, T v1)
    {
        return (v0>v1)? (v0-v1) : (v1-v0);
    }

    inline bool isPositive(f32 v)
    {
        static const u32 mask = ~(((u32)-1)>>1);
        return (*((u32*)&v) & mask) == 0;
    }

    inline bool isEqual(f32 x1, f32 x2)
    {
        return (lcore::absolute<f32>(x1 - x2) <= LMATH_F32_EPSILON);
    }

    inline bool isEqual(f32 x1, f32 x2, f32 epsilon)
    {
        return (lcore::absolute<f32>(x1 - x2) <= epsilon);
    }

    inline bool isEqual(f64 x1, f64 x2)
    {
        return (lcore::absolute<f64>(x1 - x2) <= LMATH_F64_EPSILON);
    }

    inline bool isEqual(f64 x1, f64 x2, f64 epsilon)
    {
        return (lcore::absolute<f64>(x1 - x2) <= epsilon);
    }

    inline bool isZero(f32 x1)
    {
        return (lcore::absolute<f32>(x1) <= LMATH_F32_EPSILON);
    }

    inline bool isZero(f32 x1, f32 epsilon)
    {
        return (lcore::absolute<f32>(x1) <= epsilon);
    }

    inline bool isZero(f64 x1)
    {
        return (lcore::absolute<f64>(x1) <= LMATH_F64_EPSILON);
    }

    inline bool isZero(f64 x1, f64 epsilon)
    {
        return (lcore::absolute<f64>(x1) <= epsilon);
    }


    inline bool isZeroPositive(f32 x1)
    {
        LASSERT(0.0f<=x1);
        return (x1 <= LMATH_F32_EPSILON);
    }

    inline bool isZeroPositive(f32 x1, f32 epsilon)
    {
        LASSERT(0.0f<=x1);
        return (x1 <= epsilon);
    }

    inline bool isZeroPositive(f64 x1)
    {
        LASSERT(0.0f<=x1);
        return (x1 <= LMATH_F64_EPSILON);
    }

    inline bool isZeroPositive(f64 x1, f64 epsilon)
    {
        LASSERT(0.0f<=x1);
        return (x1 <= epsilon);
    }

    inline bool isZeroNegative(f32 x1)
    {
        LASSERT(x1<=0.0f);
        return (-LMATH_F32_EPSILON<=x1);
    }

    inline bool isZeroNegative(f32 x1, f32 epsilon)
    {
        LASSERT(x1<0.0f);
        return (epsilon<=x1);
    }

    inline bool isZeroNegative(f64 x1)
    {
        LASSERT(x1<0.0f);
        return (-LMATH_F64_EPSILON<=x1);
    }

    inline bool isZeroNegative(f64 x1, f64 epsilon)
    {
        LASSERT(x1<0.0f);
        return (epsilon<=x1);
    }

    inline lm128 set_m128(f32 x, f32 y, f32 z, f32 w)
    {
        lm128 t0 = _mm_load_ss(&x);
        lm128 t1 = _mm_load_ss(&z);

        lm128 ret = _mm_unpacklo_ps(t0, t1);
        t0 = _mm_load_ss(&y);
        t1 = _mm_load_ss(&w);
        return _mm_unpacklo_ps(ret, _mm_unpacklo_ps(t0, t1));
    }

    inline lm128 load3(const f32* v)
    {
        lm128 t = _mm_load_ss(&v[2]);
        t = _mm_movelh_ps(t, t);
        t = _mm_loadl_pi(t, reinterpret_cast<const __m64*>(v));
        return t;
    }

    inline void store3(f32* v, const lm128& r)
    {
        _mm_storel_pi(reinterpret_cast<__m64*>(v), r);

        static const u32 Shuffle = 170;
        lm128 t = _mm_shuffle_ps(r, r, Shuffle);
        _mm_store_ss(&v[2], t);
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

    //inline f32 rsqrt_22bit(f32 x)
    //{
    //    return 1.0f/sqrtf(x);
    //}

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


    void sqrt(f32 ix1, f32 ix2, f32& x1, f32& x2);

    void sqrt(
        f32 ix1, f32 ix2, f32 ix3, f32 ix4,
        f32& x1, f32& x2, f32& x3, f32& x4);



    inline f32 sqrt(f32 x)
    {
        f32 ret;
        __m128 tmp = _mm_set_ss(x);
        _mm_store_ss(&ret, _mm_sqrt_ss(tmp));
        return ret;
    }

    inline f64 sqrt(f64 x)
    {
        return ::sqrt(x);
    }

    f32 sinf_fast(f32 x);
    f32 cosf_fast(f32 x);
    void sincos(f32& dsn, f32& dcs, f32 x);

    inline f32 sinf(f32 x)
    {
        return ::sinf(x);
    }

    inline f64 sin(f64 x)
    {
        return ::sin(x);
    }

    inline f32 cosf(f32 x)
    {
        return ::cosf(x);
    }

    inline f64 cos(f64 x)
    {
        return ::cos(x);
    }

    inline f32 acos(f32 x)
    {
        return ::acosf(x);
    }

    inline f64 acos(f64 x)
    {
        return ::acos(x);
    }

    inline f32 asin(f32 x)
    {
        return ::asinf(x);
    }

    inline f64 asin(f64 x)
    {
        return ::asin(x);
    }

    inline f32 atan(f32 x)
    {
        return ::atanf(x);
    }

    inline f64 atan(f64 x)
    {
        return ::atan(x);
    }

    inline f32 atan2(f32 x, f32 y)
    {
        return ::atan2f(x, y);
    }

    inline f64 atan2(f64 x, f64 y)
    {
        return ::atan2(x, y);
    }

    inline f32 exp(f32 x)
    {
        return ::expf(x);
    }

    inline f64 exp(f64 x)
    {
        return ::exp(x);
    }

    inline f32 exp2(f32 x)
    {
        return ::exp2f(x);
    }

    inline f64 exp2(f64 x)
    {
        return ::exp2(x);
    }

    inline f32 log(f32 x)
    {
        return ::logf(x);
    }

    inline f64 log(f64 x)
    {
        return ::log(x);
    }

    inline f32 log2(f32 x)
    {
        return ::log2f(x);
    }

    inline f64 log2(f64 x)
    {
        return ::log2(x);
    }
    
    inline f32 pow(f32 x, f32 y)
    {
        return ::powf(x, y);
    }

    inline f64 pow(f64 x, f64 y)
    {
        return ::pow(x, y);
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

    inline f32 fmod(f32 x, f32 y)
    {
        return ::fmodf(x, y);
    }

    u8 quantizeRadian8(f32 rad);
    f32 dequantizeRadian8(u8 q);

    u16 quantizeRadian16(f32 rad);
    f32 dequantizeRadian16(u16 q);

    inline f32 step(f32 x, f32 a)
    {
        return (a<=x)? 1.0f : 0.0f;
    }

    inline f32 boxstep(f32 x, f32 e0, f32 e1)
    {
        LASSERT(!lmath::isEqual(e0, e1));
        return lcore::clamp01((x-e0)/(e1-e0));
    }

    //3t^2-2t^3
    inline f32 smooth(f32 x)
    {
        LASSERT(0<=x && x<=1.0f);
    	return x * x * (3.0f - 2.0f*x);
    }
    
    //6t^5-15t^4+10t^3
    f32 smooth2(f32 x);

    inline f32 smoothstep(f32 x, f32 e0, f32 e1)
    {
        x = lcore::clamp01((x - e0) / (e1 - e0));
    	return x * x * (3.0f - 2.0f*x);
    }

    f32 smoothstep2(f32 x, f32 e0, f32 e1);

    inline f32 expstep(f32 x, f32 a, f32 n)
    {
        return lmath::exp(-a*lmath::pow(x, n));
    }

    f32 cubicPulse(f32 center, f32 width, f32 x);

    f32 criticallyDampedSpring(f32 target, f32 current, f32& velocity, f32 timeStep, f32 maxVelocity);
    f32 criticallyDampedSpring(f32 target, f32 current, f32& velocity, f32 dampingRatio, f32 timeStep, f32 maxVelocity);
    f32 criticallyDampedSpring2(f32 target, f32 current, f32& velocity, f32 sqrtDampingRatio, f32 timeStep, f32 maxVelocity);

    void criticallyDampedSpring(const Vector4& target, Vector4& current, Vector4& velocity, f32 timeStep, f32 maxVelocity);
    void criticallyDampedSpring(const Vector4& target, Vector4& current, Vector4& velocity, f32 dampingRatio, f32 timeStep, f32 maxVelocity);
    void criticallyDampedSpring2(const Vector4& target, Vector4& current, Vector4& velocity, f32 sqrtDampingRatio, f32 timeStep, f32 maxVelocity);


    inline f32 gamma(f32 x, f32 g)
    {
        LASSERT(!lmath::isZero(g));
        return lmath::pow(x, 1.0f/g);
    }

    inline f32 bias(f32 a, f32 b)
    {
        static const f32 InvLogHalf = static_cast<f32>(1.0/-0.30102999566);
        return lmath::pow(a, lmath::log(b)*InvLogHalf);
    }

    f32 gain(f32 a, f32 b);

    /**
    @brief 単位球内ランダム
    @param vx ... 
    @param vy ... 
    @param vz ... 
    @param x0 ... 乱数0
    @param x1 ... 乱数1
    */
    void randomInSphere(f32& vx, f32& vy, f32& vz, f32 x0, f32 x1, f32 x2);
    void randomInSphere(Vector3& dst, f32 x0, f32 x1, f32 x2);
    void randomInSphere(Vector4& dst, f32 x0, f32 x1, f32 x2);


    /**
    @brief 単位球面上ランダム
    @param vx ... 
    @param vy ... 
    @param vz ... 
    @param x0 ... 乱数0
    @param x1 ... 乱数1
    */
    void randomOnSphere(f32& vx, f32& vy, f32& vz, f32 x0, f32 x1);
    void randomOnSphere(Vector3& dst, f32 x0, f32 x1);
    void randomOnSphere(Vector4& dst, f32 x0, f32 x1);

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

    /**
    @brief 単位半球面上ランダム
    @param vx ... 
    @param vy ... 
    @param vz ... 
    @param x0 ... 乱数0
    @param x1 ... 乱数1
    */
    void randomOnHemisphere(
        f32& vx, f32& vy, f32& vz,
        f32 x0, f32 x1);

    /**
    @brief 単位半球面上ランダム
    @param vx ... 
    @param vy ... 
    @param vz ... 
    @param n ... 
    @param x0 ... 乱数0
    @param x1 ... 乱数1
    */
    void randomOnHemisphere(
        f32& vx, f32& vy, f32& vz,
        const Vector3& n,
        f32 x0, f32 x1);


    /**
    @brief cosineに偏重した単位半球面上ランダム
    @param vx ... 
    @param vy ... 
    @param vz ... 
    @param x0 ... 乱数0
    @param x1 ... 乱数1
    */
    void cosineWeightedRandomOnHemisphere(
        f32& vx, f32& vy, f32& vz,
        f32 x0, f32 x1);

    /**
    @brief cosineに偏重した単位半球面上ランダム
    @param vx ... 
    @param vy ... 
    @param vz ... 
    @param n ... 
    @param x0 ... 乱数0
    @param x1 ... 乱数1
    */
    void cosineWeightedRandomOnHemisphere(
        f32& vx, f32& vy, f32& vz,
        const Vector3& n,
        f32 x0, f32 x1);

    void randomCone(
        f32& vx, f32& vy, f32& vz,
        f32 cutoffAngle,
        f32 x0,
        f32 x1);

    //
    void randomDiskConcentric(f32& vx, f32& vy, f32 x0, f32 x1);

    /**
    @brief 一様ランダム
    @param u ... 重心座標
    @param v ... 重心座標
    @param x0 ... 乱数0
    @param x1 ... 乱数1
    */
    void randomTriangle(f32& u, f32& v, f32 x0, f32 x1);

    void reflect(Vector3& dst, const Vector3& src, const Vector3& normal);
    void reflect(Vector4& dst, const Vector4& src, const Vector4& normal);

    void getColor(u8& r, u8& g, u8& b, u8& a, const Vector4& rgba);

    u32 getARGB(const Vector4& rgba);
    u32 getABGR(const Vector4& rgba);
    u32 getRGBA(const Vector4& rgba);

    /**
    @brief 立方体表面から球面へ変換
    @param x ... [-1 1]
    @param y ... [-1 1]
    @param z ... [-1 1]
    */
    void cubeToSphere(f32& x, f32& y, f32& z);

    /**
    @brief 立方体表面から球面へ変換
    @param inout ... [-1 1]
    */
    void cubeToSphere(lmath::Vector3& inout);

    /**
    @brief 立方体表面から球面へ変換
    @param inout ... [-1 1]
    */
    void cubeToSphere(lmath::Vector4& inout);

    void sphericalToCartesian(f32& x, f32& y, f32& z, f32 theta, f32 phi);
    void sphericalToCartesian(Vector3& v, f32 theta, f32 phi);
    void sphericalToCartesian(Vector4& v, f32 theta, f32 phi);

    void sphericalToCartesian(
        Vector3& v, f32 theta, f32 phi,
        const Vector3& x,
        const Vector3& y,
        const Vector3& z);

    void sphericalToCartesian(
        Vector4& v, f32 theta, f32 phi,
        const Vector4& x,
        const Vector4& y,
        const Vector4& z);

    void cartesianToSpherical(f32& theta, f32& phi, f32 x, f32 y, f32 z);
    void cartesianToSpherical(f32& theta, f32& phi, const Vector3& v);
    void cartesianToSpherical(f32& theta, f32& phi, const Vector4& v);

    /**
    @brief v0の向きをv1に合わせる
    */
    void faceForward(Vector3& dst, const Vector3& v0, const Vector3& v1);

    /**
    @brief v0の向きをv1に合わせる
    */
    void faceForward(Vector4& dst, const Vector4& v0, const Vector4& v1);

    void orthonormalBasis(Vector3& binormal0, Vector3& binormal1, const Vector3& normal);
    void orthonormalBasis(Vector4& binormal0, Vector4& binormal1, const Vector4& normal);

    bool solveLinearSystem2x2(const f32 A[2][2], const f32 B[2], f32& x0, f32& x1);

    /**
    @brief Z軸をnormal方向へ回転する行列
    */
    void rotationMatrixFromOrthonormalBasis(Matrix44& mat, const Vector4& normal, const Vector4& binormal0, const Vector4& binormal1);

    void smoothTranslate(Vector4& dst, const Vector4& current, const Vector4& target, f32 maxDistance);
    void smoothRotate(Vector4& dst, const Vector4& current, const Vector4& target, f32 maxRotate);

    f32 calcFOVY(f32 height, f32 znear);

    
    // Statistics, Probability
    //----------------------------------------------------------------
    /**
    @brief ガウス分布
    @param d ... 平均からの差
    */
    f32 gaussian(f32 d, f32 sigma);

    /**
    @brief 分散
    */
    f32 calcVariance(s32 num, const f32* v);

    /**
    @brief 一様半球面サンプルの確率密度関数
    */
    inline f32 uniformHemispherePdf()
    {
        return 1.0f/(2.0f*PI);
    }
    
    /**
    @brief 一様球面サンプルの確率密度関数
    */
    inline f32 uniformSpherePdf()
    {
        return 1.0f/(4.0f*PI);
    }

    /**
    */
    inline f32 uniformConePdf(f32 cosCutoffAngle)
    {
        LASSERT(!isZero(cosCutoffAngle));
        return INV_PI2/(1.0f - cosCutoffAngle);
    }

    /**
    @brief 一様円面サンプルの確率密度関数
    */
    inline f32 uniformDiskConcentricPdf()
    {
        return INV_PI;
    }

    /**
    @brief cosine偏重半球面サンプルの確率密度関数
    */
    inline f32 cosineWeightedHemispherePdf(f32 cosTheta)
    {
        return cosTheta * INV_PI;
    }

    //
    //-------------------------------------------------------
    bool solveLinearSystem2x2(const f32 A[2][2], const f32 B[2], f32& x0, f32& x1);


    //--------------------------------------------------------
    //---
    //--- Bounding Box
    //---
    //--------------------------------------------------------
    void calcBBox(Vector3& bmin, Vector3& bmax, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    void calcMedian(Vector3& median, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    void calcAABBPoints(Vector4* AABB, const Vector4& aabbMin, const Vector4& aabbMax);

    //--------------------------------------------------------
    //---
    //--- Curve
    //---
    //--------------------------------------------------------
    /**
    @brief Ferguson-Coons曲線
    @param dst ... 出力。
    @param t ... 0-1
    @param p0 ... 点0
    @param v0 ... 点0での速度
    @param p1 ... 点1
    @param v1 ... 点1での速度
    */
    void fergusonCoonsCurve(Vector4& dst, f32 t, const Vector4& p0, const Vector4& v0, const Vector4& p1, const Vector4& v1);

    /**
    @brief Bezier曲線
    @param dst ... 出力。
    @param t ... 0-1
    @param p0 ... 点0
    @param p1 ... 点1
    @param p2 ... 点2
    @param p3 ... 点3
    */
    void cubicBezierCurve(Vector4& dst, f32 t, const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector4& p3);

    /**
    @brief Catmull-Rom曲線
    @param dst ... 出力。
    @param t ... 0-1
    @param p0 ... 点0
    @param p1 ... 点1
    @param p2 ... 点2
    @param p3 ... 点3
    */
    void catmullRomCurve(Vector4& dst, f32 t, const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector4& p3);
    void catmullRomCurveStartPoint(Vector4& dst, f32 t, const Vector4& p1, const Vector4& p2, const Vector4& p3);
    void catmullRomCurveEndPoint(Vector4& dst, f32 t, const Vector4& p0, const Vector4& p1, const Vector4& p2);

    /**
    @brief Catmull-Rom曲線
    @param dst ... 出力。
    @param t ... 0-1
    @param p0 ... 点0
    @param p1 ... 点1
    @param p2 ... 点2
    @param p3 ... 点3
    */
    void catmullRomCurve(f32& dst, f32 t, f32 p0, f32 p1, f32 p2, f32 p3);

    //--------------------------------------------------------
    //---
    //--- Quaternion
    //---
    //--------------------------------------------------------
    /**
    @brief 単位四元数と移動ベクトルを単位デュアルクウォータニオンに変換

    移動してから、回転
    */
    void TransQuat2UnitDualQuat(const Quaternion& quat, const Vector3& trans, Quaternion dq[2]);

    /**
    @brief 単位四元数と移動ベクトルを単位デュアルクウォータニオンに変換

    回転してから、移動
    */
    void QuatTrans2UnitDualQuat(const Quaternion& quat, const Vector3& trans, Quaternion dq[2]);


    /**
    @brief 単位デュアルクウォータニオンを単位四元数と移動ベクトルに変換
    */
    void UnitDualQuat2QuatTrans(const f32 dq[2][4], Quaternion& quat, Vector3& trans);


    /**
    @brief デュアルクウォータニオンを単位四元数と移動ベクトルに変換

    デュアルクウォータニオンは非ゼロの非デュアルパート（四元数）の場合
    */
    void DualQuaternion2QuatTrans(const f32 dq[2][4], Quaternion& quat, Vector3& trans);

    /**
    @brief 慣性座標からオブジェクト座標へのオイラー角を取得

    慣性座標は、原点がオブジェクト座標と等しく、軸がワールド座標と平行な座標系
    */
    void getEulerInertialToObject(f32& head, f32& pitch, f32& bank, const lmath::Matrix34& m);

    /**
    @brief 慣性座標からオブジェクト座標へのオイラー角を取得

    慣性座標は、原点がオブジェクト座標と等しく、軸がワールド座標と平行な座標系
    */
    void getEulerInertialToObject(f32& head, f32& pitch, f32& bank, const lmath::Quaternion& q);


    /**
    @brief オブジェクト座標から慣性座標へのオイラー角を取得

    慣性座標は、原点がオブジェクト座標と等しく、軸がワールド座標と平行な座標系
    */
    void getEulerObjectToInertial(f32& head, f32& pitch, f32& bank, const lmath::Matrix34& m);

    /**
    @brief オブジェクト座標から慣性座標へのオイラー角を取得

    慣性座標は、原点がオブジェクト座標と等しく、軸がワールド座標と平行な座標系
    */
    void getEulerObjectToInertial(f32& head, f32& pitch, f32& bank, const lmath::Quaternion& q);

    /**
    @brief オイラー角から四元数を取得
    */
    void getQuaternionObjectToInertial(Quaternion& q, f32 head, f32 pitch, f32 bank);

    /**
    @brief オイラー角から四元数を取得
    */
    void getQuaternionInertialToObject(Quaternion& q, f32 head, f32 pitch, f32 bank);
}

    //--------------------------------------------------------
    //---
    //--- SSE
    //---
    //--------------------------------------------------------
    inline lmath::lm128 operator+(const lmath::lm128& v0, const lmath::lm128& v1)
    {
        return _mm_add_ps(v0, v1);
    }

    inline lmath::lm128 operator-(const lmath::lm128& v0, const lmath::lm128& v1)
    {
        return _mm_sub_ps(v0, v1);
    }

    inline lmath::lm128 operator*(const lmath::lm128& v0, const lmath::lm128& v1)
    {
        return _mm_mul_ps(v0, v1);
    }

    inline lmath::lm128 operator*(lmath::f32 x, const lmath::lm128& v)
    {
        return _mm_mul_ps(_mm_set1_ps(x), v);
    }

    inline lmath::lm128 operator*(const lmath::lm128& v, lmath::f32 x)
    {
        return _mm_mul_ps(v, _mm_set1_ps(x));
    }

    inline lmath::lm128 operator/(const lmath::lm128& v0, const lmath::lm128& v1)
    {
        return _mm_div_ps(v0, v1);
    }

    inline lmath::lm128 operator/(const lmath::lm128& v, lmath::f32 x)
    {
        return _mm_div_ps(v, _mm_set1_ps(x));
    }

    inline lmath::lm128 sqrt(const lmath::lm128& v)
    {
        return _mm_sqrt_ps(v);
    }

    inline lmath::lm128 minimum(const lmath::lm128& v0, const lmath::lm128& v1)
    {
        return _mm_min_ps(v0, v1);
    }

    inline lmath::lm128 maximum(const lmath::lm128& v0, const lmath::lm128& v1)
    {
        return _mm_max_ps(v0, v1);
    }

    inline lmath::lm128 muladd(const lmath::lm128& v0, const lmath::lm128& v1, const lmath::lm128& v2)
    {
        return _mm_add_ps(_mm_mul_ps(v0, v1), v2);
    }

    lmath::lm128 normalize(const lmath::lm128& v);

    lmath::lm128 floor(const lmath::lm128& v);

    lmath::lm128 ceil(const lmath::lm128& v);

    //---------------------------------------------------------------------------------------------------
    lmath::lm128 construct(lmath::f32 x, lmath::f32 y, lmath::f32 z);
    lmath::lm128 construct(lmath::f32 x, lmath::f32 y, lmath::f32 z, lmath::f32 w);

    lmath::lm128 normalize(lmath::f32 x, lmath::f32 y, lmath::f32 z);
    lmath::lm128 normalizeLengthSqr(lmath::f32 x, lmath::f32 y, lmath::f32 z, lmath::f32 lengthSqr);
    lmath::lm128 normalizeChecked(lmath::f32 x, lmath::f32 y, lmath::f32 z);

    lmath::lm128 normalize(lmath::f32 x, lmath::f32 y, lmath::f32 z, lmath::f32 w);
    lmath::lm128 normalizeLengthSqr(lmath::f32 x, lmath::f32 y, lmath::f32 z, lmath::f32 w, lmath::f32 lengthSqr);
    lmath::lm128 normalizeChecked(lmath::f32 x, lmath::f32 y, lmath::f32 z, lmath::f32 w);

#endif //INC_LMATH_H__
