/**
@file Matrix34.cpp
@author t-sakai
@date 2011/06/30
*/
#include "Matrix34.h"
#include "Matrix44.h"
#include "Quaternion.h"

namespace lmath
{

    //--------------------------------------------
    //---
    //--- Matrix34
    //---
    //--------------------------------------------
    Matrix34::Matrix34(const Matrix44& rhs)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0 = _mm_loadu_ps(&rhs.m_[0][0]);
        lm128 r1 = _mm_loadu_ps(&rhs.m_[1][0]);
        lm128 r2 = _mm_loadu_ps(&rhs.m_[2][0]);
        store(*this, r0, r1, r2);
#else
        lcore::memcpy(m_, rhs.m_, sizeof(Matrix34));
#endif
    }

    Matrix34& Matrix34::operator=(const Matrix34& rhs)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0, r1, r2;
        load(r0, r1, r2, rhs);
        store(*this, r0, r1, r2);
#else
        lcore::memcpy(m_, rhs.m_, sizeof(Matrix34));
#endif
        return *this;
    }

    Matrix34& Matrix34::operator*=(f32 f)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0, r1, r2;
        load(r0, r1, r2, *this);

        lm128 v = _mm_load1_ps(&f);

        r0 = _mm_mul_ps(r0, v);
        r1 = _mm_mul_ps(r1, v);
        r2 = _mm_mul_ps(r2, v);

        store(*this, r0, r1, r2);

#else
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] *= f;
            }
        }
#endif
        return *this;
    }


    Matrix34& Matrix34::operator+=(const Matrix34& rhs)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0, r1, r2;
        load(r0, r1, r2, *this);

        lm128 t0, t1, t2;
        load(t0, t1, t2, rhs);

        r0 = _mm_add_ps(r0, t0);
        r1 = _mm_add_ps(r1, t1);
        r2 = _mm_add_ps(r2, t2);

        store(*this, r0, r1, r2);

#else
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] += rhs.m_[i][j];
            }
        }
#endif
        return *this;
    }


    Matrix34& Matrix34::operator-=(const Matrix34& rhs)
    {
#if defined(LMATH_USE_SSE)
        lm128 r0, r1, r2;
        load(r0, r1, r2, *this);

        lm128 t0, t1, t2;
        load(t0, t1, t2, rhs);

        r0 = _mm_sub_ps(r0, t0);
        r1 = _mm_sub_ps(r1, t1);
        r2 = _mm_sub_ps(r2, t2);

        store(*this, r0, r1, r2);

#else
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] -= rhs.m_[i][j];
            }
        }
#endif
        return *this;
    }


    Matrix34 Matrix34::operator-() const
    {
        Matrix34 ret;

#if defined(LMATH_USE_SSE)
        f32 f;
        *((u32*)&f) = 0x80000000U;
        lm128 mask = _mm_load1_ps(&f);
        lm128 r0, r1, r2;
        load(r0, r1, r2, *this);

        r0 = _mm_xor_ps(r0, mask);
        r1 = _mm_xor_ps(r1, mask);
        r2 = _mm_xor_ps(r2, mask);

        store(ret, r0, r1, r2);
#else
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                ret.m_[i][j] = -m_[i][j];
            }
        }
