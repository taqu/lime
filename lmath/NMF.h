#ifndef INC_LMATH_NMF_H__
#define INC_LMATH_NMF_H__
/**
@file NMF.h
@author t-sakai
@date 2016/03/22 create
*/
#include "lmath.h"
#include "Matrix.h"

namespace lmath
{
    /**
    Non-Negative Matrix Factorization
    */
    class NMF
    {
    public:
        NMF();
        NMF(s32 r, s32 c, u32 seed);
        ~NMF();

        void run(Matrix& W, Matrix& H, const Matrix& V, s32 numIterations);
        void swap(NMF& rhs);
    private:
        NMF(const NMF&);
        NMF& operator=(const NMF&);

        Matrix tmp_;
        u32 seed_;
    };
}
#endif //INC_LMATH_NMF_H__
