#ifndef INC_LMATH_MATRIX44_H__
#define INC_LMATH_MATRIX44_H__
/**
@file Matrix44.h
@author t-sakai
@date 2009/01/18 create
*/
#include "lmathcore.h"
#include "Vector3.h"

namespace lmath
{
    class Matrix43;

    //--------------------------------------------
    //---
    //--- Matrix44
    //---
    //--------------------------------------------
    class Matrix44
    {
    public:
        Matrix44()
        {}

        Matrix44(const Matrix43& mat);

        ~Matrix44()
        {}

        inline f32 operator()(s32 c, s32 r) const;
        inline f32& operator()(s32 c, s32 r);

        Matrix44& operator*=(f32 f);
        Matrix44& operator*=(const Matrix44& m);
        Matrix44& operator+=(const Matrix44& m);
        Matrix44& operator-=(const Matrix44& m);
        Matrix44& operator=(const Matrix44& rhs);
        Matrix44& operator=(const Matrix43& rhs);

        Matrix44 operator-() const;

        void identity();
        void transpose();
        f32 determinant() const;
        void invert();

        /// 平行移動セット
        inline void setTranslate(const Vector3& v);

        /// 平行移動セット
        inline void setTranslate(f32 x, f32 y, f32 z);

        /// 後から平行移動
        void translate(const Vector3& v);

        /// 後から平行移動
        void translate(f32 x, f32 y, f32 z);

        /// 前から平行移動
        void preTranslate(const Vector3& v);

        /// 前から平行移動
        void preTranslate(f32 x, f32 y, f32 z);

        void rotateX(f32 radian);
        void rotateY(f32 radian);
        void rotateZ(f32 radian);
        void rotateAxis(const Vector3& axis, f32 radian)
        {
            rotateAxis(axis._x, axis._y, axis._z, radian);
        }

        void rotateAxis(f32 x, f32 y, f32 z, f32 radian);

        void lookAt(const Vector3& eye, const Vector3& at, const Vector3& up);

        void perspective(f32 width, f32 height, f32 znear, f32 zfar);
        void perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar);
        void ortho(f32 width, f32 height, f32 znear, f32 zfar);

        void getTranslate(Vector3& trans) const
        {
            trans.set(_elem[3][0], _elem[3][1], _elem[3][2]);
        }

        bool isNan() const;

        f32 _elem[4][4];
    };

    inline f32 Matrix44::operator()(s32 c, s32 r) const
    {
        LASSERT(0<= c
            && c < 4);
        LASSERT(0<= r
            && r < 4);
        return _elem[c][r];
    }

    inline f32& Matrix44::operator()(s32 c, s32 r)
    {
        LASSERT(0<= c
            && c < 4);
        LASSERT(0<= r
            && r < 4);
        return _elem[c][r];
    }

    // 平行移動セット
    inline void Matrix44::setTranslate(const Vector3& v)
    {
        _elem[3][0] = v._x;
        _elem[3][1] = v._y;
        _elem[3][2] = v._z;

    }

    // 平行移動セット
    inline void Matrix44::setTranslate(f32 x, f32 y, f32 z)
    {
        _elem[3][0] = x;
        _elem[3][1] = y;
        _elem[3][2] = z;
    }
}

#endif //INC_LMATH_MATRIX44_H__
