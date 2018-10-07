/**
@file lmath.cpp
@author t-sakai
@date 2009/01/18 create
*/
#include "lmath.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix34.h"
#include "Matrix44.h"

namespace lmath
{
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
        LALIGN16 f32 ret[4];
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
        LALIGN16 f32 ret[4];
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
        LALIGN16 f32 ret[4];
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
        LALIGN16 f32 ret[4];
        lm128 tmp = set_m128(ix1, ix2, ix3, ix4);
        _mm_store_ps(ret, rsqrt_22bit_packed_core(tmp));
        x1 = ret[0];
        x2 = ret[1];
        x3 = ret[2];
        x4 = ret[3];
    }

    void sqrt(f32 ix1, f32 ix2, f32& x1, f32& x2)
    {
        LALIGN16 f32 ret[4];
        __m128 tmp = set_m128(ix1, ix2, ix1, ix2);
        _mm_store_ps(ret, _mm_sqrt_ps(tmp));
        x1 = ret[0];
        x2 = ret[1];
    }

    void sqrt(
        f32 ix1, f32 ix2, f32 ix3, f32 ix4,
        f32& x1, f32& x2, f32& x3, f32& x4)
    {
        LALIGN16 f32 ret[4];
        __m128 tmp = set_m128(ix1, ix2, ix3, ix4);
        _mm_store_ps(ret, _mm_sqrt_ps(tmp));
        x1 = ret[0];
        x2 = ret[1];
        x3 = ret[2];
        x4 = ret[3];
    }

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
            dsn = static_cast<f32>( (sign)? sin_series_x2(x, x2) : -sin_series_x2(x, x2) );
            dcs = static_cast<f32>( (sign2)? cos_series_x2(x2) : -cos_series_x2(x2) );
        }else{
            dsn = static_cast<f32>( (sign)? cos_series_x2(x2) : -cos_series_x2(x2) );
            dcs = static_cast<f32>( (sign2)? sin_series_x2(x, x2) : -sin_series_x2(x, x2) );
        }
    }

    u8 quantizeRadian8(f32 rad)
    {
        s32 srad = static_cast<s32>(rad*256.0f/PI2);
        if(255<srad){
            return 255;
        }else if(srad<0){
            return static_cast<u8>(srad+256);
        }else{
            return static_cast<u8>(srad);
        }
    }

    f32 dequantizeRadian8(u8 q)
    {
        return q*(PI2/255.0f);
    }

    u16 quantizeRadian16(f32 rad)
    {
        s32 srad = static_cast<s32>(rad*65536.0f/PI2);
        if(65535<srad){
            return 65535;
        }else if(srad<0){
            return static_cast<u16>(srad+65536);
        }else{
            return static_cast<u16>(srad);
        }
    }

    f32 dequantizeRadian16(u16 q)
    {
        return q*(PI2/65535.0f);
    }

    f32 smooth2(f32 x)
    {
        return ((6.0f*x -15.0f)*x+10.0f)*x*x*x;
    }

    f32 smoothstep2(f32 x, f32 e0, f32 e1)
    {
        x = lcore::clamp01((x - e0) / (e1 - e0));

        return ((6.0f*x -15.0f)*x+10.0f)*x*x*x;
    }

    f32 cubicPulse(f32 center, f32 width, f32 x)
    {
        x = lcore::absolute(x - center);
        if(x>width){
            return 0.0f;
        }
        x /= width;
        return 1.0f - x*x*(3.0f-2.0f*x);
    }

    f32 criticallyDampedSpring(f32 target, f32 current, f32& velocity, f32 timeStep, f32 maxVelocity)
    {
        f32 difference = target - current;
        f32 springForce = difference;
        f32 dampingForce = velocity * -2.0f;
        f32 force = springForce + dampingForce;
        velocity += force * timeStep;
        velocity = lcore::clamp(velocity, -maxVelocity, maxVelocity);
        return current + velocity * timeStep;
    }

    f32 criticallyDampedSpring(f32 target, f32 current, f32& velocity, f32 dampingRatio, f32 timeStep, f32 maxVelocity)
    {
        f32 difference = target - current;
        f32 springForce = difference * dampingRatio;
        f32 dampingForce = velocity * -2.0f * sqrtf(dampingRatio);
        f32 force = springForce + dampingForce;
        velocity += force * timeStep;
        velocity = lcore::clamp(velocity, -maxVelocity, maxVelocity);
        return current + velocity * timeStep;
    }

    f32 criticallyDampedSpring2(f32 target, f32 current, f32& velocity, f32 sqrtDampingRatio, f32 timeStep, f32 maxVelocity)
    {
        f32 difference = target - current;
        f32 springForce = difference * sqrtDampingRatio * sqrtDampingRatio;
        f32 dampingForce = velocity * -2.0f * sqrtDampingRatio;
        f32 force = springForce + dampingForce;
        velocity += force * timeStep;
        velocity = lcore::clamp(velocity, -maxVelocity, maxVelocity);
        return current + velocity * timeStep;
    }

    void criticallyDampedSpring(const Vector4& target, Vector4& current, Vector4& velocity, f32 timeStep, f32 maxVelocity)
    {
        return_type_vec4 difference = target - current;
        const return_type_vec4& springForce = difference;
        return_type_vec4 dampingForce = -2.0f * velocity;

        return_type_vec4 force = springForce + dampingForce;
        velocity = Vector4::construct(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(timeStep), force), (lm128)velocity));
        f32 velocityMaginitude = velocity.length();
        if(maxVelocity<velocityMaginitude){
            velocity *= maxVelocity/velocityMaginitude;
        }
        current = muladd(timeStep, velocity, current);
    }

    void criticallyDampedSpring(const Vector4& target, Vector4& current, Vector4& velocity, f32 dampingRatio, f32 timeStep, f32 maxVelocity)
    {
        return_type_vec4 difference = target - current;
        return_type_vec4 springForce = _mm_mul_ps(_mm_set1_ps(dampingRatio), difference);
        return_type_vec4 dampingForce = mul(-2.0f*sqrtf(dampingRatio), velocity);

        return_type_vec4 force = springForce + dampingForce;
        velocity = Vector4::construct(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(timeStep), force), (lm128)velocity));
        f32 velocityMaginitude = velocity.length();
        if(maxVelocity<velocityMaginitude){
            velocity *= maxVelocity/velocityMaginitude;
        }
        current = muladd(timeStep, velocity, current);
    }

    void criticallyDampedSpring2(const Vector4& target, Vector4& current, Vector4& velocity, f32 sqrtDampingRatio, f32 timeStep, f32 maxVelocity)
    {
        criticallyDampedSpring(target, current, velocity, sqrtDampingRatio*sqrtDampingRatio, timeStep, maxVelocity);
    }

    f32 gain(f32 a, f32 b)
    {
        if(a<0.001f){
            return 0.0f;
        }
        if(0.999f<a){
            return 1.0f;
        }

        static const f32 InvLogHalf = static_cast<f32>(1.0/-0.30102999566);
        f32 p = lmath::log(1.0f-b)*InvLogHalf;

        if (a < 0.5f){
            return lmath::pow(2.0f * a, p) * 0.5f;
        }else{
            return 1.0f - lmath::pow(2.0f - 2.0f*a, p) * 0.5f;
        }
    }

    void randomInSphere(f32& vx, f32& vy, f32& vz, f32 x0, f32 x1, f32 x2)
    {
        LASSERT(0.0f<=x0 && x0<=1.0f);
        LASSERT(0.0f<=x1 && x1<=1.0f);

        f32 theta = 2.0f*x0 - 1.0f;
        f32 r = lmath::sqrt(1.0f - theta*theta);

        f32 phi = PI2 *x1;
        f32 sn, cs;
        lmath::sincos(sn, cs, phi);

        r *= x2;
        vx = r * cs;
        vy = r * sn;
        vz = x2 * theta;
    }

    void randomInSphere(Vector3& dst, f32 x0, f32 x1, f32 x2)
    {
        randomInSphere(dst.x_, dst.y_, dst.z_, x0, x1, x2);
    }

    void randomInSphere(Vector4& dst, f32 x0, f32 x1, f32 x2)
    {
        randomInSphere(dst.x_, dst.y_, dst.z_, x0, x1, x2);
    }

    void randomOnSphere(f32& vx, f32& vy, f32& vz, f32 x0, f32 x1)
    {
        LASSERT(0.0f<=x0 && x0<=1.0f);
        LASSERT(0.0f<=x1 && x1<=1.0f);

        f32 theta = 2.0f*x0 - 1.0f;
        f32 r = lmath::sqrt(1.0f - theta*theta);

        f32 phi = PI2 *x1;
        f32 sn, cs;
        lmath::sincos(sn, cs, phi);
        vx = r * cs;
        vy = r * sn;
        vz = theta;
    }

    void randomOnSphere(Vector3& dst, f32 x0, f32 x1)
    {
        randomOnSphere(dst.x_, dst.y_, dst.z_, x0, x1);
    }

    void randomOnSphere(Vector4& dst, f32 x0, f32 x1)
    {
        randomOnSphere(dst.x_, dst.y_, dst.z_, x0, x1);
    }

    // 単位半球面上ランダム
    void randomOnHemisphere(
        f32& vx, f32& vy, f32& vz,
        f32 x0, f32 x1)
    {
        LASSERT(0.0f<=x0 && x0<=1.0f);
        LASSERT(0.0f<=x1 && x1<=1.0f);

        f32 theta = x0;
        f32 r = lmath::sqrt(1.0f - theta*theta);

        f32 phi = PI2 *x1;
        f32 sn, cs;
        lmath::sincos(sn, cs, phi);
        vx = r * cs;
        vy = r * sn;
        vz = theta;
    }

    // 単位半球面上ランダム
    void randomOnHemisphere(
        f32& vx, f32& vy, f32& vz,
        const Vector3& n,
        f32 x0, f32 x1)
    {
        Vector3 projected;
        randomOnSphere(projected.x_, projected.y_, projected.z_, x0, x1);

        f32 t = dot(projected, n);
        if(t<0.0f){
            projected = -projected;
        }
        vx = projected.x_;
        vy = projected.y_;
        vz = projected.z_;
    }

    // cosineに偏重した単位半球面上ランダム
    void cosineWeightedRandomOnHemisphere(
        f32& vx, f32& vy, f32& vz,
        f32 x0, f32 x1)
    {
        randomDiskConcentric(vx, vy, x0, x1);
        vz = lmath::sqrt(lcore::maximum(0.0f, 1.0f-vx*vx-vy*vy));
    }

    // cosineに偏重した単位半球面上ランダム
    void cosineWeightedRandomOnHemisphere(
        f32& vx, f32& vy, f32& vz,
        const Vector3& n,
        f32 x0, f32 x1)
    {
        f32 xs, ys, zs;
        cosineWeightedRandomOnHemisphere(xs, ys, zs, x0, x1);

        Vector3 b0, b1;
        orthonormalBasis(b0, b1, n);

        vx = xs*b0.x_ + ys*n.x_ + zs*b1.x_;
        vy = xs*b0.y_ + ys*n.y_ + zs*b1.y_;
        vz = xs*b0.z_ + ys*n.z_ + zs*b1.z_;
    }

    void randomCone(
        f32& vx, f32& vy, f32& vz,
        f32 cutoffAngle,
        f32 x0,
        f32 x1)
    {
        f32 cosTheta = (1.0f-x0) + x0*cutoffAngle;
        f32 sinTheta = lmath::sqrt(1.0f - cosTheta*cosTheta);
        f32 cosPhi, sinPhi;
        sincos(sinPhi, cosPhi, 2.0f*PI*x1);

        vx = sinTheta * cosPhi;
        vy = sinTheta * sinPhi;
        vz = cosTheta;
    }

    void randomDiskConcentric(f32& vx, f32& vy, f32 x0, f32 x1)
    {
        //http://psgraphics.blogspot.ch/2011/01/improved-code-for-concentric-map.html
        f32 sx = 2.0f*x0 - 1.0f;
        f32 sy = 2.0f*x1 - 1.0f;

        f32 phi, r;
        if(0.0f==sx && 0.0f==sy){
            r = 0.0f;
            phi = 0.0f;

        }else if(sy*sy<sx*sx){
            r = sx;
            phi = (PI/4)*(sy/sx);

        } else {
            r = sy;
            phi = (PI/2) - (PI/4)*(sx/sy);
        }

        f32 cosPhi, sinPhi;
        sincos(sinPhi, cosPhi, phi);
        vx = r * cosPhi;
        vy = r * sinPhi;
        //LASSERT(sqrt(vx*vx+vy*vy)<=1.0f);
    }

    void randomTriangle(f32& u, f32& v, f32 x0, f32 x1)
    {
        f32 su = lmath::sqrt(x0);
        u = 1.0f - su;
        v = x1 * su;
    }


    void reflect(Vector3& dst, const Vector3& src, const Vector3& normal)
    {
        Vector3 tmp = src;

        dst = normal;
        dst *= 2.0f*dot(normal, src);
        dst -= tmp;
    }

    void reflect(Vector4& dst, const Vector4& src, const Vector4& normal)
    {
        Vector4 tmp = src;

        dst = normal;
        dst *= 2.0f*dot(normal, src);
        dst -= tmp;
    }

    namespace
    {
        static const LALIGN16 f32 V255[4] = {255.0f, 255.0f, 255.0f, 255.0f};
    }

    void getColor(u8& r, u8& g, u8& b, u8& a, const Vector4& rgba)
    {
//#if defined(LMATH_USE_SSE)
#if 1
        lm128 tmp = _mm_loadu_ps(static_cast<const f32*>(&rgba.x_));

        lm128 one = _mm_load_ps(Vector4::One);
        lm128 zero = _mm_setzero_ps();

        tmp = _mm_max_ps(zero, tmp);
        tmp = _mm_min_ps(one, tmp);

        lm128 v256 = _mm_load_ps(V255);

        tmp = _mm_mul_ps(tmp, v256);
        lm128i tmp2 = _mm_cvtps_epi32(tmp);

        LALIGN16 s32 ret[4];
        _mm_store_si128((lm128i*)ret, tmp2);
        r = static_cast<u8>(ret[0]);
        g = static_cast<u8>(ret[1]);
        b = static_cast<u8>(ret[2]);
        a = static_cast<u8>(ret[3]);
#else
        r = static_cast<u8>(lcore::clamp01(rgba.x_) * 255.0f);
        g = static_cast<u8>(lcore::clamp01(rgba.y_) * 255.0f);
        b = static_cast<u8>(lcore::clamp01(rgba.z_) * 255.0f);
        a = static_cast<u8>(lcore::clamp01(rgba.w_) * 255.0f);
#endif
    }

    u32 getARGB(const Vector4& rgba)
    {
        u8 r, g, b, a;
        getColor(r, g, b, a, rgba);
        return lcore::getARGB(a, r, g, b);
    }

    u32 getABGR(const Vector4& rgba)
    {
        u8 r, g, b, a;
        getColor(r, g, b, a, rgba);
        return lcore::getABGR(a, r, g, b);
    }

    u32 getRGBA(const Vector4& rgba)
    {
        u8 r, g, b, a;
        getColor(r, g, b, a, rgba);
        return lcore::getRGBA(a, r, g, b);
    }

    void cubeToSphere(f32& x, f32& y, f32& z)
    {
        LASSERT(-1.0f<=x && x<=1.0f);
        LASSERT(-1.0f<=y && y<=1.0f);
        LASSERT(-1.0f<=z && z<=1.0f);

        f32 x2 = x*x;
        f32 y2 = y*y;
        f32 z2 = z*z;

        f32 dx = x*lmath::sqrt(1.0f - 0.5f*y2 - 0.5f*z2 + y2*z2/3.0f);
        f32 dy = y*lmath::sqrt(1.0f - 0.5f*z2 - 0.5f*x2 + z2*x2/3.0f);
        f32 dz = z*lmath::sqrt(1.0f - 0.5f*x2 - 0.5f*y2 + x2*y2/3.0f);
        x = dx;
        y = dy;
        z = dz;
    }

    void cubeToSphere(Vector3& inout)
    {
        cubeToSphere(inout.x_, inout.y_, inout.z_);
    }

    void cubeToSphere(Vector4& inout)
    {
        cubeToSphere(inout.x_, inout.y_, inout.z_);
    }

    void sphericalToCartesian(f32& x, f32& y, f32& z, f32 theta, f32 phi)
    {
        f32 csTheta, snTheta;
        f32 csPhi, snPhi;
        sincos(snTheta, csTheta, theta);
        sincos(snPhi, csPhi, phi);

        x = snTheta * csPhi;
        y = snTheta * snPhi;
        z = csTheta;
    }

    void sphericalToCartesian(Vector3& normal, f32 theta, f32 phi)
    {
        sphericalToCartesian(normal.x_, normal.y_, normal.z_, theta, phi);
    }

    void sphericalToCartesian(Vector4& normal, f32 theta, f32 phi)
    {
        sphericalToCartesian(normal.x_, normal.y_, normal.z_, theta, phi);
    }

    void sphericalToCartesian(
        Vector3& v, f32 theta, f32 phi,
        const Vector3& x,
        const Vector3& y,
        const Vector3& z)
    {
        f32 csTheta, snTheta;
        f32 csPhi, snPhi;
        sincos(snTheta, csTheta, theta);
        sincos(snPhi, csPhi, phi);

        v = mul(snTheta * csPhi, x);
        v = muladd(snTheta * snPhi, y, v);
        v = muladd(csTheta, z, v);
    }

    void sphericalToCartesian(
        Vector4& v, f32 theta, f32 phi,
        const Vector4& x,
        const Vector4& y,
        const Vector4& z)
    {
        f32 csTheta, snTheta;
        f32 csPhi, snPhi;
        sincos(snTheta, csTheta, theta);
        sincos(snPhi, csPhi, phi);

        lm128 tv = mul(snTheta * csPhi, x);
        tv = (snTheta * snPhi) * y + tv;
        tv = (csTheta) * z + tv;
        v = Vector4::construct(tv);
    }

    void cartesianToSpherical(f32& theta, f32& phi, f32 x, f32 y, f32 z)
    {
        theta = acosf(lcore::clamp(z, -1.0f, 1.0f));
        f32 p = atan2f(y, x);
        phi = (p<0.0f)? p + PI2 : p;
    }

    void cartesianToSpherical(f32& theta, f32& phi, const Vector3& v)
    {
        cartesianToSpherical(theta, phi, v.x_, v.y_, v.z_);
    }

    void normalToSphericalCoordinate(f32& theta, f32& phi, const Vector4& v)
    {
        cartesianToSpherical(theta, phi, v.x_, v.y_, v.z_);
    }

    // v0の向きをv1に合わせる
    void faceForward(Vector3& dst, const Vector3& v0, const Vector3& v1)
    {
        dst = (dot(v0, v1)<0.0f)? -v0 : v0;
    }

    // v0の向きをv1に合わせる
    void faceForward(Vector4& dst, const Vector4& v0, const Vector4& v1)
    {
        dst = (dot(v0, v1)<0.0f)? -v0 : v0;
    }

    //from Jeppe Revall Frisvad, "Building an Orthonormal Basis from a 3D Unit Vector Without Normalization"
    void orthonormalBasis(Vector3& binormal0, Vector3& binormal1, const Vector3& normal)
    {
        if(normal.z_<-0.9999999f){
            binormal0.set(0.0f, -1.0f, 0.0f);
            binormal1.set(-1.0f,  0.0f, 0.0f);
            return;
        }

        const f32 a = 1.0f/(1.0f + normal.z_);
        const f32 b = -normal.x_*normal.y_*a;
        binormal0.set(1.0f - normal.x_*normal.x_*a, b, -normal.x_);
        binormal1.set(b, 1.0f - normal.y_*normal.y_*a, -normal.y_);
    }

    void orthonormalBasis(Vector4& binormal0, Vector4& binormal1, const Vector4& normal)
    {
        if(normal.z_<-0.9999999f){
            binormal0.set(0.0f, -1.0f, 0.0f, 0.0f);
            binormal1.set(-1.0f,  0.0f, 0.0f, 0.0f);
            return;
        }

        const f32 a = 1.0f/(1.0f + normal.z_);
        const f32 b = -normal.x_*normal.y_*a;
        binormal0.set(1.0f - normal.x_*normal.x_*a, b, -normal.x_, 0.0f);
        binormal1.set(b, 1.0f - normal.y_*normal.y_*a, -normal.y_, 0.0f);
    }

    void rotationMatrixFromOrthonormalBasis(Matrix44& mat, const Vector4& normal, const Vector4& binormal0, const Vector4& binormal1)
    {
        mat.identity();
        mat.m_[0][0] = binormal0.x_; mat.m_[0][1] = binormal1.x_; mat.m_[0][2] = normal.x_;
        mat.m_[1][0] = binormal0.y_; mat.m_[1][1] = binormal1.y_; mat.m_[1][2] = normal.y_;
        mat.m_[2][0] = binormal0.z_; mat.m_[2][1] = binormal1.z_; mat.m_[2][2] = normal.z_;
    }

    void smoothTranslate(Vector4& dst, const Vector4& current, const Vector4& target, f32 maxDistance)
    {
        LASSERT(LMATH_F32_EPSILON<maxDistance);

        f32 d = distance(current, target);
        f32 t = lcore::clamp01(d/maxDistance);
        t = lmath::smooth(t);
        dst = lerp(current, target, t);
    }

    void smoothRotate(Vector4& dst, const Vector4& current, const Vector4& target, f32 maxRotate)
    {
        LASSERT(LMATH_F32_EPSILON<maxRotate);
        f32 cosine = dot(current, target);
        f32 omega = lmath::acos(cosine);

        f32 t = lcore::clamp01(omega/maxRotate);
        t = lmath::smooth(t);

        if(LMATH_F32_ANGLE_LIMIT1<=lcore::absolute(cosine)){
            dst = lerp(current, target, t);
        }else{
            dst = slerp(current, target, t, cosine);
        }
    }

    f32 calcFOVY(f32 height, f32 znear)
    {
        return 2.0f*lmath::atan2(0.5f*height, znear);
    }

    // Statistics, Probability
    //----------------------------------------------------------------
    // ガウス分布
    f32 gaussian(f32 d, f32 sigma)
    {
        return lmath::exp(-d*d/(2.0f*sigma)) * lmath::rsqrt_22bit(2.0f*PI*sigma);
    }

    // 分散
    f32 calcVariance(s32 num, const f32* v)
    {
        f32 avg = 0.0f;
        f32 avg2 = 0.0f;
        for(s32 i=0; i<num; ++i){
            avg += v[i];
            avg2 += v[i] * v[i];
        }
        f32 invN = 1.0f/num;
        avg *= invN;
        avg2 *= invN;
        return avg2 - avg*avg;
    }

    //
    //-------------------------------------------------------
    bool solveLinearSystem2x2(const f32 A[2][2], const f32 B[2], f32& x0, f32& x1)
    {
        f32 determinant = A[0][0]*A[1][1] - A[0][1]*A[1][0];
        if(lcore::absolute(determinant)<1.0e-6f){
            return false;
        }

        f32 invDeterminant = 1.0f/determinant;
        x0 = (A[1][1]*B[0] - A[0][1]*B[1]) * invDeterminant;
        x1 = (A[0][0]*B[1] - A[1][0]*B[0]) * invDeterminant;

        return (lcore::isNan(x0) || lcore::isNan(x1))? false : true;
    }


    //--------------------------------------------------------
    //---
    //--- Bounding Box
    //---
    //--------------------------------------------------------
    void calcBBox(Vector3& bmin, Vector3& bmax, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
#if defined(LMATH_USE_SSE)
        lm128 x0 = Vector3::load(v0);
        lm128 x1 = Vector3::load(v1);
        lm128 x2 = Vector3::load(v2);

        lm128 retMin = _mm_min_ps(x0, x1);
        lm128 retMax = _mm_max_ps(x0, x1);

        retMin = _mm_min_ps(retMin, x2);
        retMax = _mm_max_ps(retMax, x2);

        store3(&bmin.x_, retMin);
        store3(&bmax.x_, retMax);

#else
        bmin = v0;
        bmax = v0;

        const Vector3 *pv = &v1;
        for(int i=0; i<2; ++i){
            for(int j=0; j<3; ++j){
                if(bmin[j] > (*pv)[j]){
                    bmin[j] = (*pv)[j];
                }

                if(bmax[j] < (*pv)[j]){
                    bmax[j] = (*pv)[j];
                }
            }
            pv = &v2;
        }
#endif
    }


    void calcMedian(Vector3& median, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
#if defined(LMATH_USE_SSE)
        lm128 x0 = Vector3::load(v0);
        lm128 x1 = Vector3::load(v1);
        lm128 x2 = Vector3::load(v2);


        lm128 inv = _mm_set1_ps((1.0f/3.0f));

        lm128 mRet = _mm_add_ps(x0, x1);
        mRet = _mm_add_ps(mRet, x2);
        mRet = _mm_mul_ps(mRet, inv);

        store3(&median.x_, mRet);
#else
        median = v0;
        median += v1;
        median += v2;
        median /= 3.0f;
#endif
    }

    void calcAABBPoints(Vector4* AABB, const Vector4& aabbMin, const Vector4& aabbMax)
    {
        AABB[0].set(aabbMax.x_, aabbMax.y_, aabbMin.z_, 1.0f);
        AABB[1].set(aabbMin.x_, aabbMax.y_, aabbMin.z_, 1.0f);
        AABB[2].set(aabbMax.x_, aabbMin.y_, aabbMin.z_, 1.0f);
        AABB[3].set(aabbMin.x_, aabbMin.y_, aabbMin.z_, 1.0f);

        AABB[4].set(aabbMax.x_, aabbMax.y_, aabbMax.z_, 1.0f);
        AABB[5].set(aabbMin.x_, aabbMax.y_, aabbMax.z_, 1.0f);
        AABB[6].set(aabbMax.x_, aabbMin.y_, aabbMax.z_, 1.0f);
        AABB[7].set(aabbMin.x_, aabbMin.y_, aabbMax.z_, 1.0f);
    }


    //--------------------------------------------------------
    //---
    //--- Curve
    //---
    //--------------------------------------------------------
