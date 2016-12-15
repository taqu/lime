#ifndef INC_LMATH_MATRIX_H__
#define INC_LMATH_MATRIX_H__
/**
@file Matrix.h
@author t-sakai
@date 2016/03/22 create
*/
#include "lmath.h"

namespace lmath
{
    class Matrix
    {
    public:
        Matrix();
        Matrix(s32 r, s32 c);
        ~Matrix();

        inline s32 getRows() const;
        inline s32 getCols() const;
        inline f32* operator[](s32 r);
        inline const f32* operator[](s32 r) const;

        void swap(Matrix& rhs);
        static void mul(Matrix& dst, const Matrix& m0, const Matrix& m1);

        void print();
    private:
       Matrix(const Matrix&);
       Matrix& operator=(const Matrix&);

       s32 rows_;
       s32 cols_;
       f32* data_;
    };

    inline s32 Matrix::getRows() const
    {
        return rows_;
    }

    inline s32 Matrix::getCols() const
    {
        return cols_;
    }

    inline f32* Matrix::operator[](s32 r)
    {
        LASSERT(0<=r && r<rows_);
        return data_ + r*cols_;
    }

    inline const f32* Matrix::operator[](s32 r) const
    {
        LASSERT(0<=r && r<rows_);
        return data_ + r*cols_;
    }
    
    template<class T>
    void randomMatrix(Matrix& m, T& rand)
    {
        for(s32 i=0; i<m.getRows(); ++i){
            for(s32 j=0; j<m.getCols(); ++j){
                m[i][j] = rand.frand2();
            }
        }
    }
}
#endif //INC_LMATH_MATRIX_H__
