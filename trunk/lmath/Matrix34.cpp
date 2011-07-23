/**
@file Matrix34.cpp
@author t-sakai
@date 2011/06/30
*/
#include "Matrix34.h"
#include "Matrix44.h"

namespace lmath
{

    //--------------------------------------------
    //---
    //--- Matrix34
    //---
    //--------------------------------------------
    Matrix34::Matrix34(const Matrix44& rhs)
    {
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] = rhs.m_[i][j];
            }
        }
    }

    Matrix34& Matrix34::operator=(const Matrix34& rhs)
    {
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] = rhs.m_[i][j];
            }
        }
        return *this;
    }

    Matrix34& Matrix34::operator*=(f32 f)
    {
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] *= f;
            }
        }
        return *this;
    }


    Matrix34& Matrix34::operator+=(const Matrix34& rhs)
    {
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] += rhs.m_[i][j];
            }
        }
        return *this;
    }


    Matrix34& Matrix34::operator-=(const Matrix34& rhs)
    {
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] -= rhs.m_[i][j];
            }
        }
        return *this;
    }


    Matrix34 Matrix34::operator-() const
    {
        Matrix34 ret;
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                ret.m_[i][j] = -m_[i][j];
            }
        }
        return ret;
    }


    void Matrix34::identity()
    {
        m_[0][0] = 1.0f; m_[0][1] = 0.0f; m_[0][2] = 0.0f; m_[0][3] = 0.0f;
        m_[1][0] = 0.0f; m_[1][1] = 1.0f; m_[1][2] = 0.0f; m_[1][3] = 0.0f;
        m_[2][0] = 0.0f; m_[2][1] = 0.0f; m_[2][2] = 1.0f; m_[2][3] = 0.0f;
    }


    Matrix34& Matrix34::operator*=(const Matrix34& rhs)
    {
        f32 x, y, z, w;
        for(s32 c=0; c<3; ++c){
            x = m_[c][0] * rhs.m_[0][0]
              + m_[c][1] * rhs.m_[1][0]
              + m_[c][2] * rhs.m_[2][0];

            y = m_[c][0] * rhs.m_[0][1]
              + m_[c][1] * rhs.m_[1][1]
              + m_[c][2] * rhs.m_[2][1];

            z = m_[c][0] * rhs.m_[0][2]
              + m_[c][1] * rhs.m_[1][2]
              + m_[c][2] * rhs.m_[2][2];

            w = m_[c][0] * rhs.m_[0][3]
              + m_[c][1] * rhs.m_[1][3]
              + m_[c][2] * rhs.m_[2][3]
              + m_[c][3];


            m_[c][0] = x;
            m_[c][1] = y;
            m_[c][2] = z;
            m_[c][3] = w;
        }

        return *this;
    }

    // 3x3部分行列の転置
    void Matrix34::transpose33()
    {
        lmath::swap(m_[0][1], m_[1][0]);
        lmath::swap(m_[0][2], m_[2][0]);
        lmath::swap(m_[1][2], m_[2][1]);

    }

    // 3x3部分行列の行列式
    f32 Matrix34::determinant33() const
    {
        return m_[0][0] * (m_[1][1]*m_[2][2] - m_[1][2]*m_[2][1])
             + m_[0][1] * (m_[1][2]*m_[2][0] - m_[1][0]*m_[2][2])
             + m_[0][2] * (m_[1][0]*m_[2][1] - m_[1][1]*m_[2][0]);
    }

    // 3x3部分行列の逆行列
    void Matrix34::invert33()
    {
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
    }

    // 逆行列
    void Matrix34::invert()
    {
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

        f32 xSqr = x*x;
        f32 ySqr = y*y;
        f32 zSqr = z*z;

        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);
        f32 invCosA = 1.0f - cosA;

        m_[0][0] = (invCosA * xSqr) + cosA;
        m_[1][0] = (invCosA * x * y) - (sinA * z);
        m_[2][0] = (invCosA * x * z) + (sinA * y);

        m_[0][1] = (invCosA * x * y) + (sinA * z);
        m_[1][1] = (invCosA * ySqr) + (cosA);
        m_[2][1] = (invCosA * y * z) - (sinA * x);

        m_[0][2] = (invCosA * x * z) - (sinA * y);
        m_[1][2] = (invCosA * y * z) + (sinA * x);
        m_[2][2] = (invCosA * zSqr) + (cosA);

        m_[0][3] = 0.0f;
        m_[1][3] = 0.0f;
        m_[2][3] = 0.0f;
    }

    void Matrix34::mul(const Matrix34& m0, const Matrix34& m1)
    {
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
    }

    Matrix34& Matrix34::operator*=(const Matrix44& rhs)
    {
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
}