namespace
{
    inline void calcCubic(Vector4& dst, f32 t, const Matrix44& curveCoeff, const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector4& p3)
    {
        Vector4 vt;
        vt.w_ = 1.0f;
        vt.z_ = t;
        vt.y_ = t*t;
        vt.x_ = vt.y_ * t;

        Vector4 tmp = mul(vt, curveCoeff);

        Matrix44 param;
        param.set(p0, p1, p2, p3);
        
        dst = mul(tmp, param);
    }

    inline void calcCubic(f32& dst, f32 t, const Matrix44& curveCoeff, f32 p0, f32 p1, f32 p2, f32 p3)
    {
        Vector4 vt;
        vt.w_ = 1.0f;
        vt.z_ = t;
        vt.y_ = t*t;
        vt.x_ = vt.y_ * t;

        Vector4 tmp = mul(vt, curveCoeff);

        dst = p0 * tmp.x_ + p1 * tmp.y_ + p2 * tmp.z_ + p3 * tmp.w_;
    }
}

    // Ferguson-Coons曲線
    void fergusonCoonsCurve(Vector4& dst, f32 t, const Vector4& p0, const Vector4& v0, const Vector4& p1, const Vector4& v1)
    {
        Matrix44 curveCoeff = Matrix44::construct(
            2.0f, -2.0f, 1.0f, 1.0f,
            -3.0f, 3.0f, -2.0f, -1.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 0.0f);

        calcCubic(dst, t, curveCoeff, p0, p1, v0, v1);
    }

    // Bezier曲線
    void cubicBezierCurve(Vector4& dst, f32 t, const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector4& p3)
    {
        Matrix44 curveCoeff = Matrix44::construct(
            -1.0f,  3.0f, -3.0f, 1.0f,
             3.0f, -6.0f,  3.0f, 0.0f,
            -3.0f,  3.0f,  0.0f, 0.0f,
             1.0f,  0.0f,  0.0f, 0.0f);

        calcCubic(dst, t, curveCoeff, p0, p1, p2, p3);
    }


    // Catmull-Rom曲線
    void catmullRomCurve(Vector4& dst, f32 t, const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector4& p3)
    {
        Matrix44 curveCoeff = Matrix44::construct(
            -1.0f/2.0f,  3.0f/2.0f, -3.0f/2.0f,  1.0f/2.0f,
             2.0f/2.0f, -5.0f/2.0f,  4.0f/2.0f, -1.0f/2.0f,
            -1.0f/2.0f,  0.0f,       1.0f/2.0f,  0.0f,
             0.0f,       2.0f/2.0f,  0.0f,       0.0f);

        calcCubic(dst, t, curveCoeff, p0, p1, p2, p3);
    }

    void catmullRomCurveStartPoint(Vector4& dst, f32 t, const Vector4& p1, const Vector4& p2, const Vector4& p3)
    {
        Matrix44 curveCoeff = Matrix44::construct(
           0.0f,  0.0f,       0.0f,       0.0f,
           0.0f,  1.0f/2.0f, -2.0f/2.0f,  1.0f/2.0f,
           0.0f, -3.0f/2.0f,  4.0f/2.0f, -1.0f/2.0f,
           0.0f,  2.0f/2.0f,  0.0f,       0.0f);

        Vector4 vzero = Vector4::zero();
        calcCubic(dst, t, curveCoeff, vzero, p1, p2, p3);
    }

    void catmullRomCurveEndPoint(Vector4& dst, f32 t, const Vector4& p0, const Vector4& p1, const Vector4& p2)
    {
        Matrix44 curveCoeff = Matrix44::construct(
            0.0f,       0.0f,       0.0f,      0.0f,
            1.0f/2.0f, -2.0f/2.0f,  1.0f/2.0f, 0.0f,
           -1.0f/2.0f,  0.0f,       1.0f/2.0f, 0.0f,
            0.0f,       2.0f/2.0f,  0.0f,      0.0f);

        Vector4 vzero = Vector4::zero();
        calcCubic(dst, t, curveCoeff, p0, p1, p2, vzero);
    }

    // Catmull-Rom曲線
    void catmullRomCurve(f32& dst, f32 t, f32 p0, f32 p1, f32 p2, f32 p3)
    {
        Matrix44 curveCoeff = Matrix44::construct(
            -1.0f/2.0f,  3.0f/2.0f, -3.0f/2.0f,  1.0f/2.0f,
             2.0f/2.0f, -5.0f/2.0f,  4.0f/2.0f, -1.0f/2.0f,
            -1.0f/2.0f,  0.0f,       1.0f/2.0f,  0.0f,
             0.0f,       2.0f/2.0f,  0.0f,       0.0f);

        calcCubic(dst, t, curveCoeff, p0, p1, p2, p3);
    }


    //--------------------------------------------------------
    //---
    //--- Quaternion
    //---
    //--------------------------------------------------------
