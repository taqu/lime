/**
@file Matrix44.cpp
@author t-sakai
@date 2009/01/18 create
*/
#include "Matrix44.h"
#include "Matrix34.h"

namespace lmath
{
    //--------------------------------------------
    //---
    //--- Matrix44
    //---
    //--------------------------------------------
    Matrix44::Matrix44(const Matrix34& mat)
    {
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] = mat.m_[i][j];
            }
        }
        m_[3][0] = m_[3][1] = m_[3][2] = 0.0f;
        m_[3][3] = 1.0f;
    }

    Matrix44& Matrix44::operator=(const Matrix44& rhs)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] = rhs.m_[i][j];
            }
        }
        return *this;
    }


    Matrix44& Matrix44::operator*=(f32 f)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] *= f;
            }
        }
        return *this;
    }

    Matrix44& Matrix44::operator+=(const Matrix44& rhs)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] += rhs.m_[i][j];
            }
        }
        return *this;
    }

    Matrix44& Matrix44::operator-=(const Matrix44& rhs)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] -= rhs.m_[i][j];
            }
        }
        return *this;
    }

    Matrix44 Matrix44::operator-() const
    {
        Matrix44 ret;
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<4; ++j){
                ret.m_[i][j] = -m_[i][j];
            }
        }
        return ret;
    }


    Matrix44& Matrix44::operator*=(const Matrix34& rhs)
    {
        f32 x, y, z, w;
        for(s32 c=0; c<4; ++c){
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


    Matrix44& Matrix44::operator=(const Matrix34& rhs)
    {
        for(s32 i=0; i<3; ++i){
            for(s32 j=0; j<4; ++j){
                m_[i][j] = rhs.m_[i][j];
            }
        }
        m_[3][0] = m_[3][1] = m_[3][2] = 0.0f;
        m_[3][3] = 1.0f;
        return *this;
    }

    void Matrix44::identity()
    {
        m_[0][0] = 1.0f; m_[0][1] = 0.0f; m_[0][2] = 0.0f; m_[0][3] = 0.0f;
        m_[1][0] = 0.0f; m_[1][1] = 1.0f; m_[1][2] = 0.0f; m_[1][3] = 0.0f;
        m_[2][0] = 0.0f; m_[2][1] = 0.0f; m_[2][2] = 1.0f; m_[2][3] = 0.0f;
        m_[3][0] = 0.0f; m_[3][1] = 0.0f; m_[3][2] = 0.0f; m_[3][3] = 1.0f;
    }

    Matrix44& Matrix44::operator*=(const Matrix44& rhs)
    {
        f32 x, y, z, w;
        for(s32 c=0; c<4; ++c){
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

    void Matrix44::transpose()
    {
        lmath::swap(m_[0][1], m_[1][0]);
        lmath::swap(m_[0][2], m_[2][0]);
        lmath::swap(m_[0][3], m_[3][0]);
        lmath::swap(m_[1][2], m_[2][1]);
        lmath::swap(m_[1][3], m_[3][1]);
        lmath::swap(m_[2][3], m_[3][2]);
    }

    f32 Matrix44::determinant() const
    {
        f32 tmp[4];
        tmp[0] = m_[0][0] * (m_[1][1]*m_[2][2]*m_[3][3] + m_[1][2]*m_[2][3]*m_[3][1] + m_[1][3]*m_[2][1]*m_[3][2]
                           - m_[1][1]*m_[2][3]*m_[3][2] - m_[1][2]*m_[2][1]*m_[3][3] - m_[1][3]*m_[2][2]*m_[3][1]);

        tmp[1] = m_[0][1] * (m_[1][0]*m_[2][3]*m_[3][2] + m_[1][2]*m_[2][0]*m_[3][3] + m_[1][3]*m_[2][2]*m_[3][0]
                           - m_[1][0]*m_[2][2]*m_[3][3] - m_[1][2]*m_[2][3]*m_[3][0] - m_[1][3]*m_[2][0]*m_[3][2]);

        tmp[2] = m_[0][2] * (m_[1][0]*m_[2][1]*m_[3][2] + m_[1][1]*m_[2][3]*m_[3][0] + m_[1][3]*m_[2][0]*m_[3][1]
                           - m_[1][0]*m_[2][3]*m_[3][1] - m_[1][1]*m_[2][0]*m_[3][3] - m_[1][3]*m_[2][1]*m_[3][0]);

        tmp[3] = m_[0][3] * (m_[1][0]*m_[2][2]*m_[3][1] + m_[1][1]*m_[2][0]*m_[3][2] + m_[1][2]*m_[2][1]*m_[3][0]
                           - m_[1][0]*m_[2][1]*m_[3][2] - m_[1][1]*m_[2][2]*m_[3][0] - m_[1][2]*m_[2][0]*m_[3][1]);

        return (tmp[0] + tmp[1] + tmp[2] + tmp[3]);
    }

    void Matrix44::invert()
    {

        Matrix44 tmp(*this);
        Matrix44 tmp2;
        tmp2.identity();

        for(s32 j=0; j<4; j++){
            // Find the row with max value in this column
            s32 rowMax = j; // Points to max abs value row in this column
            for(s32 i=j+1; i<4; i++){
                if( lcore::absolute(tmp.m_[j][i]) > lcore::absolute(tmp.m_[j][rowMax]) ){
                    rowMax = i;
                }
            }

            // If the max value here is 0, we can't invert.
            if( lmath::isEqual(tmp.m_[j][rowMax], 0.0f) ){
                return;
            }

            // Swap row "rowMax" with row "j"
            for(s32 cc=0; cc<4; ++cc){
                lmath::swap(tmp.m_[cc][j], tmp.m_[cc][rowMax]);
                lmath::swap(tmp2.m_[cc][j], tmp2.m_[cc][rowMax]);
            }

            // Now everything we do is on row "c".
            // Set the max cell to 1 by dividing the entire row by that value
            f32 invPivot = 1.0f/tmp(j, j);
            for(s32 cc=0; cc<4; ++cc){
                tmp.m_[cc][j] *= invPivot;
                tmp2.m_[cc][j] *= invPivot;
            }

            f32 pivot;
            // Now do the other rows, so that this column only has a 1 and 0's
            for(s32 i=0; i<4; ++i){
                if(i != j){
                    pivot = tmp.m_[j][i];
                    for(s32 cc=0; cc<4; cc++)
                    {
                        tmp.m_[cc][i] -= tmp.m_[cc][j] * pivot;
                        tmp2.m_[cc][i] -= tmp2.m_[cc][j] * pivot;
                    }
                }
            }

        }

        (*this) = tmp2;
    }

    void Matrix44::translate(const Vector3& v)
    {
        m_[0][3] += v.x_;
        m_[1][3] += v.y_;
        m_[2][3] += v.z_;
    }

    void Matrix44::translate(f32 x, f32 y, f32 z)
    {
        m_[0][3] += x;
        m_[1][3] += y;
        m_[2][3] += z;
    }

    void Matrix44::preTranslate(f32 x, f32 y, f32 z)
    {
        m_[0][3] += m_[0][0] * x + m_[0][1] * y + m_[0][2] * z;
        m_[1][3] += m_[1][0] * x + m_[1][1] * y + m_[1][2] * z;
        m_[2][3] += m_[2][0] * x + m_[2][1] * y + m_[2][2] * z;
    }


    void Matrix44::rotateX(f32 radian)
    {
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);

#if 0
        Matrix44 rotation;
        rotation(0, 0) = 1.0f; rotation(0, 1) = 0.0f;  rotation(0, 2) = 0.0f; rotation(0, 3) = 0.0f;
        rotation(1, 0) = 0.0f; rotation(1, 1) = cosA;  rotation(1, 2) = sinA; rotation(1, 3) = 0.0f;
        rotation(2, 0) = 0.0f; rotation(2, 1) = -sinA; rotation(2, 2) = cosA; rotation(2, 3) = 0.0f;
        rotation(3, 0) = 0.0f; rotation(3, 1) = 0.0f;  rotation(3, 2) = 0.0f; rotation(3, 3) = 1.0f;

        *this *= rotation;

#else
        f32 tmp[6];
        tmp[0] = m_[1][0]*cosA + m_[2][0]*sinA;
        tmp[1] = m_[1][1]*cosA + m_[2][1]*sinA;
        tmp[2] = m_[1][2]*cosA + m_[2][2]*sinA;

        tmp[3] = -m_[1][0]*sinA + m_[2][0]*cosA;
        tmp[4] = -m_[1][1]*sinA + m_[2][1]*cosA;
        tmp[5] = -m_[1][2]*sinA + m_[2][2]*cosA;

        m_[1][0] = tmp[0]; m_[1][1] = tmp[1]; m_[1][2] = tmp[2];
        m_[2][0] = tmp[3]; m_[2][1] = tmp[4]; m_[2][2] = tmp[5];
#endif

    }

    void Matrix44::rotateY(f32 radian)
    {
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);

#if 0
        Matrix44 rotation;
        rotation(0, 0) = cosA; rotation(0, 1) = 0.0f;  rotation(0, 2) = -sinA; rotation(0, 3) = 0.0f;
        rotation(1, 0) = 0.0f; rotation(1, 1) = 1.0f;  rotation(1, 2) = 0.0f; rotation(1, 3) = 0.0f;
        rotation(2, 0) = sinA; rotation(2, 1) = 0.0f; rotation(2, 2) = cosA; rotation(2, 3) = 0.0f;
        rotation(3, 0) = 0.0f; rotation(3, 1) = 0.0f;  rotation(3, 2) = 0.0f; rotation(3, 3) = 1.0f;

        *this *= rotation;

#else
        f32 tmp[6];
        tmp[0] = m_[0][0]*cosA - m_[2][0]*sinA;
        tmp[1] = m_[0][1]*cosA - m_[2][1]*sinA;
        tmp[2] = m_[0][2]*cosA - m_[2][2]*sinA;

        tmp[3] = m_[0][0]*sinA + m_[2][0]*cosA;
        tmp[4] = m_[0][1]*sinA + m_[2][1]*cosA;
        tmp[5] = m_[0][2]*sinA + m_[2][2]*cosA;

        m_[0][0] = tmp[0]; m_[0][1] = tmp[1]; m_[0][2] = tmp[2];
        m_[2][0] = tmp[3]; m_[2][1] = tmp[4]; m_[2][2] = tmp[5];
#endif
    }

    void Matrix44::rotateZ(f32 radian)
    {
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);

#if 0
        Matrix44 rotation;
        rotation(0, 0) = cosA; rotation(0, 1) = sinA;  rotation(0, 2) = 0.0f; rotation(0, 3) = 0.0f;
        rotation(1, 0) = -sinA; rotation(1, 1) = cosA;  rotation(1, 2) = 0.0f; rotation(1, 3) = 0.0f;
        rotation(2, 0) = 0.0f; rotation(2, 1) = 0.0f; rotation(2, 2) = 1.0f; rotation(2, 3) = 0.0f;
        rotation(3, 0) = 0.0f; rotation(3, 1) = 0.0f;  rotation(3, 2) = 0.0f; rotation(3, 3) = 1.0f;

        *this *= rotation;

#else
        f32 tmp[6];
        tmp[0] = m_[0][0]*cosA + m_[1][0]*sinA;
        tmp[1] = m_[0][1]*cosA + m_[1][1]*sinA;
        tmp[2] = m_[0][2]*cosA + m_[1][2]*sinA;

        tmp[3] = -m_[0][0]*sinA + m_[1][0]*cosA;
        tmp[4] = -m_[0][1]*sinA + m_[1][1]*cosA;
        tmp[5] = -m_[0][2]*sinA + m_[1][2]*cosA;

        m_[0][0] = tmp[0]; m_[0][1] = tmp[1]; m_[0][2] = tmp[2];
        m_[1][0] = tmp[3]; m_[1][1] = tmp[4]; m_[1][2] = tmp[5];
#endif
    }

    void Matrix44::setRotateAxis(f32 x, f32 y, f32 z, f32 radian)
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
        m_[3][0] = 0.0f;

        m_[0][1] = (invCosA * x * y) + (sinA * z);
        m_[1][1] = (invCosA * ySqr) + (cosA);
        m_[2][1] = (invCosA * y * z) - (sinA * x);
        m_[3][1] = 0.0f;

        m_[0][2] = (invCosA * x * z) - (sinA * y);
        m_[1][2] = (invCosA * y * z) + (sinA * x);
        m_[2][2] = (invCosA * zSqr) + (cosA);
        m_[3][2] = 0.0f;

        m_[0][3] = 0.0f;
        m_[1][3] = 0.0f;
        m_[2][3] = 0.0f;
        m_[3][3] = 1.0f;
    }

    void Matrix44::lookAt(const Vector3& eye, const Vector3& at, const Vector3& up)
    {
        Vector3 xaxis, yaxis, zaxis = at;
        zaxis -= eye;
        zaxis.normalize();
        
        xaxis.cross(up, zaxis);
        xaxis.normalize();

        yaxis.cross(zaxis, xaxis);

#if 0
        m_[0][0] = xaxis.x_; m_[0][1] = yaxis.x_; m_[0][2] = zaxis.x_; m_[0][3] = 0.0f;
        m_[1][0] = xaxis.y_; m_[1][1] = yaxis.y_; m_[1][2] = zaxis.y_; m_[1][3] = 0.0f;
        m_[2][0] = xaxis.z_; m_[2][1] = yaxis.z_; m_[2][2] = zaxis.z_; m_[2][3] = 0.0f;
        m_[3][0] = -eye.dot(xaxis); m_[3][1] = -eye.dot(yaxis); m_[3][2] = -eye.dot(zaxis); m_[3][3] = 1.0f;
#else
        m_[0][0] = xaxis.x_; m_[0][1] = xaxis.y_; m_[0][2] = xaxis.z_; m_[0][3] = -eye.dot(xaxis);
        m_[1][0] = yaxis.x_; m_[1][1] = yaxis.y_; m_[1][2] = yaxis.z_; m_[1][3] = -eye.dot(yaxis);
        m_[2][0] = zaxis.x_; m_[2][1] = zaxis.y_; m_[2][2] = zaxis.z_; m_[2][3] = -eye.dot(zaxis);
        m_[3][0] = 0.0f; m_[3][1] = 0.0f; m_[3][2] = 0.0f; m_[3][3] = 1.0f;
#endif
    }

    void Matrix44::perspective(f32 width, f32 height, f32 znear, f32 zfar)
    {
#if 0
        m_[0][0] = 2.0f*znear/width; m_[0][1] = 0.0f;              m_[0][2] = 0.0f;              m_[0][3] = 0.0f;
        m_[1][0] = 0.0f;             m_[1][1] = 2.0f*znear/height; m_[1][2] = 0.0f;              m_[1][3] = 0.0f;
        m_[2][0] = 0.0f;             m_[2][1] = 0.0f;              m_[2][2] = zfar/(zfar-znear); m_[2][3] = 1.0f;
        m_[3][0] = 0.0f;             m_[3][1] = 0.0f;              m_[3][2] = znear*zfar/(znear-zfar); m_[3][3] = 0.0f;

#else
        m_[0][0] = 2.0f*znear/width; m_[0][1] = 0.0f;              m_[0][2] = 0.0f;              m_[0][3] = 0.0f;
        m_[1][0] = 0.0f;             m_[1][1] = 2.0f*znear/height; m_[1][2] = 0.0f;              m_[1][3] = 0.0f;
        m_[2][0] = 0.0f;             m_[2][1] = 0.0f;              m_[2][2] = zfar/(zfar-znear); m_[2][3] = znear*zfar/(znear-zfar);
        m_[3][0] = 0.0f;             m_[3][1] = 0.0f;              m_[3][2] = 1.0f; m_[3][3] = 0.0f;
#endif
    }

    void Matrix44::perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar)
    {
        f32 yscale = 1.0f / lmath::tan(0.5f * fovy);
        f32 xscale = yscale / aspect;
#if 0
        m_[0][0] = xscale; m_[0][1] = 0.0f;   m_[0][2] = 0.0f;              m_[0][3] = 0.0f;
        m_[1][0] = 0.0f;   m_[1][1] = yscale; m_[1][2] = 0.0f;              m_[1][3] = 0.0f;
        m_[2][0] = 0.0f;   m_[2][1] = 0.0f;   m_[2][2] = zfar/(zfar-znear); m_[2][3] = 1.0f;
        m_[3][0] = 0.0f;   m_[3][1] = 0.0f;   m_[3][2] = znear*zfar/(znear-zfar); m_[3][3] = 0.0f;

#else
        m_[0][0] = xscale; m_[0][1] = 0.0f;   m_[0][2] = 0.0f;              m_[0][3] = 0.0f;
        m_[1][0] = 0.0f;   m_[1][1] = yscale; m_[1][2] = 0.0f;              m_[1][3] = 0.0f;
        m_[2][0] = 0.0f;   m_[2][1] = 0.0f;   m_[2][2] = zfar/(zfar-znear); m_[2][3] = znear*zfar/(znear-zfar);
        m_[3][0] = 0.0f;   m_[3][1] = 0.0f;   m_[3][2] = 1.0f; m_[3][3] = 0.0f;
#endif
    }

    void Matrix44::ortho(f32 width, f32 height, f32 znear, f32 zfar)
    {
#if 0
        m_[0][0] = 2.0f/width; m_[0][1] = 0.0f;        m_[0][2] = 0.0f;              m_[0][3] = 0.0f;
        m_[1][0] = 0.0f;       m_[1][1] = 2.0f/height; m_[1][2] = 0.0f;              m_[1][3] = 0.0f;
        m_[2][0] = 0.0f;       m_[2][1] = 0.0f;        m_[2][2] = 1.0f/(zfar-znear); m_[2][3] = 0.0f;
        m_[3][0] = 0.0f;       m_[3][1] = 0.0f;        m_[3][2] = znear/(znear-zfar); m_[3][3] = 1.0f;

#else
        m_[0][0] = 2.0f/width; m_[0][1] = 0.0f;        m_[0][2] = 0.0f;              m_[0][3] = 0.0f;
        m_[1][0] = 0.0f;       m_[1][1] = 2.0f/height; m_[1][2] = 0.0f;              m_[1][3] = 0.0f;
        m_[2][0] = 0.0f;       m_[2][1] = 0.0f;        m_[2][2] = 1.0f/(zfar-znear); m_[2][3] = znear/(znear-zfar);
        m_[3][0] = 0.0f;       m_[3][1] = 0.0f;        m_[3][2] = 0.0f; m_[3][3] = 1.0f;
#endif
    }

    void Matrix44::perspectiveLinearZ(f32 width, f32 height, f32 znear, f32 zfar)
    {
#if 0
        m_[0][0] = 2.0f*znear/width; m_[0][1] = 0.0f;              m_[0][2] = 0.0f;              m_[0][3] = 0.0f;
        m_[1][0] = 0.0f;             m_[1][1] = 2.0f*znear/height; m_[1][2] = 0.0f;              m_[1][3] = 0.0f;
        m_[2][0] = 0.0f;             m_[2][1] = 0.0f;              m_[2][2] = 1.0f/(zfar-znear); m_[2][3] = 1.0f;
        m_[3][0] = 0.0f;             m_[3][1] = 0.0f;              m_[3][2] = znear/(znear-zfar); m_[3][3] = 0.0f;

#else
        m_[0][0] = 2.0f*znear/width; m_[0][1] = 0.0f;              m_[0][2] = 0.0f;              m_[0][3] = 0.0f;
        m_[1][0] = 0.0f;             m_[1][1] = 2.0f*znear/height; m_[1][2] = 0.0f;              m_[1][3] = 0.0f;
        m_[2][0] = 0.0f;             m_[2][1] = 0.0f;              m_[2][2] = 1.0f/(zfar-znear); m_[2][3] = znear/(znear-zfar);
        m_[3][0] = 0.0f;             m_[3][1] = 0.0f;              m_[3][2] = 1.0f; m_[3][3] = 0.0f;
#endif
    }

    void Matrix44::perspectiveFovLinearZ(f32 fovy, f32 aspect, f32 znear, f32 zfar)
    {
        f32 yscale = 1.0f / lmath::tan(0.5f * fovy);
        f32 xscale = yscale / aspect;

#if 0
        m_[0][0] = xscale; m_[0][1] = 0.0f;   m_[0][2] = 0.0f;              m_[0][3] = 0.0f;
        m_[1][0] = 0.0f;   m_[1][1] = yscale; m_[1][2] = 0.0f;              m_[1][3] = 0.0f;
        m_[2][0] = 0.0f;   m_[2][1] = 0.0f;   m_[2][2] = 1.0f/(zfar-znear); m_[2][3] = 1.0f;
        m_[3][0] = 0.0f;   m_[3][1] = 0.0f;   m_[3][2] = znear/(znear-zfar); m_[3][3] = 0.0f;

#else
        m_[0][0] = xscale; m_[0][1] = 0.0f;   m_[0][2] = 0.0f;              m_[0][3] = 0.0f;
        m_[1][0] = 0.0f;   m_[1][1] = yscale; m_[1][2] = 0.0f;              m_[1][3] = 0.0f;
        m_[2][0] = 0.0f;   m_[2][1] = 0.0f;   m_[2][2] = 1.0f/(zfar-znear); m_[2][3] = znear/(znear-zfar);
        m_[3][0] = 0.0f;   m_[3][1] = 0.0f;   m_[3][2] = 1.0f; m_[3][3] = 0.0f;
#endif
    }

    bool Matrix44::isNan() const
    {
        for(u32 i=0; i<4; ++i){
            for(u32 j=0; j<4; ++j){
                if(lcore::isNan(m_[i][j])){
                    return true;
                }
            }
        }
        return false;
    }
}
