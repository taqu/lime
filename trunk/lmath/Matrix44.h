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
    class Matrix34;

    //--------------------------------------------
    //---
    //--- Matrix44
    //---
    //--------------------------------------------
    /**
    右手座標系、列ベクトル、ベクトルに対して左から掛けて変換する。
    */
    class Matrix44
    {
    public:
        Matrix44()
        {}

        explicit Matrix44(const Matrix34& mat);

        ~Matrix44()
        {}

        inline f32 operator()(s32 r, s32 c) const;
        inline f32& operator()(s32 r, s32 c);

        Matrix44& operator*=(f32 f);
        Matrix44& operator*=(const Matrix44& rhs);
        Matrix44& operator+=(const Matrix44& rhs);
        Matrix44& operator-=(const Matrix44& rhs);
        Matrix44& operator=(const Matrix44& rhs);

        Matrix44& operator*=(const Matrix34& rhs);
        Matrix44& operator=(const Matrix34& rhs);

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
        inline void preTranslate(const Vector3& v);

        /// 前から平行移動
        void preTranslate(f32 x, f32 y, f32 z);

        /// 後から回転
        void rotateX(f32 radian);

        /// 後から回転
        void rotateY(f32 radian);

        /// 後から回転
        void rotateZ(f32 radian);

        /// 軸回転
        void setRotateAxis(const Vector3& axis, f32 radian)
        {
            setRotateAxis(axis.x_, axis.y_, axis.z_, radian);
        }

        /// 軸回転
        void setRotateAxis(f32 x, f32 y, f32 z, f32 radian);

        inline void setScale(f32 s);
        inline void scale(f32 s);

        void lookAt(const Vector3& eye, const Vector3& at, const Vector3& up);

        void perspective(f32 width, f32 height, f32 znear, f32 zfar);
        void perspectiveFov(f32 fovy, f32 aspect, f32 znear, f32 zfar);
        void ortho(f32 width, f32 height, f32 znear, f32 zfar);

        void perspectiveLinearZ(f32 width, f32 height, f32 znear, f32 zfar);
        void perspectiveFovLinearZ(f32 fovy, f32 aspect, f32 znear, f32 zfar);

        void getTranslate(Vector3& trans) const
        {
            trans.set(m_[0][3], m_[1][3], m_[2][3]);
        }

        bool isNan() const;

        f32 m_[4][4];
    };

    inline f32 Matrix44::operator()(s32 r, s32 c) const
    {
        LASSERT(0<= r
            && r < 4);
        LASSERT(0<= c
            && c < 4);
        return m_[r][c];
    }

    inline f32& Matrix44::operator()(s32 r, s32 c)
    {
        LASSERT(0<= r
            && r < 4);
        LASSERT(0<= c
            && c < 4);
        return m_[r][c];
    }

    // 平行移動セット
    inline void Matrix44::setTranslate(const Vector3& v)
    {
        m_[0][3] = v.x_;
        m_[1][3] = v.y_;
        m_[2][3] = v.z_;

    }

    // 平行移動セット
    inline void Matrix44::setTranslate(f32 x, f32 y, f32 z)
    {
        m_[0][3] = x;
        m_[1][3] = y;
        m_[2][3] = z;
    }

    // 前から平行移動
    inline void Matrix44::preTranslate(const Vector3& v)
    {
        preTranslate(v.x_, v.y_, v.z_);
    }

    inline void Matrix44::setScale(f32 s)
    {
        m_[0][0] = s;
        m_[1][1] = s;
        m_[2][2] = s;
    }

    inline void Matrix44::scale(f32 s)
    {
        m_[0][0] *= s;
        m_[1][1] *= s;
        m_[2][2] *= s;
    }
}

#endif //INC_LMATH_MATRIX44_H__