/*
=====================================================================================
Original Code:
dqconv.c

  Conversion routines between (regular quaternion, translation) and dual quaternion.

  Version 1.0.0, February 7th, 2007

  Copyright (C) 2006-2007 University of Dublin, Trinity College, All Rights 
  Reserved

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the author(s) be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Author: Ladislav Kavan, kavanl@cs.tcd.ie
=====================================================================================
*/
    // 単位四元数と移動ベクトルを単位デュアルクウォータニオンに変換
    void TransQuat2UnitDualQuat(const Quaternion& quat, const Vector3& trans, Quaternion dq[2])
    {
        // 四元数部分
        for(int i=0; i<4; ++i){
            dq[0][i] = quat[i];
        }

        // デュアルパート
        dq[1][0] = -0.5f * ( trans.x_ * quat.x_ + trans.y_ * quat.y_ + trans.z_ * quat.z_);

        dq[1][1] =  0.5f * ( trans.x_ * quat.w_ + trans.y_ * quat.z_ - trans.z_ * quat.y_);

        dq[1][2] =  0.5f * (-trans.x_ * quat.z_ + trans.y_ * quat.w_ + trans.z_ * quat.x_);

        dq[1][3] =  0.5f * ( trans.x_ * quat.y_ - trans.y_ * quat.x_ + trans.z_ * quat.w_);

    }

    // 単位四元数と移動ベクトルを単位デュアルクウォータニオンに変換
    void QuatTrans2UnitDualQuat(const Quaternion& quat, const Vector3& trans, Quaternion dq[2])
    {
        // 四元数部分
        for(int i=0; i<4; ++i){
            dq[0][i] = quat[i];
        }

        // デュアルパート
        dq[1][0] = 0.0f;

        dq[1][1] = 0.5f * trans.x_;

        dq[1][2] = 0.5f * trans.y_;

        dq[1][3] = 0.5f * trans.z_;

    }

    // 単位デュアルクウォータニオンを単位四元数と移動ベクトルに変換
    void UnitDualQuat2QuatTrans(const f32 dq[2][4], Quaternion& quat, Vector3& trans)
    {
        // 四元数
        quat.set(dq[0][0], dq[0][1], dq[0][2], dq[0][3]);

        // 移動ベクトル
        trans.x_ = 2.0f * (-dq[1][0]*dq[0][1] + dq[1][1]*dq[0][0] - dq[1][2]*dq[0][3] + dq[1][3]*dq[0][2]);
        trans.y_ = 2.0f * (-dq[1][0]*dq[0][2] + dq[1][1]*dq[0][3] + dq[1][2]*dq[0][0] - dq[1][3]*dq[0][1]);
        trans.z_ = 2.0f * (-dq[1][0]*dq[0][3] - dq[1][1]*dq[0][2] + dq[1][2]*dq[0][1] + dq[1][3]*dq[0][0]);
    }

    // デュアルクウォータニオンを単位四元数と移動ベクトルに変換
    void DualQuaternion2QuatTrans(const f32 dq[2][4], Quaternion& quat, Vector3& trans)
    {
        f32 length = 0.0f;
        for(int i=0; i<4; ++i){
            length += dq[0][i] * dq[0][i];
        }

        LASSERT(lmath::isEqual(length, 0.0f) == false);

        length = 1.0f / lmath::sqrt(length);

        quat.set(dq[0][0]*length, dq[0][1]*length, dq[0][2]*length, dq[0][3]*length);

        // 移動ベクトル
        length *= 2.0f;
        trans.x_ = length * (-dq[1][0]*dq[0][1] + dq[1][1]*dq[0][0] - dq[1][2]*dq[0][3] + dq[1][3]*dq[0][2]);
        trans.y_ = length * (-dq[1][0]*dq[0][2] + dq[1][1]*dq[0][3] + dq[1][2]*dq[0][0] - dq[1][3]*dq[0][1]);
        trans.z_ = length * (-dq[1][0]*dq[0][3] - dq[1][1]*dq[0][2] + dq[1][2]*dq[0][1] + dq[1][3]*dq[0][0]);
    }

    // 慣性座標からオブジェクト座標へのオイラー角を取得
    void getEulerInertialToObject(f32& head, f32& pitch, f32& bank, const lmath::Matrix34& m)
    {
        f32 sinPitch = -m.m_[2][1];
        if(sinPitch <= -1.0f){
            pitch = -PI_2;
        }else if(sinPitch >= 1.0f){
            pitch = PI_2;
        }else{
            pitch = asin(sinPitch);
        }

        // sin(pitch)が１ならバンクとヘディングがジンバルロック
        if(lcore::absolute(sinPitch) > LMATH_F32_ANGLE_LIMIT1){
            // バンクを0として、ヘディングを計算
            bank = 0.0f;
            head = atan2(-m.m_[0][2], m.m_[0][0]);
        }else{
            head = atan2(m.m_[2][0], m.m_[2][2]);

            bank = atan2(m.m_[0][1], m.m_[1][1]);
        }
    }

    // 慣性座標からオブジェクト座標へのオイラー角を取得
    void getEulerInertialToObject(f32& head, f32& pitch, f32& bank, const lmath::Quaternion& q)
    {
        f32 sinPitch = -2.0f * (q.y_*q.z_ + q.w_*q.x_);

        // sin(pitch)が１ならバンクとヘディングがジンバルロック
        if(lcore::absolute(sinPitch) > LMATH_F32_ANGLE_LIMIT1){
            pitch = PI_2 * sinPitch;

            // バンクを0として、ヘディングを計算
            bank = 0.0f;
            head = atan2(-q.x_*q.z_ - q.w_*q.y_, 0.5f-q.y_*q.y_-q.z_*q.z_);
        }else{
            pitch = asin(sinPitch);
            head = atan2(q.x_*q.z_ - q.w_*q.y_, 0.5f-q.x_*q.x_-q.y_*q.y_);
            bank = atan2(q.x_*q.y_ - q.w_*q.z_, 0.5f-q.x_*q.x_-q.z_*q.z_);
        }
    }


    // オブジェクト座標から慣性座標へのオイラー角を取得
    void getEulerObjectToInertial(f32& head, f32& pitch, f32& bank, const lmath::Matrix34& m)
    {
        f32 sinPitch = -m.m_[1][2];
        if(sinPitch <= -1.0f){
            pitch = -PI_2;
        }else if(sinPitch >= 1.0f){
            pitch = PI_2;
        }else{
            pitch = asin(sinPitch);
        }

        // sin(pitch)が１ならバンクとヘディングがジンバルロック
        if(lcore::absolute(sinPitch) > LMATH_F32_ANGLE_LIMIT1){
            // バンクを0として、ヘディングを計算
            bank = 0.0f;
            head = atan2(-m.m_[2][0], m.m_[0][0]);
        }else{
            head = atan2(m.m_[0][2], m.m_[2][2]);

            bank = atan2(m.m_[1][0], m.m_[1][1]);
        }
    }

    // オブジェクト座標から慣性座標へのオイラー角を取得
    void getEulerObjectToInertial(f32& head, f32& pitch, f32& bank, const lmath::Quaternion& q)
    {
        f32 sinPitch = -2.0f * (q.y_*q.z_ - q.w_*q.x_);

        // sin(pitch)が１ならバンクとヘディングがジンバルロック
        if(lcore::absolute(sinPitch) > LMATH_F32_ANGLE_LIMIT1){
            pitch = PI_2 * sinPitch;

            // バンクを0として、ヘディングを計算
            bank = 0.0f;
            head = atan2(-q.x_*q.z_ + q.w_*q.y_, 0.5f-q.y_*q.y_-q.z_*q.z_);
        }else{
            pitch = asin(sinPitch);
            head = atan2(q.x_*q.z_ + q.w_*q.y_, 0.5f-q.x_*q.x_-q.y_*q.y_);
            bank = atan2(q.x_*q.y_ + q.w_*q.z_, 0.5f-q.x_*q.x_-q.z_*q.z_);
        }
    }


    // オイラー角から四元数を取得
    void getQuaternionObjectToInertial(Quaternion& q, f32 head, f32 pitch, f32 bank)
    {
        //角度半分にする
        head *= 0.5f;
        pitch *= 0.5f;
        bank *= 0.5f;

        f32 csh = cosf(head);
        f32 snh = sinf(head);

        f32 csp = cosf(pitch);
        f32 snp = sinf(pitch);

        f32 csb = cosf(bank);
        f32 snb = sinf(bank);

        q.w_ = csh*csp*csb + snh*snp*snb;
        q.x_ = csh*snp*csb + snh*csp*snb;
        q.y_ = snh*csp*csb - csh*snp*snb;
        q.z_ = csh*csp*snb - snh*snp*csb;
    }

    // オイラー角から四元数を取得
    void getQuaternionInertialToObject(Quaternion& q, f32 head, f32 pitch, f32 bank)
    {
        //角度半分にする
        head *= 0.5f;
        pitch *= 0.5f;
        bank *= 0.5f;

        f32 csh = cosf(head);
        f32 snh = sinf(head);

        f32 csp = cosf(pitch);
        f32 snp = sinf(pitch);

        f32 csb = cosf(bank);
        f32 snb = sinf(bank);

        q.w_ =  csh*csp*csb + snh*snp*snb;
        q.x_ = -csh*snp*csb - snh*csp*snb;
        q.y_ =  csh*snp*snb - snh*csp*csb;
        q.z_ =  snh*snp*csb - csh*csp*snb;
    }

}

