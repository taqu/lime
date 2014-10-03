/**
@file lmathutil.cpp
@author t-sakai
@date 2010/09/09 create

*/
#include "lmathutil.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix34.h"
#include "Matrix44.h"
#include "Quaternion.h"

namespace lmath
{
namespace
{
    inline void store(Vector3& v, const lm128& r)
    {
        _mm_storel_pi(reinterpret_cast<__m64*>(&v.x_), r);

        static const u32 Shuffle = 170;
        lm128 t = _mm_shuffle_ps(r, r, Shuffle);
        _mm_store_ss(&v.z_, t);
    }

    inline void calcCubic(Vector4& dst, f32 t, const Matrix44& curveCoeff, const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector4& p3)
    {
        Vector4 vt;
        vt.w_ = 1.0f;
        vt.z_ = t;
        vt.y_ = t*t;
        vt.x_ = vt.y_ * t;

        Vector4 tmp;
        tmp.mul(vt, curveCoeff);

        lmath::Matrix44 param;
        param.set(p0, p1, p2, p3);
        
        dst.mul(tmp, param);
    }

    inline void calcCubic(f32& dst, f32 t, const Matrix44& curveCoeff, f32 p0, f32 p1, f32 p2, f32 p3)
    {
        Vector4 vt;
        vt.w_ = 1.0f;
        vt.z_ = t;
        vt.y_ = t*t;
        vt.x_ = vt.y_ * t;

        Vector4 tmp;
        tmp.mul(vt, curveCoeff);

        dst = p0 * tmp.x_ + p1 * tmp.y_ + p2 * tmp.z_ + p3 * tmp.w_;
    }
}

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

        store(bmin, retMin);
        store(bmax, retMax);

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

        store(median, mRet);
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


    // Ferguson-Coons曲線
    void fergusonCoonsCurve(Vector4& dst, f32 t, const Vector4& p0, const Vector4& v0, const Vector4& p1, const Vector4& v1)
    {
        lmath::Matrix44 curveCoeff(
            2.0f, -2.0f, 1.0f, 1.0f,
            -3.0f, 3.0f, -2.0f, -1.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 0.0f);

        calcCubic(dst, t, curveCoeff, p0, p1, v0, v1);
    }

    // Bezier曲線
    void cubicBezierCurve(Vector4& dst, f32 t, const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector4& p3)
    {
        lmath::Matrix44 curveCoeff(
            -1.0f,  3.0f, -3.0f, 1.0f,
             3.0f, -6.0f,  3.0f, 0.0f,
            -3.0f,  3.0f,  0.0f, 0.0f,
             1.0f,  0.0f,  0.0f, 0.0f);

        calcCubic(dst, t, curveCoeff, p0, p1, p2, p3);
    }


    // Catmull-Rom曲線
    void catmullRomCurve(Vector4& dst, f32 t, const Vector4& p0, const Vector4& p1, const Vector4& p2, const Vector4& p3)
    {
        lmath::Matrix44 curveCoeff(
            -1.0f/2.0f,  3.0f/2.0f, -3.0f/2.0f,  1.0f/2.0f,
             2.0f/2.0f, -5.0f/2.0f,  4.0f/2.0f, -1.0f/2.0f,
            -1.0f/2.0f,  0.0f,       1.0f/2.0f,  0.0f,
             0.0f,       2.0f/2.0f,  0.0f,       0.0f);

        calcCubic(dst, t, curveCoeff, p0, p1, p2, p3);
    }

    void catmullRomCurveStartPoint(Vector4& dst, f32 t, const Vector4& p1, const Vector4& p2, const Vector4& p3)
    {
        lmath::Matrix44 curveCoeff(
           0.0f,  0.0f,       0.0f,       0.0f,
           0.0f,  1.0f/2.0f, -2.0f/2.0f,  1.0f/2.0f,
           0.0f, -3.0f/2.0f,  4.0f/2.0f, -1.0f/2.0f,
           0.0f,  2.0f/2.0f,  0.0f,       0.0f);

        Vector4 vzero;
        vzero.zero();
        calcCubic(dst, t, curveCoeff, vzero, p1, p2, p3);
    }

    void catmullRomCurveEndPoint(Vector4& dst, f32 t, const Vector4& p0, const Vector4& p1, const Vector4& p2)
    {
        lmath::Matrix44 curveCoeff(
            0.0f,       0.0f,       0.0f,      0.0f,
            1.0f/2.0f, -2.0f/2.0f,  1.0f/2.0f, 0.0f,
           -1.0f/2.0f,  0.0f,       1.0f/2.0f, 0.0f,
            0.0f,       2.0f/2.0f,  0.0f,      0.0f);

        Vector4 vzero;
        vzero.zero();
        calcCubic(dst, t, curveCoeff, p0, p1, p2, vzero);
    }

    // Catmull-Rom曲線
    void catmullRomCurve(f32& dst, f32 t, f32 p0, f32 p1, f32 p2, f32 p3)
    {
        lmath::Matrix44 curveCoeff(
            -1.0f/2.0f,  3.0f/2.0f, -3.0f/2.0f,  1.0f/2.0f,
             2.0f/2.0f, -5.0f/2.0f,  4.0f/2.0f, -1.0f/2.0f,
            -1.0f/2.0f,  0.0f,       1.0f/2.0f,  0.0f,
             0.0f,       2.0f/2.0f,  0.0f,       0.0f);

        calcCubic(dst, t, curveCoeff, p0, p1, p2, p3);
    }


    //--------------------------------------------------------
    //--- 四元数
    //--------------------------------------------------------
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
        if(lcore::absolute(sinPitch) > F32_ANGLE_LIMIT1){
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
        if(lcore::absolute(sinPitch) > F32_ANGLE_LIMIT1){
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
        if(lcore::absolute(sinPitch) > F32_ANGLE_LIMIT1){
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
        if(lcore::absolute(sinPitch) > F32_ANGLE_LIMIT1){
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
