#ifndef INC_LMATH_MATRIX43_H__
#define INC_LMATH_MATRIX43_H__
/**
@file Matrix43.h
@author t-sakai
@date 2009/01/18 create
*/
#include "lmathcore.h"
#include "Vector3.h"

namespace lmath
{

    //--------------------------------------------
    //---
    //--- Matrix43
    //---
    //--------------------------------------------
    class Matrix43
    {
    public:
        Matrix43()
        {}

        Matrix43(f32 m00, f32 m01, f32 m02,
            f32 m10, f32 m11, f32 m12,
            f32 m20, f32 m21, f32 m22,
            f32 m30, f32 m31, f32 m32)
        {
            _elem[0][0] = m00; _elem[0][1] = m01; _elem[0][2] = m02;
            _elem[1][0] = m10; _elem[1][1] = m11; _elem[1][2] = m12;
            _elem[2][0] = m20; _elem[2][1] = m21; _elem[2][2] = m22;
            _elem[3][0] = m30; _elem[3][1] = m31; _elem[3][2] = m32;
        }

        ~Matrix43()
        {}

        /// 値セット
        void set(f32 m00, f32 m01, f32 m02,
            f32 m10, f32 m11, f32 m12,
            f32 m20, f32 m21, f32 m22,
            f32 m30, f32 m31, f32 m32)
        {
            _elem[0][0] = m00; _elem[0][1] = m01; _elem[0][2] = m02;
            _elem[1][0] = m10; _elem[1][1] = m11; _elem[1][2] = m12;
            _elem[2][0] = m20; _elem[2][1] = m21; _elem[2][2] = m22;
            _elem[3][0] = m30; _elem[3][1] = m31; _elem[3][2] = m32;
        }

        /// 要素アクセス
        inline f32 operator()(s32 c, s32 r) const;

        /// 要素アクセス
        inline f32& operator()(s32 c, s32 r);

        Matrix43& operator*=(f32 f);
        Matrix43& operator*=(const Matrix43& rhs);
        Matrix43& operator+=(const Matrix43& rhs);
        Matrix43& operator-=(const Matrix43& rhs);
        Matrix43& operator=(const Matrix43& rhs);

        Matrix43 operator-() const;

        void identity();

        /// 3x3部分行列の転置
        void transpose33();

        /// 3x3部分行列の行列式
        f32 determinant33() const;

        /// 3x3部分行列の逆行列
        void invert33();

        /// 逆行列
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

        /// 後から回転
        void rotateX(f32 radian);

        /// 後から回転
        void rotateY(f32 radian);

        /// 後から回転
        void rotateZ(f32 radian);

        /// 後から回転
        void rotateAxis(const Vector3& axis, f32 radian)
        {
            rotateAxis(axis._x, axis._y, axis._z, radian);
        }

        /// 後から回転
        void rotateAxis(f32 x, f32 y, f32 z, f32 radian);

        inline void setScale(f32 s);
        inline void scale(f32 s);

        void mul(const Matrix43& m0, const Matrix43& m1);

        Matrix43& operator*=(const Matrix44& rhs);

        inline void getRow4(lmath::Vector3& v) const;

        bool isNan() const;

        f32 _elem[4][3];
    };

    inline f32 Matrix43::operator()(s32 c, s32 r) const
    {
        LASSERT(0<= c
            && c < 4);
        LASSERT(0<= r
            && r < 3);
        return _elem[c][r];
    }

    inline f32& Matrix43::operator()(s32 c, s32 r)
    {
        LASSERT(0<= c
            && c < 4);
        LASSERT(0<= r
            && r < 3);
        return _elem[c][r];
    }

    // 平行移動セット
    inline void Matrix43::setTranslate(const Vector3& v)
    {
        _elem[3][0] = v._x;
        _elem[3][1] = v._y;
        _elem[3][2] = v._z;

    }

    // 平行移動セット
    inline void Matrix43::setTranslate(f32 x, f32 y, f32 z)
    {
        _elem[3][0] = x;
        _elem[3][1] = y;
        _elem[3][2] = z;
    }

    inline void Matrix43::setScale(f32 s)
    {
        _elem[0][0] = s;
        _elem[1][1] = s;
        _elem[2][2] = s;
    }

    inline void Matrix43::scale(f32 s)
    {
        _elem[0][0] *= s;
        _elem[1][1] *= s;
        _elem[2][2] *= s;
    }

    inline void Matrix43::getRow4(lmath::Vector3& v) const
    {
        v.set(_elem[3][0], _elem[3][1], _elem[3][2]);
    }

}
#endif //INC_LMATH_MATRIX43_H__