lmath::lm128 normalize(const lmath::lm128& r0)
{
    lmath::lm128 r1 = r0;
    lmath::lm128 tmp = _mm_mul_ps(r0, r0);
    tmp = _mm_add_ps( _mm_shuffle_ps(tmp, tmp, 0x4E), tmp);
    tmp = _mm_add_ps( _mm_shuffle_ps(tmp, tmp, 0xB1), tmp);

    tmp = _mm_sqrt_ss(tmp);
    tmp = _mm_shuffle_ps(tmp, tmp, 0);

    r1 = _mm_div_ps(r1, tmp);
    return r1;
}

lmath::lm128 floor(const lmath::lm128& tv0)
{
    lmath::lm128 tv1 = _mm_cvtepi32_ps(_mm_cvttps_epi32(tv0));

    return _mm_sub_ps(tv1, _mm_and_ps(_mm_cmplt_ps(tv0, tv1), _mm_set1_ps(1.0f)));
}

lmath::lm128 ceil(const lmath::lm128& tv0)
{
    lmath::lm128 tv1 = _mm_cvtepi32_ps(_mm_cvttps_epi32(tv0));
    return _mm_add_ps(tv1, _mm_and_ps(_mm_cmplt_ps(tv0, tv1), _mm_set1_ps(1.0f)));
}