#endif
        return ret;
    }


    void Matrix34::identity()
    {
#if defined(LMATH_USE_SSE)
        f32 one = 1.0f;
        static const u32 rotmask = 147; //上位桁方向へ回転
        lm128 t = _mm_load_ss(&one);
        _mm_storeu_ps(&(m_[0][0]), t);

        t = _mm_shuffle_ps(t, t, rotmask);
        _mm_storeu_ps(&(m_[1][0]), t);

        t = _mm_shuffle_ps(t, t, rotmask);
        _mm_storeu_ps(&(m_[2][0]), t);
#else
        m_[0][0] = 1.0f; m_[0][1] = 0.0f; m_[0][2] = 0.0f; m_[0][3] = 0.0f;
        m_[1][0] = 0.0f; m_[1][1] = 1.0f; m_[1][2] = 0.0f; m_[1][3] = 0.0f;
        m_[2][0] = 0.0f; m_[2][1] = 0.0f; m_[2][2] = 1.0f; m_[2][3] = 0.0f;
#endif
    }


    Matrix34& Matrix34::operator*=(const Matrix34& rhs)
    {
#if defined(LMATH_USE_SSE)
        lm128 rm0 = _mm_loadu_ps(&(rhs.m_[0][0]));
        lm128 rm1 = _mm_loadu_ps(&(rhs.m_[1][0]));
        lm128 rm2 = _mm_loadu_ps(&(rhs.m_[2][0]));

        lm128 t0, t1, t2;
        f32 t;
        for(s32 c=0; c<3; ++c){
            t0 = _mm_load1_ps(&(m_[c][0]));
            t1 = _mm_load1_ps(&(m_[c][1]));
            t2 = _mm_load1_ps(&(m_[c][2]));

            t0 = _mm_mul_ps(t0, rm0);
            t1 = _mm_mul_ps(t1, rm1);
            t2 = _mm_mul_ps(t2, rm2);

            t0 = _mm_add_ps(t0, t1);
            t0 = _mm_add_ps(t0, t2);

            t = m_[c][3];
            _mm_storeu_ps(&(m_[c][0]), t0);
            m_[c][3] += t;
        }

#else
        Matrix34 tmp = rhs;

        f32 x, y, z, w;
        for(s32 c=0; c<3; ++c){
            x = m_[c][0] * tmp.m_[0][0]
              + m_[c][1] * tmp.m_[1][0]
              + m_[c][2] * tmp.m_[2][0];

            y = m_[c][0] * tmp.m_[0][1]
              + m_[c][1] * tmp.m_[1][1]
              + m_[c][2] * tmp.m_[2][1];

            z = m_[c][0] * tmp.m_[0][2]
              + m_[c][1] * tmp.m_[1][2]
              + m_[c][2] * tmp.m_[2][2];

            w = m_[c][0] * tmp.m_[0][3]
              + m_[c][1] * tmp.m_[1][3]
              + m_[c][2] * tmp.m_[2][3]
              + m_[c][3];


            m_[c][0] = x;
            m_[c][1] = y;
            m_[c][2] = z;
            m_[c][3] = w;
        }
#endif

        return *this;
    }

    // 3x3部分行列の転置
    void Matrix34::transpose33()
    {
        lcore::swap(m_[0][1], m_[1][0]);
        lcore::swap(m_[0][2], m_[2][0]);
        lcore::swap(m_[1][2], m_[2][1]);

    }

    // 3x3部分行列の行列式
    f32 Matrix34::determinant33() const
    {
//#if defined(LMATH_USE_SSE)
#if 0
        //LIME_ALIGN16 f32 buffer[4];

        lm128 r0 = _mm_loadu_ps(&(m_[0][0]));
        lm128 r1 = _mm_loadu_ps(&(m_[1][0]));
        lm128 r2 = _mm_loadu_ps(&(m_[2][0]));

        static const u32 Shuffle0 = 201; // 1 2 0に並び替え
        static const u32 Shuffle1 = 210; // 2 0 1に並び替え

        lm128 t00 = _mm_shuffle_ps(r1, r1, Shuffle0);
        lm128 t01 = _mm_shuffle_ps(r1, r1, Shuffle1);

        lm128 t10 = _mm_shuffle_ps(r2, r2, Shuffle0);
        lm128 t11 = _mm_shuffle_ps(r2, r2, Shuffle1);

        t00 = _mm_mul_ps(t00, t11);
        t01 = _mm_mul_ps(t01, t10);

        t00 = _mm_sub_ps(t00, t01);

        t00 = _mm_mul_ps(r0, t00);

        t00 = _mm_add_ps( _mm_shuffle_ps(t00, t00, 0x4E), t00);
        t00 = _mm_add_ps( _mm_shuffle_ps(t00, t00, 0xB1), t00);

        f32 ret;
        _mm_store_ss(&ret, t00);
        //_mm_store_ps(buffer, t00);

        //f32 ret = buffer[0] + buffer[1] + buffer[2];
        return ret;
#else
        return m_[0][0] * (m_[1][1]*m_[2][2] - m_[1][2]*m_[2][1])
             + m_[0][1] * (m_[1][2]*m_[2][0] - m_[1][0]*m_[2][2])
             + m_[0][2] * (m_[1][0]*m_[2][1] - m_[1][1]*m_[2][0]);
#endif
    }

    // 3x3部分行列の逆行列
    void Matrix34::invert33()
    {
#if defined(LMATH_USE_SSE)
        LIME_ALIGN16 f32 buffer[4];

        lm128 c0 = _mm_loadu_ps(&m_[0][0]);
        lm128 c1 = _mm_loadu_ps(&m_[1][0]);
        lm128 c2 = _mm_loadu_ps(&m_[2][0]);

        lm128 t0 = _mm_shuffle_ps(c1, c1, 0xC9);//11 12 10 13
        lm128 t1 = _mm_shuffle_ps(c2, c2, 0xD2);//22 20 21 23

        lm128 t2 = _mm_shuffle_ps(c1, c1, 0xD2);//12 10 11 13
        lm128 t3 = _mm_shuffle_ps(c2, c2, 0xC9);//21 22 20 23

        t0 = _mm_mul_ps(t0, t1);
        t2 = _mm_mul_ps(t2, t3);
        t0 = _mm_sub_ps(t0, t2); //余因子行列　0列目

        //行列式の逆数計算
        //-------------------------------------------------------
        lm128 det = _mm_mul_ps(c0, t0);
        //_mm_store_ps(buffer, det);
        //buffer[0] = buffer[0] + buffer[1] + buffer[2]; //水平演算
        //det = _mm_load_ss(buffer);

        //水平演算
        det = _mm_add_ps( _mm_shuffle_ps(det, det, 0x4E), det);
        det = _mm_add_ps( _mm_shuffle_ps(det, det, 0xB1), det);

        //Newton-Raphson法で、行列式の逆数を計算
        {
            t1 = _mm_rcp_ss(det);
            det = _mm_mul_ss(det, t1);
            det = _mm_mul_ss(det, t1);
            t1 = _mm_add_ss(t1, t1);
            det = _mm_sub_ss(t1, det);
            det = _mm_shuffle_ps(det, det, 0);
        }

        //0列目
        t0 = _mm_mul_ps(t0, det);
        _mm_store_ps(buffer, t0);

        m_[0][0] = buffer[0];
        m_[1][0] = buffer[1];
        m_[2][0] = buffer[2];


        t0 = _mm_shuffle_ps(c0, c0, 0xD2);//02 00 01 03
        t1 = _mm_shuffle_ps(c2, c2, 0xC9);//21 22 20 23

        t2 = _mm_shuffle_ps(c0, c0, 0xC9);//01 02 00 03
        t3 = _mm_shuffle_ps(c2, c2, 0xD2);//22 20 21 23

        t0 = _mm_mul_ps(t0, t1);
        t2 = _mm_mul_ps(t2, t3);
        t0 = _mm_sub_ps(t0, t2); //余因子行列　1列目

        //1列目
        t0 = _mm_mul_ps(t0, det);
        _mm_store_ps(buffer, t0);

        m_[0][1] = buffer[0];
        m_[1][1] = buffer[1];
        m_[2][1] = buffer[2];


        t0 = _mm_shuffle_ps(c0, c0, 0xC9);//01 02 00 03
        t1 = _mm_shuffle_ps(c1, c1, 0xD2);//12 10 11 13

        t2 = _mm_shuffle_ps(c0, c0, 0xD2);//02 00 01 03
        t3 = _mm_shuffle_ps(c1, c1, 0xC9);//11 12 10 13

        t0 = _mm_mul_ps(t0, t1);
        t2 = _mm_mul_ps(t2, t3);
        t0 = _mm_sub_ps(t0, t2); //余因子行列　2列目

        //2列目
        t0 = _mm_mul_ps(t0, det);
        _mm_store_ps(buffer, t0);

        m_[0][2] = buffer[0];
        m_[1][2] = buffer[1];
        m_[2][2] = buffer[2];
#else
        f32 det = determinant33();

        LASSERT(isEqual(det, 0.0f) == false);

        f32 invDet = 1.0f / det;

        Matrix34 ret;
        ret.m_[0][0] = (m_[1][1]*m_[2][2] - m_[1][2]*m_[2][1]) * invDet;
        ret.m_[0][1] = (m_[0][2]*m_[2][1] - m_[0][1]*m_[2][2]) * invDet;
        ret.m_[0][2] = (m_[0][1]*m_[1][2] - m_[0][2]*m_[1][1]) * invDet;

        ret.m_[1][0] = (m_[1][2]*m_[2][0] - m_[1][0]*m_[2][2]) * invDet;
        ret.m_[1][1] = (m_[0][0]*m_[2][2] - m_[0][2]*m_[2][0]) * invDet;
        ret.m_[1][2] = (m_[0][2]*m_[1][0] - m_[0][0]*m_[1][2]) * invDet;

        ret.m_[2][0] = (m_[1][0]*m_[2][1] - m_[1][1]*m_[2][0]) * invDet;
        ret.m_[2][1] = (m_[0][1]*m_[2][0] - m_[0][0]*m_[2][1]) * invDet;
        ret.m_[2][2] = (m_[0][0]*m_[1][1] - m_[0][1]*m_[1][0]) * invDet;

        ret.m_[0][3] = m_[0][3];
        ret.m_[1][3] = m_[1][3];
        ret.m_[2][3] = m_[2][3];

        *this = ret;
#endif
    }

    // 逆行列
    void Matrix34::invert()
    {

#if defined(LMATH_USE_SSE)
        LIME_ALIGN16 f32 buffer0[4];
        LIME_ALIGN16 f32 buffer1[4];
        LIME_ALIGN16 f32 buffer2[4];

        lm128 c0 = _mm_loadu_ps(&m_[0][0]);
        lm128 c1 = _mm_loadu_ps(&m_[1][0]);
        lm128 c2 = _mm_loadu_ps(&m_[2][0]);

        lm128 t0 = _mm_shuffle_ps(c1, c1, 0xC9);//11 12 10 13
        lm128 t1 = _mm_shuffle_ps(c2, c2, 0xD2);//22 20 21 23

        lm128 t2 = _mm_shuffle_ps(c1, c1, 0xD2);//12 10 11 13
        lm128 t3 = _mm_shuffle_ps(c2, c2, 0xC9);//21 22 20 23

        t0 = _mm_mul_ps(t0, t1);
        t2 = _mm_mul_ps(t2, t3);
        t0 = _mm_sub_ps(t0, t2); //余因子行列　0列目

        //行列式の逆数計算
        //-------------------------------------------------------
        lm128 det = _mm_mul_ps(c0, t0);
        //_mm_store_ps(buffer0, det);
        //buffer0[0] = buffer0[0] + buffer0[1] + buffer0[2]; //水平演算
        //det = _mm_load_ss(buffer0);

        //水平演算
        det = _mm_add_ps( _mm_shuffle_ps(det, det, 0x4E), det);
        det = _mm_add_ps( _mm_shuffle_ps(det, det, 0xB1), det);

#if 0
        {//Newton-Raphson法で、行列式の逆数を計算
            t1 = _mm_rcp_ss(det);
            det = _mm_mul_ss(det, t1);
            det = _mm_mul_ss(det, t1);
            t1 = _mm_add_ss(t1, t1);
            det = _mm_sub_ss(t1, det);
            det = _mm_shuffle_ps(det, det, 0);
        }
#else
        {//行列式の逆数を計算
            t1 = _mm_set_ss(1.0f);
            det = _mm_div_ss(t1, det);
            det = _mm_shuffle_ps(det, det, 0);
        }
#endif

        //0列目
        t0 = _mm_mul_ps(t0, det);
        _mm_store_ps(buffer0, t0);

        m_[0][0] = buffer0[0];
        m_[1][0] = buffer0[1];
        m_[2][0] = buffer0[2];

        t0 = _mm_shuffle_ps(c0, c0, 0xD2);//02 00 01 03
        t1 = _mm_shuffle_ps(c2, c2, 0xC9);//21 22 20 23

        t2 = _mm_shuffle_ps(c0, c0, 0xC9);//01 02 00 03
        t3 = _mm_shuffle_ps(c2, c2, 0xD2);//22 20 21 23

        t0 = _mm_mul_ps(t0, t1);
        t2 = _mm_mul_ps(t2, t3);
        t0 = _mm_sub_ps(t0, t2); //余因子行列　1列目

        //1列目
        t0 = _mm_mul_ps(t0, det);
        _mm_store_ps(buffer1, t0);

        m_[0][1] = buffer1[0];
        m_[1][1] = buffer1[1];
        m_[2][1] = buffer1[2];

        t0 = _mm_shuffle_ps(c0, c0, 0xC9);//01 02 00 03
        t1 = _mm_shuffle_ps(c1, c1, 0xD2);//12 10 11 13

        t2 = _mm_shuffle_ps(c0, c0, 0xD2);//02 00 01 03
        t3 = _mm_shuffle_ps(c1, c1, 0xC9);//11 12 10 13

        t0 = _mm_mul_ps(t0, t1);
        t2 = _mm_mul_ps(t2, t3);
        t0 = _mm_sub_ps(t0, t2); //余因子行列　2列目

        //2列目
        t2 = _mm_mul_ps(t0, det);
        _mm_store_ps(buffer2, t2);

        m_[0][2] = buffer2[0];
        m_[1][2] = buffer2[1];
        m_[2][2] = buffer2[2];

        c0 = _mm_shuffle_ps(c0, c0, 0xFF);
        c1 = _mm_shuffle_ps(c1, c1, 0xFF);
        c2 = _mm_shuffle_ps(c2, c2, 0xFF);

        t0 = _mm_load_ps(buffer0);
        t1 = _mm_load_ps(buffer1);

        t0 = _mm_mul_ps(c0, t0);
        t1 = _mm_mul_ps(c1, t1);
        t2 = _mm_mul_ps(c2, t2);

        t0 = _mm_add_ps(_mm_add_ps(t0, t1), t2);

        _mm_store_ps(buffer0, t0);

        m_[0][3] = -buffer0[0];
        m_[1][3] = -buffer0[1];
        m_[2][3] = -buffer0[2];

#else
        f32 det = determinant33();

        LASSERT(isEqual(det, 0.0f) == false);

        f32 invDet = 1.0f / det;

        Matrix34 ret;
        ret.m_[0][0] = (m_[1][1]*m_[2][2] - m_[1][2]*m_[2][1]) * invDet;
        ret.m_[0][1] = (m_[0][2]*m_[2][1] - m_[0][1]*m_[2][2]) * invDet;
        ret.m_[0][2] = (m_[0][1]*m_[1][2] - m_[0][2]*m_[1][1]) * invDet;

        ret.m_[1][0] = (m_[1][2]*m_[2][0] - m_[1][0]*m_[2][2]) * invDet;
        ret.m_[1][1] = (m_[0][0]*m_[2][2] - m_[0][2]*m_[2][0]) * invDet;
        ret.m_[1][2] = (m_[0][2]*m_[1][0] - m_[0][0]*m_[1][2]) * invDet;

        ret.m_[2][0] = (m_[1][0]*m_[2][1] - m_[1][1]*m_[2][0]) * invDet;
        ret.m_[2][1] = (m_[0][1]*m_[2][0] - m_[0][0]*m_[2][1]) * invDet;
        ret.m_[2][2] = (m_[0][0]*m_[1][1] - m_[0][1]*m_[1][0]) * invDet;

        ret.m_[0][3] = -(m_[0][3]*ret.m_[0][0] + m_[1][3]*ret.m_[0][1] + m_[2][3]*ret.m_[0][2]);
        ret.m_[1][3] = -(m_[0][3]*ret.m_[1][0] + m_[1][3]*ret.m_[1][1] + m_[2][3]*ret.m_[1][2]);
        ret.m_[2][3] = -(m_[0][3]*ret.m_[2][0] + m_[1][3]*ret.m_[2][1] + m_[2][3]*ret.m_[2][2]);

        *this = ret;
#endif
    }


    void Matrix34::translate(const Vector3& v)
    {
        m_[0][3] += v.x_;
        m_[1][3] += v.y_;
        m_[2][3] += v.z_;
    }

    void Matrix34::translate(f32 x, f32 y, f32 z)
    {
        m_[0][3] += x;
        m_[1][3] += y;
        m_[2][3] += z;
    }

    void Matrix34::preTranslate(f32 x, f32 y, f32 z)
    {
        m_[0][3] += m_[0][0] * x + m_[0][1] * y + m_[0][2] * z;
        m_[1][3] += m_[1][0] * x + m_[1][1] * y + m_[1][2] * z;
        m_[2][3] += m_[2][0] * x + m_[2][1] * y + m_[2][2] * z;
    }


    void Matrix34::rotateX(f32 radian)
    {
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);

        f32 tmp[6];
        tmp[0] = m_[1][0]*cosA + m_[2][0]*sinA;
        tmp[1] = m_[1][1]*cosA + m_[2][1]*sinA;
        tmp[2] = m_[1][2]*cosA + m_[2][2]*sinA;

        tmp[3] = -m_[1][0]*sinA + m_[2][0]*cosA;
        tmp[4] = -m_[1][1]*sinA + m_[2][1]*cosA;
        tmp[5] = -m_[1][2]*sinA + m_[2][2]*cosA;

        m_[1][0] = tmp[0]; m_[1][1] = tmp[1]; m_[1][2] = tmp[2];
        m_[2][0] = tmp[3]; m_[2][1] = tmp[4]; m_[2][2] = tmp[5];
    }

    void Matrix34::rotateY(f32 radian)
    {
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);

        f32 tmp[6];
        tmp[0] = m_[0][0]*cosA - m_[2][0]*sinA;
        tmp[1] = m_[0][1]*cosA - m_[2][1]*sinA;
        tmp[2] = m_[0][2]*cosA - m_[2][2]*sinA;

        tmp[3] = m_[0][0]*sinA + m_[2][0]*cosA;
        tmp[4] = m_[0][1]*sinA + m_[2][1]*cosA;
        tmp[5] = m_[0][2]*sinA + m_[2][2]*cosA;

        m_[0][0] = tmp[0]; m_[0][1] = tmp[1]; m_[0][2] = tmp[2];
        m_[2][0] = tmp[3]; m_[2][1] = tmp[4]; m_[2][2] = tmp[5];
    }

    void Matrix34::rotateZ(f32 radian)
    {
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);

        f32 tmp[6];
        tmp[0] = m_[0][0]*cosA + m_[1][0]*sinA;
        tmp[1] = m_[0][1]*cosA + m_[1][1]*sinA;
        tmp[2] = m_[0][2]*cosA + m_[1][2]*sinA;

        tmp[3] = -m_[0][0]*sinA + m_[1][0]*cosA;
        tmp[4] = -m_[0][1]*sinA + m_[1][1]*cosA;
        tmp[5] = -m_[0][2]*sinA + m_[1][2]*cosA;

        m_[0][0] = tmp[0]; m_[0][1] = tmp[1]; m_[0][2] = tmp[2];
        m_[1][0] = tmp[3]; m_[1][1] = tmp[4]; m_[1][2] = tmp[5];
    }

    void Matrix34::setRotateAxis(f32 x, f32 y, f32 z, f32 radian)
    {
        f32 norm = lmath::sqrt(x*x + y*y + z*z);

        LASSERT(lmath::isEqual(norm, 0.0f) == false);

        norm = 1.0f / norm;
        x *= norm;
        y *= norm;
        z *= norm;

        f32 xx = x*x;
        f32 xy = x*y;
        f32 xz = x*z;

        f32 yx = y*x;
        f32 yy = y*y;
        f32 yz = y*z;

        f32 zx = z*x;
        f32 zy = z*y;
        f32 zz = z*z;

        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);
        f32 sx = sinA * x;
        f32 sy = sinA * y;
        f32 sz = sinA * z;

        f32 invCosA = 1.0f - cosA;

        m_[0][0] = (invCosA * xx) + cosA;
        m_[1][0] = (invCosA * xy) - (sz);
        m_[2][0] = (invCosA * xz) + (sy);

        m_[0][1] = (invCosA * yx) + (sz);
        m_[1][1] = (invCosA * yy) + (cosA);
        m_[2][1] = (invCosA * yz) - (sx);

        m_[0][2] = (invCosA * zx) - (sy);
        m_[1][2] = (invCosA * zy) + (sx);
        m_[2][2] = (invCosA * zz) + (cosA);

        m_[0][3] = 0.0f;
        m_[1][3] = 0.0f;
        m_[2][3] = 0.0f;
    }

    void Matrix34::mul(const Matrix34& m0, const Matrix34& m1)
    {
#if defined(LMATH_USE_SSE)
        lm128 rm0 = _mm_loadu_ps(&(m1.m_[0][0]));
        lm128 rm1 = _mm_loadu_ps(&(m1.m_[1][0]));
        lm128 rm2 = _mm_loadu_ps(&(m1.m_[2][0]));

        lm128 t0, t1, t2;
        for(s32 c=0; c<3; ++c){
            t0 = _mm_load1_ps(&(m0.m_[c][0]));
            t1 = _mm_load1_ps(&(m0.m_[c][1]));
            t2 = _mm_load1_ps(&(m0.m_[c][2]));

            t0 = _mm_mul_ps(t0, rm0);
            t1 = _mm_mul_ps(t1, rm1);
            t2 = _mm_mul_ps(t2, rm2);

            t0 = _mm_add_ps(t0, t1);
            t0 = _mm_add_ps(t0, t2);

            _mm_storeu_ps(&(m_[c][0]), t0);
            m_[c][3] += m0.m_[c][3];
        }

#else
        Matrix34 tmp;
        for(s32 c=0; c<3; ++c){
            tmp.m_[c][0] = m0.m_[c][0] * m1.m_[0][0]
              + m0.m_[c][1] * m1.m_[1][0]
              + m0.m_[c][2] * m1.m_[2][0];

            tmp.m_[c][1] =  m0.m_[c][0] * m1.m_[0][1]
              + m0.m_[c][1] * m1.m_[1][1]
              + m0.m_[c][2] * m1.m_[2][1];

            tmp.m_[c][2] =  m0.m_[c][0] * m1.m_[0][2]
              + m0.m_[c][1] * m1.m_[1][2]
              + m0.m_[c][2] * m1.m_[2][2];

            tmp.m_[c][3] = m0.m_[c][0] * m1.m_[0][3]
              + m0.m_[c][1] * m1.m_[1][3]
              + m0.m_[c][2] * m1.m_[2][3]
              + m0.m_[c][3];
        }
        *this = tmp;
#endif
    }

    Matrix34& Matrix34::operator*=(const Matrix44& rhs)
    {
#if defined(LMATH_USE_SSE)
        lm128 rm0 = _mm_loadu_ps(&(rhs.m_[0][0]));
        lm128 rm1 = _mm_loadu_ps(&(rhs.m_[1][0]));
        lm128 rm2 = _mm_loadu_ps(&(rhs.m_[2][0]));
        lm128 rm3 = _mm_loadu_ps(&(rhs.m_[3][0]));

        lm128 t0, t1, t2, t3;
        for(s32 c=0; c<3; ++c){
            t0 = _mm_load1_ps(&(m_[c][0]));
            t1 = _mm_load1_ps(&(m_[c][1]));
            t2 = _mm_load1_ps(&(m_[c][2]));
            t3 = _mm_load1_ps(&(m_[c][3]));

            t0 = _mm_mul_ps(t0, rm0);
            t1 = _mm_mul_ps(t1, rm1);
            t2 = _mm_mul_ps(t2, rm2);
            t3 = _mm_mul_ps(t3, rm3);

            t0 = _mm_add_ps(t0, t1);
            t0 = _mm_add_ps(t0, t2);
            t0 = _mm_add_ps(t0, t3);

            _mm_storeu_ps(&(m_[c][0]), t0);
        }

#else
        f32 x, y, z, w;
        for(s32 c=0; c<3; ++c){
            x = m_[c][0] * rhs.m_[0][0]
                + m_[c][1] * rhs.m_[1][0]
                + m_[c][2] * rhs.m_[2][0]
                + m_[c][3] * rhs.m_[3][0];

            y = m_[c][0] * rhs.m_[0][1]
                + m_[c][1] * rhs.m_[1][1]
                + m_[c][2] * rhs.m_[2][1]
                + m_[c][3] * rhs.m_[3][1];

            z = m_[c][0] * rhs.m_[0][2]
                + m_[c][1] * rhs.m_[1][2]
                + m_[c][2] * rhs.m_[2][2]
                + m_[c][3] * rhs.m_[3][2];

            w = m_[c][0] * rhs.m_[0][3]
                + m_[c][1] * rhs.m_[1][3]
                + m_[c][2] * rhs.m_[2][3]
                + m_[c][3] * rhs.m_[3][3];

            m_[c][0] = x;
            m_[c][1] = y;
            m_[c][2] = z;
            m_[c][3] = w;
        }
#endif
        return *this;
    }

    bool Matrix34::isNan() const
    {
        for(u32 i=0; i<3; ++i){
            for(u32 j=0; j<4; ++j){
                if(lcore::isNan(m_[i][j])){
                    return true;
                }
            }
        }
        return false;
    }

    void Matrix34::getRotation(Quaternion& rotation) const
    {
        f32 trace0 = m_[0][0] + m_[1][1] + m_[2][2];
        f32 trace1 = m_[0][0] - m_[1][1] - m_[2][2];
        f32 trace2 = m_[1][1] - m_[0][0] - m_[2][2];
        f32 trace3 = m_[2][2] - m_[0][0] - m_[1][1];

        s32 index = 0;
        f32 trace = trace0;
        if(trace1>trace){
            index = 1;
            trace = trace1;
        }
        if(trace2>trace){
            index = 2;
            trace = trace2;
        }
        if(trace3>trace){
            index = 3;
            trace = trace3;
        }

        f32 value = lmath::sqrt(trace + 1.0f) * 0.5f;
        f32 m = 0.25f/value;

        switch(index)
        {
        case 0:
            rotation.w_ = value;
            rotation.x_ = (m_[1][2] - m_[2][1]) * m;
            rotation.y_ = (m_[2][0] - m_[0][2]) * m;
            rotation.z_ = (m_[0][1] - m_[1][0]) * m;
            break;

        case 1:
            rotation.x_ = value;
            rotation.w_ = (m_[1][2] - m_[2][1]) * m;
            rotation.y_ = (m_[0][1] + m_[0][1]) * m;
            rotation.z_ = (m_[2][0] + m_[0][2]) * m;
            break;

        case 2:
            rotation.y_ = value;
            rotation.w_ = (m_[2][0] - m_[0][2]) * m;
            rotation.x_ = (m_[0][1] + m_[1][0]) * m;
            rotation.z_ = (m_[1][2] + m_[2][1]) * m;
            break;

        case 3:
            rotation.z_ = value;
            rotation.w_ = (m_[0][1] - m_[1][0]) * m;
            rotation.x_ = (m_[2][0] + m_[0][2]) * m;
            rotation.y_ = (m_[1][2] + m_[2][1]) * m;
            break;
        }
    }

#if defined(LMATH_USE_SSE)
    //SSEセット・ストア命令

    void Matrix34::load(lm128& r0, lm128& r1, lm128& r2, const Matrix34& m)
    {
        r0 = _mm_loadu_ps(&(m.m_[0][0]));
        r1 = _mm_loadu_ps(&(m.m_[1][0]));
        r2 = _mm_loadu_ps(&(m.m_[2][0]));
    }

    void Matrix34::store(Matrix34& m, const lm128& r0, const lm128& r1, const lm128& r2)
    {
        _mm_storeu_ps(&(m.m_[0][0]), r0);
        _mm_storeu_ps(&(m.m_[1][0]), r1);
        _mm_storeu_ps(&(m.m_[2][0]), r2);
    }
#endif
}
