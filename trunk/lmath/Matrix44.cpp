/**
@file Matrix44.cpp
@author t-sakai
@date 2009/01/18 create
*/
#include "Matrix44.h"
#include "Matrix43.h"

namespace lmath
{
    //--------------------------------------------
    //---
    //--- Matrix44
    //---
    //--------------------------------------------
    Matrix44::Matrix44(const Matrix43& mat)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<3; ++j){
                _elem[i][j] = mat._elem[i][j];
            }
        }
        _elem[0][3] = _elem[1][3] = _elem[2][3] = 0.0f;
        _elem[3][3] = 1.0f;
    }

    Matrix44& Matrix44::operator=(const Matrix44& rhs)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<4; ++j){
                _elem[i][j] = rhs._elem[i][j];
            }
        }
        return *this;
    }

    Matrix44& Matrix44::operator=(const Matrix43& rhs)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<3; ++j){
                _elem[i][j] = rhs._elem[i][j];
            }
        }
        _elem[0][3] = _elem[1][3] = _elem[2][3] = 0.0f;
        _elem[3][3] = 1.0f;
        return *this;
    }

    Matrix44& Matrix44::operator*=(f32 f)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<4; ++j){
                _elem[i][j] *= f;
            }
        }
        return *this;
    }

    Matrix44& Matrix44::operator+=(const Matrix44& m)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<4; ++j){
                _elem[i][j] += m(i, j);
            }
        }
        return *this;
    }

    Matrix44& Matrix44::operator-=(const Matrix44& m)
    {
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<4; ++j){
                _elem[i][j] -= m(i, j);
            }
        }
        return *this;
    }

    Matrix44 Matrix44::operator-() const
    {
        Matrix44 ret;
        for(s32 i=0; i<4; ++i){
            for(s32 j=0; j<4; ++j){
                ret(i, j) = -_elem[i][j];
            }
        }
        return ret;
    }

    void Matrix44::identity()
    {
        _elem[0][0] = 1.0f; _elem[0][1] = 0.0f; _elem[0][2] = 0.0f; _elem[0][3] = 0.0f;
        _elem[1][0] = 0.0f; _elem[1][1] = 1.0f; _elem[1][2] = 0.0f; _elem[1][3] = 0.0f;
        _elem[2][0] = 0.0f; _elem[2][1] = 0.0f; _elem[2][2] = 1.0f; _elem[2][3] = 0.0f;
        _elem[3][0] = 0.0f; _elem[3][1] = 0.0f; _elem[3][2] = 0.0f; _elem[3][3] = 1.0f;
    }

    Matrix44& Matrix44::operator*=(const Matrix44& m)
    {
        f32 x, y, z, w;
        for(s32 c=0; c<4; ++c){
            x = _elem[c][0] * m(0, 0)
                + _elem[c][1] * m(1, 0)
                + _elem[c][2] * m(2, 0)
                + _elem[c][3] * m(3, 0);

            y = _elem[c][0] * m(0, 1)
                + _elem[c][1] * m(1, 1)
                + _elem[c][2] * m(2, 1)
                + _elem[c][3] * m(3, 1);

            z = _elem[c][0] * m(0, 2)
                + _elem[c][1] * m(1, 2)
                + _elem[c][2] * m(2, 2)
                + _elem[c][3] * m(3, 2);

            w = _elem[c][0] * m(0, 3)
                + _elem[c][1] * m(1, 3)
                + _elem[c][2] * m(2, 3)
                + _elem[c][3] * m(3, 3);

            _elem[c][0] = x;
            _elem[c][1] = y;
            _elem[c][2] = z;
            _elem[c][3] = w;
        }
        return *this;
    }

    void Matrix44::transpose()
    {
        for(s32 c=0; c<4; ++c){
            lmath::swap(_elem[c][0], _elem[0][c]);
            lmath::swap(_elem[c][1], _elem[1][c]);
            lmath::swap(_elem[c][2], _elem[2][c]);
            lmath::swap(_elem[c][3], _elem[3][c]);
        }
    }

    f32 Matrix44::determinant() const
    {
        return _elem[0][0]*(_elem[1][1]*_elem[2][2]*_elem[3][3] - _elem[1][3]*_elem[2][2]*_elem[3][1])
            + _elem[0][1]*(_elem[1][2]*_elem[2][3]*_elem[3][0] - _elem[1][0]*_elem[2][3]*_elem[3][2])
            + _elem[0][2]*(_elem[1][3]*_elem[2][0]*_elem[3][1] - _elem[3][1]*_elem[2][0]*_elem[3][3])
            + _elem[0][3]*(_elem[1][0]*_elem[2][1]*_elem[3][2] - _elem[1][2]*_elem[2][1]*_elem[3][0]);
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
                if( lmath::absolute(tmp(j,i)) > lmath::absolute(tmp(j,rowMax)) ){
                    rowMax = i;
                }
            }

            // If the max value here is 0, we can't invert.
            if( lmath::isEqual(tmp(rowMax,j), 0.0f) ){
                return;
            }

            // Swap row "rowMax" with row "j"
            for(s32 cc=0; cc<4; ++cc){
                lmath::swap(tmp(cc, j), tmp(cc, rowMax));
                lmath::swap(tmp2(cc, j), tmp2(cc, rowMax));
            }

            // Now everything we do is on row "c".
            // Set the max cell to 1 by dividing the entire row by that value
            f32 pivot = tmp(j, j);
            for(s32 cc=0; cc<4; ++cc){
                tmp(cc, j) /= pivot;
                tmp2(cc, j) /= pivot;
            }

            // Now do the other rows, so that this column only has a 1 and 0's
            for(s32 i=0; i<4; ++i){
                if(i != j){
                    pivot = tmp(j, i);
                    for(s32 cc=0; cc<4; cc++)
                    {
                        tmp(cc, i) -= tmp(cc, j) * pivot;
                        tmp2(cc, i) -= tmp2(cc, j) * pivot;
                    }
                }
            }

        }

        (*this) = tmp2;
    }

    void Matrix44::translate(const Vector3& vector)
    {
        _elem[3][0] += vector[0];
        _elem[3][1] += vector[1];
        _elem[3][2] += vector[2];
    }

    void Matrix44::translate(f32 x, f32 y, f32 z)
    {
        _elem[3][0] += x;
        _elem[3][1] += y;
        _elem[3][2] += z;
    }

    void Matrix44::preTranslate(const Vector3& v)
    {
        preTranslate(v._x, v._y, v._z);
    }


    void Matrix44::preTranslate(f32 x, f32 y, f32 z)
    {
        _elem[3][0] += _elem[0][0] * x + _elem[1][0] * y + _elem[2][0] * z;
        _elem[3][1] += _elem[0][1] * x + _elem[1][1] * y + _elem[2][1] * z;
        _elem[3][2] += _elem[0][2] * x + _elem[1][2] * y + _elem[2][2] * z;
    }


    void Matrix44::rotateX(f32 radian)
    {
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);

        Matrix44 rotation;
        rotation(0, 0) = 1.0f; rotation(0, 1) = 0.0f;  rotation(0, 2) = 0.0f; rotation(0, 3) = 0.0f;
        rotation(1, 0) = 0.0f; rotation(1, 1) = cosA;  rotation(1, 2) = sinA; rotation(1, 3) = 0.0f;
        rotation(2, 0) = 0.0f; rotation(2, 1) = -sinA; rotation(2, 2) = cosA; rotation(2, 3) = 0.0f;
        rotation(3, 0) = 0.0f; rotation(3, 1) = 0.0f;  rotation(3, 2) = 0.0f; rotation(3, 3) = 1.0f;

        *this *= rotation;
    }

    void Matrix44::rotateY(f32 radian)
    {
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);

        Matrix44 rotation;
        rotation(0, 0) = cosA; rotation(0, 1) = 0.0f;  rotation(0, 2) = -sinA; rotation(0, 3) = 0.0f;
        rotation(1, 0) = 0.0f; rotation(1, 1) = 1.0f;  rotation(1, 2) = 0.0f; rotation(1, 3) = 0.0f;
        rotation(2, 0) = sinA; rotation(2, 1) = 0.0f; rotation(2, 2) = cosA; rotation(2, 3) = 0.0f;
        rotation(3, 0) = 0.0f; rotation(3, 1) = 0.0f;  rotation(3, 2) = 0.0f; rotation(3, 3) = 1.0f;

        *this *= rotation;
    }

    void Matrix44::rotateZ(f32 radian)
    {
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);

        Matrix44 rotation;
        rotation(0, 0) = cosA; rotation(0, 1) = sinA;  rotation(0, 2) = 0.0f; rotation(0, 3) = 0.0f;
        rotation(1, 0) = -sinA; rotation(1, 1) = cosA;  rotation(1, 2) = 0.0f; rotation(1, 3) = 0.0f;
        rotation(2, 0) = 0.0f; rotation(2, 1) = 0.0f; rotation(2, 2) = 1.0f; rotation(2, 3) = 0.0f;
        rotation(3, 0) = 0.0f; rotation(3, 1) = 0.0f;  rotation(3, 2) = 0.0f; rotation(3, 3) = 1.0f;

        *this *= rotation;
    }

    void Matrix44::rotateAxis(f32 x, f32 y, f32 z, f32 radian)
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

        Matrix44 rotation;
        f32 cosA = lmath::cosf(radian);
        f32 sinA = lmath::sinf(radian);
        f32 invCosA = 1.0f - cosA;

        rotation(0, 0) = (invCosA * xSqr) + cosA;
        rotation(0, 1) = (invCosA * x * y) - (sinA * z);
        rotation(0, 2) = (invCosA * x * z) + (sinA * y);
        rotation(0, 3) = 0.0f;

        rotation(1, 0) = (invCosA * x * y) + (sinA * z);
        rotation(1, 1) = (invCosA * ySqr) + (cosA);
        rotation(1, 2) = (invCosA * y * z) - (sinA * x);
        rotation(1, 3) = 0.0f;

        rotation(2, 0) = (invCosA * x * z) - (sinA * y);
        rotation(2, 1) = (invCosA * y * z) + (sinA * x);
        rotation(2, 2) = (invCosA * zSqr) + (cosA);
        rotation(2, 3) = 0.0f;

        rotation(3, 0) = 0.0f;
        rotation(3, 1) = 0.0f;
        rotation(3, 2) = 0.0f;
        rotation(3, 3) = 1.0f;

        *this *= rotation;
    }

    void Matrix44::lookAt(const Vector3& eye, const Vector3& at, const Vector3& up)
    {
        Vector3 xaxis, yaxis, zaxis = at;
        zaxis -= eye;
        zaxis.normalize();
        
        xaxis.cross(up, zaxis);
        xaxis.normalize();

        yaxis.cross(zaxis, xaxis);

        _elem[0][0] = xaxis._x; _elem[0][1] = yaxis._x; _elem[0][2] = zaxis._x; _elem[0][3] = 0.0f;
        _elem[1][0] = xaxis._y; _elem[1][1] = yaxis._y; _elem[1][2] = zaxis._y; _elem[1][3] = 0.0f;
        _elem[2][0] = xaxis._z; _elem[2][1] = yaxis._z; _elem[2][2] = zaxis._z; _elem[2][3] = 0.0f;
        _elem[3][0] = -eye.dot(xaxis); _elem[3][1] = -eye.dot(yaxis); _elem[3][2] = -eye.dot(zaxis); _elem[3][3] = 1.0f;
    }

    void Matrix44::perspective(f32 width, f32 height, f32 znear, f32 zfar)
    {
        _elem[0][0] = 2.0f * znear/width; _elem[0][1] = 0.0f; _elem[0][2] = 0.0f; _elem[0][3] = 0.0f;
        _elem[1][0] = 0.0f; _elem[1][1] = 2.0f * znear/height; _elem[1][2] = 0.0f; _elem[1][3] = 0.0f;
        _elem[2][0] = 0.0f; _elem[2][1] = 0.0f; _elem[2][2] = zfar / (zfar - znear); _elem[2][3] = 1.0f;
        _elem[3][0] = 0.0f; _elem[3][1] = 0.0f; _elem[3][2] = znear * zfar / (znear - zfar); _elem[3][3] = 0.0f;
    }

    void Matrix44::perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar)
    {
        f32 yscale = 1.0f / lmath::tan(0.5f * fovy);
        f32 xscale = yscale / aspect;

        _elem[0][0] = xscale; _elem[0][1] = 0.0f; _elem[0][2] = 0.0f; _elem[0][3] = 0.0f;
        _elem[1][0] = 0.0f; _elem[1][1] = yscale; _elem[1][2] = 0.0f; _elem[1][3] = 0.0f;
        _elem[2][0] = 0.0f; _elem[2][1] = 0.0f; _elem[2][2] = zfar / (zfar - znear); _elem[2][3] = 1.0f;
        _elem[3][0] = 0.0f; _elem[3][1] = 0.0f; _elem[3][2] = znear * zfar / (znear - zfar); _elem[3][3] = 0.0f;
    }

    void Matrix44::ortho(f32 width, f32 height, f32 znear, f32 zfar)
    {
        _elem[0][0] = 2.0f / width; _elem[0][1] = 0.0f; _elem[0][2] = 0.0f; _elem[0][3] = 0.0f;
        _elem[1][0] = 0.0f; _elem[1][1] = 2.0f / height; _elem[1][2] = 0.0f; _elem[1][3] = 0.0f;
        _elem[2][0] = 0.0f; _elem[2][1] = 0.0f; _elem[2][2] = 1.0f / (zfar - znear); _elem[2][3] = 0.0f;
        _elem[3][0] = 0.0f; _elem[3][1] = 0.0f; _elem[3][2] = znear / (znear - zfar); _elem[3][3] = 1.0f;
    }

    bool Matrix44::isNan() const
    {
        for(u32 i=0; i<4; ++i){
            for(u32 j=0; j<4; ++j){
                if(lcore::isNan(_elem[i][j])){
                    return true;
                }
            }
        }
        return false;
    }
}