//---------------------------------------------------------------------------------------------------
lmath::lm128 construct(lmath::f32 x, lmath::f32 y, lmath::f32 z)
{
    return _mm_set_ps(0.0f, z, y, x);
}

lmath::lm128 construct(lmath::f32 x, lmath::f32 y, lmath::f32 z, lmath::f32 w)
{
    return _mm_set_ps(w, z, y, x);
}

lmath::lm128 normalize(lmath::f32 x, lmath::f32 y, lmath::f32 z)
{
    lmath::lm128 r0 = _mm_set_ps(0.0f, z, y, x);
    lmath::lm128 r1 = r0;
    r0 = _mm_mul_ps(r0, r0);
    r0 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0x4E), r0);
    r0 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0xB1), r0);

    r0 = _mm_sqrt_ss(r0);
    r0 = _mm_shuffle_ps(r0, r0, 0);

    return _mm_div_ps(r1, r0);
}

lmath::lm128 normalizeLengthSqr(lmath::f32 x, lmath::f32 y, lmath::f32 z, lmath::f32 lengthSqr)
{
    lmath::lm128 r0 = _mm_set1_ps(lengthSqr);
    lmath::lm128 r1 = _mm_set_ps(0.0f, z, y, x);
    r0 = _mm_sqrt_ps(r0);
    return _mm_div_ps(r1, r0);
}

