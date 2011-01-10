/**
@file Matrix43.cpp
@author t-sakai
@date 2009/01/18 create
*/
#include "Matrix43.h"
#include "Vector3.h"
#include "Matrix44.h"

namespace lmath
{

    //--------------------------------------------
    //---
    //--- Matrix43
    //---
    //--------------------------------------------
    Matrix43& Matrix43::operator=(const Matrix43& rhs)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<3; ++j){
                _elem[i][j] = rhs._elem[i][j];
            }
        }
        return *this;
    }

    Matrix43& Matrix43::operator*=(f32 f)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<3; ++j){
                _elem[i][j] *= f;
            }
        }
        return *this;
    }


    Matrix43& Matrix43::operator+=(const Matrix43& rhs)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<3; ++j){
                _elem[i][j] += rhs(i, j);
            }
        }
        return *this;
    }


    Matrix43& Matrix43::operator-=(const Matrix43& rhs)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<3; ++j){
                _elem[i][j] -= rhs(i, j);
            }
        }
        return *this;
    }


    Matrix43 Matrix43::operator-() const
    {
        Matrix43 ret;
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<3; ++j){
                ret(i, j) = -_elem[i][j];
            }
        }
        return ret;
    }


    void Matrix43::identity()
    {
        _elem[0][0] = 1.0f; _elem[0][1] = 0.0f; _elem[0][2] = 0.0f;
        _elem[1][0] = 0.0f; _elem[1][1] = 1.0f; _elem[1][2] = 0.0f;
        _elem[2][0] = 0.0f; _elem[2][1] = 0.0f; _elem[2][2] = 1.0f;
        _elem[3][0] = 0.0f; _elem[3][1] = 0.0f; _elem[3][2] = 0.0f;
    }


    Matrix43& Matrix43::operator*=(const Matrix43& rhs)
    {
        f32 x, y, z;
        Vector3 t;
        for(s32 c=0; c<3; ++c){
            x = _elem[c][0] * rhs(0, 0)
                + _elem[c][1] * rhs(1, 0)
                + _elem[c][2] * rhs(2, 0);

            y = _elem[c][0] * rhs(0, 1)
                + _elem[c][1] * rhs(1, 1)
                + _elem[c][2] * rhs(2, 1);

            z = _elem[c][0] * rhs(0, 2)
                + _elem[c][1] * rhs(1, 2)
                + _elem[c][2] * rhs(2, 2);

            _elem[c][0] = x;
            _elem[c][1] = y;
            _elem[c][2] = z;

            t[c] = _elem[3][0] * rhs(0, c)
                + _elem[3][1] * rhs(1, c)
                + _elem[3][2] * rhs(2, c);
        }
        _elem[3][0] = t[0] + rhs(3, 0);
        _elem[3][1] = t[1] + rhs(3, 1);
        _elem[3][2] = t[2] + rhs(3, 2);

        return *this;
    }

    // 3x3部分行列の転置
    void Matrix43::transpose33()
    {
        for(s32 c=0; c<3; ++c){
            lmath::swap(_elem[c][0], _elem[0][c]);
            lmath::swap(_elem[c][1], _elem[1][c]);
            lmath::swap(_elem[c][2], _elem[2][c]);
        }
    }

    // 3x3部分行列の行列式
    f32 Matrix43::determinant33() const
    {
        return _elem[0][0] * (_elem[1][1]*_elem[2][2] - _elem[1][2]*_elem[2][1])
             + _elem[0][1] * (_elem[1][2]*_elem[2][0] - _elem[1][0]*_elem[2][2])
             + _elem[0][2] * (_elem[1][0]*_elem[2][1] - _elem[1][1]*_elem[2][0]);
    }

    // 3x3部分行列の逆行列
    void Matrix43::invert33()
    {
        f32 det = determinant33();

        LASSERT(isEqual(det, 0.0f) != false);

        f32 invDet = 1.0f / det;

        Matrix43 ret;
        ret(0,0) = (_elem[1][1]*_elem[2][2] - _elem[1][2]*_elem[2][1]) * invDet;
        ret(0,1) = (_elem[0][2]*_elem[2][1] - _elem[0][1]*_elem[2][2]) * invDet;
        ret(0,2) = (_elem[0][1]*_elem[1][2] - _elem[0][2]*_elem[1][1]) * invDet;

        ret(1,0) = (_elem[1][2]*_elem[2][0] - _elem[1][0]*_elem[2][2]) * invDet;
        ret(1,1) = (_elem[0][0]*_elem[2][2] - _elem[0][2]*_elem[2][0]) * invDet;
        ret(1,2) = (_elem[0][2]*_elem[1][0] - _elem[0][0]*_elem[1][2]) * invDet;

        ret(2,0) = (_elem[1][0]*_elem[2][1] - _elem[1][1]*_elem[2][0]) * invDet;
        ret(2,1) = (_elem[0][1]*_elem[2][0] - _elem[0][0]*_elem[2][1]) * invDet;
        ret(2,2) = (_elem[0][0]*_elem[1][1] - _elem[0][1]*_elem[1][0]) * invDet;

        ret(3,0) = _elem[3][0];
        ret(3,1) = _elem[3][1];
        ret(3,2) = _elem[3][2];

        *this = ret;
    }

    // 逆行列
    void Matrix43::invert()
    {
        f32 det = determinant33();

        LASSERT(isEqual(det, 0.0f) == false);

        f32 invDet = 1.0f / det;

        Matrix43 ret;
        ret(0,0) = (_elem[1][1]*_elem[2][2] - _elem[1][2]*_elem[2][1]) * invDet;
        ret(0,1) = (_elem[0][2]*_elem[2][1] - _elem[0][1]*_elem[2][2]) * invDet;
        ret(0,2) = (_elem[0][1]*_elem[1][2] - _elem[0][2]*_elem[1][1]) * invDet;

        ret(1,0) = (_elem[1][2]*_elem[2][0] - _elem[1][0]*_elem[2][2]) * invDet;
        ret(1,1) = (_elem[0][0]*_elem[2][2] - _elem[0][2]*_elem[2][0]) * invDet;
        ret(1,2) = (_elem[0][2]*_elem[1][0] - _elem[0][0]*_elem[1][2]) * invDet;

        ret(2,0) = (_elem[1][0]*_elem[2][1] - _elem[1][1]*_elem[2][0]) * invDet;
        ret(2,1) = (_elem[0][1]*_elem[2][0] - _elem[0][0]*_elem[2][1]) * invDet;
        ret(2,2) = (_elem[0][0]*_elem[1][1] - _elem[0][1]*_elem[1][0]) * invDet;

        ret(3,0) = -(_elem[3][0]*ret(0,0) + _elem[3][1]*ret(1,0) + _elem[3][2]*ret(2,0));
        ret(3,1) = -(_elem[3][0]*ret(0,1) + _elem[3][1]*ret(1,1) + _elem[3][2]*ret(2,1));
        ret(3,2) = -(_elem[3][0]*ret(0,2) + _elem[3][1]*ret(1,2) + _elem[3][2]*ret(2,2));

        *this = ret;
    }


    void Matrix43::translate(const Vector3& v)
    {
        translate(v._x, v._y, v._z);
    }

    void Matrix43::translate(f32 x, f32 y, f32 z)
    {
        _elem[3][0] += x;
        _elem[3][1] += y;
        _elem[3][2] += z;
    }

    void Matrix43::preTranslate(const Vector3& v)
    {
        preTranslate(v._x, v._y, v._z);
    }


    void Matrix43::preTranslate(f32 x, f32 y, f32 z)
    {
        _elem[3][0] += _elem[0][0] * x + _elem[1][0] * y + _elem[2][0] * z;
        _elem[3][1] += _elem[0][1] * x + _elem[1][1] * y + _elem[2][1] * z;
        _elem[3][2] += _elem[0][2] * x + _elem[1][2] * y + _elem[2][2] * z;
    }


    void Matrix43::rotateX(f32 radian)
    {
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);

        Matrix43 rotation;
        rotation(0, 0) = 1.0f; rotation(0, 1) = 0.0f;  rotation(0, 2) = 0.0f;
        rotation(1, 0) = 0.0f; rotation(1, 1) = cosA;  rotation(1, 2) = sinA;
        rotation(2, 0) = 0.0f; rotation(2, 1) = -sinA; rotation(2, 2) = cosA;
        rotation(3, 0) = 0.0f; rotation(3, 1) = 0.0f;  rotation(3, 2) = 0.0f;

        *this *= rotation;
    }

    void Matrix43::rotateY(f32 radian)
    {
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);

        Matrix43 rotation;
        rotation(0, 0) = cosA; rotation(0, 1) = 0.0f;  rotation(0, 2) = -sinA;
        rotation(1, 0) = 0.0f; rotation(1, 1) = 1.0f;  rotation(1, 2) = 0.0f;
        rotation(2, 0) = sinA; rotation(2, 1) = 0.0f; rotation(2, 2) = cosA;
        rotation(3, 0) = 0.0f; rotation(3, 1) = 0.0f;  rotation(3, 2) = 0.0f;

        *this *= rotation;
    }

    void Matrix43::rotateZ(f32 radian)
    {
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);

        Matrix43 rotation;
        rotation(0, 0) = cosA; rotation(0, 1) = sinA;  rotation(0, 2) = 0.0f;
        rotation(1, 0) = -sinA; rotation(1, 1) = cosA;  rotation(1, 2) = 0.0f;
        rotation(2, 0) = 0.0f; rotation(2, 1) = 0.0f; rotation(2, 2) = 1.0f;
        rotation(3, 0) = 0.0f; rotation(3, 1) = 0.0f;  rotation(3, 2) = 0.0f;

        *this *= rotation;
    }

    void Matrix43::rotateAxis(f32 x, f32 y, f32 z, f32 radian)
    {
        f32 norm = lmath::sqrt(x*x + y*y + z*z);
        if(lmath::isEqual(norm, 0.0f)){
            return;
        }

        norm = 1.0f / norm;
        x *= norm;
        y *= norm;
        z *= norm;

        f32 xSqr = x*x;
        f32 ySqr = y*y;
        f32 zSqr = z*z;

        Matrix43 rotation;
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);
        f32 invCosA = 1.0f - cosA;

        rotation(0, 0) = (invCosA * xSqr) + cosA;
        rotation(0, 1) = (invCosA * x * y) - (sinA * z);
        rotation(0, 2) = (invCosA * x * z) + (sinA * y);

        rotation(1, 0) = (invCosA * x * y) + (sinA * z);
        rotation(1, 1) = (invCosA * ySqr) + (cosA);
        rotation(1, 2) = (invCosA * y * z) - (sinA * x);

        rotation(2, 0) = (invCosA * x * z) - (sinA * y);
        rotation(2, 1) = (invCosA * y * z) + (sinA * x);
        rotation(2, 2) = (invCosA * zSqr) + (cosA);

        rotation(3, 0) = 0.0f;
        rotation(3, 1) = 0.0f;
        rotation(3, 2) = 0.0f;

        *this *= rotation;
    }

    void Matrix43::mul(const Matrix43& m0, const Matrix43& m1)
    {
        Matrix43 tmp;
        for(s32 c=0; c<3; ++c){
            tmp(c, 0) = m0(c,0) * m1(0, 0)
                + m0(c, 1) * m1(1, 0)
                + m0(c, 2) * m1(2, 0);

            tmp(c, 1) = m0(c,0) * m1(0, 1)
                + m0(c, 1) * m1(1, 1)
                + m0(c, 2) * m1(2, 1);

            tmp(c, 2) = m0(c,0) * m1(0, 2)
                + m0(c, 1) * m1(1, 2)
                + m0(c, 2) * m1(2, 2);

            tmp(3, c) = m0(3, 0) * m1(0, c)
                + m0(3, 1) * m1(1, c)
                + m0(3, 2) * m1(2, c);
        }

        tmp(3, 0) += m1(3, 0);
        tmp(3, 1) += m1(3, 1);
        tmp(3, 2) += m1(3, 2);
        *this = tmp;
    }

    Matrix43& Matrix43::operator*=(const Matrix44& rhs)
    {
        f32 x, y, z;
        Vector3 t;
        for(s32 c=0; c<3; ++c){
            x = _elem[c][0] * rhs(0, 0)
                + _elem[c][1] * rhs(1, 0)
                + _elem[c][2] * rhs(2, 0);

            y = _elem[c][0] * rhs(0, 1)
                + _elem[c][1] * rhs(1, 1)
                + _elem[c][2] * rhs(2, 1);

            z = _elem[c][0] * rhs(0, 2)
                + _elem[c][1] * rhs(1, 2)
                + _elem[c][2] * rhs(2, 2);

            _elem[c][0] = x;
            _elem[c][1] = y;
            _elem[c][2] = z;

            t[c] = _elem[3][0] * rhs(0, c)
                + _elem[3][1] * rhs(1, c)
                + _elem[3][2] * rhs(2, c);
        }

        _elem[3][0] = t[0] + rhs(3, 0);
        _elem[3][1] = t[1] + rhs(3, 1);
        _elem[3][2] = t[2] + rhs(3, 2);

        return *this;
    }


    bool Matrix43::isNan() const
    {
        for(u32 i=0; i<4; ++i){
            for(u32 j=0; j<3; ++j){
                if(lcore::isNan(_elem[i][j])){
                    return true;
                }
            }
        }
        return false;
    }
}
