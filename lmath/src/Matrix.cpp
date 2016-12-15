/**
@file Matrix.cpp
@author t-sakai
@date 2016/03/22 create
*/
#include "Matrix.h"

#ifdef _DEBUG
#include <stdio.h>
#endif

namespace lmath
{
    Matrix::Matrix()
        :rows_(0)
        ,cols_(0)
        ,data_(NULL)
    {
    }

    Matrix::Matrix(s32 r, s32 c)
        :rows_(r)
        ,cols_(c)
        ,data_(NULL)
    {
        LASSERT(0 <= rows_);
        LASSERT(0 <= cols_);
        data_ = LNEW f32[rows_*cols_];
    }

    Matrix::~Matrix()
    {
        LDELETE_ARRAY(data_);
    }

    void Matrix::swap(Matrix& rhs)
    {
        lcore::swap(rows_, rhs.rows_);
        lcore::swap(cols_, rhs.cols_);
        lcore::swap(data_, rhs.data_);
    }

    void Matrix::mul(Matrix& dst, const Matrix& m0, const Matrix& m1)
    {
        LASSERT(dst.rows_ == m0.rows_);
        LASSERT(dst.cols_ == m1.cols_);
        LASSERT(m0.cols_ == m1.rows_);

        for(s32 i=0; i<m0.rows_; ++i){
            for(s32 j=0; j<m1.cols_; ++j){
                f32 t = 0.0f;
                for(s32 k=0; k<m1.rows_; ++k){
                    t += m0[i][k] * m1[k][j];
                }//for(s32 k
                dst[i][j] = t;
            }//for(s32 j
        }//for(s32 i
    }

    void Matrix::print()
    {
#ifdef _DEBUG
        printf("[");
        for(s32 i=0; i<rows_; ++i){
            for(s32 j=0; j<cols_; ++j){
                printf("%f,", (*this)[i][j]);
            }
            if(i != (rows_-1)){
                printf("\n");
            }
        }
        printf("]\n");
#endif
    }
}