lmath::lm128 normalizeChecked(lmath::f32 x, lmath::f32 y, lmath::f32 z)
{
    lmath::f32 l = x*x + y*y + z*z;
    if(lmath::isZeroPositive(l)){
        return _mm_setzero_ps();
    }else{
        return normalizeLengthSqr(x, y, z, l);
    }
}

lmath::lm128 normalize(lmath::f32 x, lmath::f32 y, lmath::f32 z, lmath::f32 w)
{
    lmath::lm128 r0 = _mm_set_ps(w, z, y, x);
    lmath::lm128 r1 = r0;
    r0 = _mm_mul_ps(r0, r0);
    r0 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0x4E), r0);
    r0 = _mm_add_ps( _mm_shuffle_ps(r0, r0, 0xB1), r0);

    r0 = _mm_sqrt_ss(r0);
    r0 = _mm_shuffle_ps(r0, r0, 0);

    return _mm_div_ps(r1, r0);
}

lmath::lm128 normalizeLengthSqr(lmath::f32 x, lmath::f32 y, lmath::f32 z, lmath::f32 w, lmath::f32 lengthSqr)
{
    lmath::lm128 r0 = _mm_set1_ps(lengthSqr);
    lmath::lm128 r1 = _mm_set_ps(w, z, y, x);
    r0 = _mm_sqrt_ps(r0);
    return _mm_div_ps(r1, r0);
}

lmath::lm128 normalizeChecked(lmath::f32 x, lmath::f32 y, lmath::f32 z, lmath::f32 w)
{
    lmath::f32 l = x*x + y*y + z*z + w*w;
    if(lmath::isZeroPositive(l)){
        return _mm_setzero_ps();
    }else{
        return normalizeLengthSqr(x, y, z, w, l);
    }
}
