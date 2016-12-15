/**
@file NMF.cpp
@author t-sakai
@date 2016/03/22 create
*/
#include "NMF.h"
#include "lcore/Random.h"

#ifdef _DEBUG
#include <stdio.h>
#endif

namespace lmath
{
    NMF::NMF()
        :seed_(123)
    {}

    NMF::NMF(s32 r, s32 c, u32 seed)
        :tmp_(r, c)
        ,seed_(seed)
    {
    }

    NMF::~NMF()
    {}

    void NMF::run(Matrix& W, Matrix& H, const Matrix& V, s32 numIterations)
    {
        LASSERT(V.getRows() == W.getRows());
        LASSERT(V.getCols() == H.getCols());
        LASSERT(W.getCols() == H.getRows());
        LASSERT(tmp_.getRows() == V.getRows());
        LASSERT(tmp_.getCols() == V.getCols());

        static const f32 epsilon = 1.0e-6f;
        lcore::RandXorshift random(seed_);
        randomMatrix(W, random);
        randomMatrix(H, random);

        Matrix::mul(tmp_, W, H);

        s32 r = W.getCols();
        for(s32 itr=0; itr<numIterations; ++itr){
            for(s32 i=0; i<V.getRows(); ++i){
                for(s32 j=0; j<r; ++j){
                    f32 sum = 0.0f;
                    for(s32 k=0; k<V.getCols(); ++k){
                        f32 t = tmp_[i][k];
                        if(epsilon<t){
                            sum += H[j][k] * (V[i][k]/t);
                        }
                    }
                    W[i][j] = sum*W[i][j];
                }//for(s32 j
            }//for(s32 i
            //W.print();

            for(s32 i=0; i<r; ++i){
                for(s32 j=0; j<V.getCols(); ++j){
                    f32 sum = 0.0f;
                    for(s32 k=0; k<V.getRows(); ++k){
                        f32 t = tmp_[k][j];
                        if(epsilon<t){
                            sum += W[k][i] * (V[k][j] / t);
                        }
                    }
                    H[i][j] = sum*H[i][j];
                }//for(s32 j
            }//for(s32 i
            //H.print();

            for(s32 i=0; i<r; ++i){
                f32 sum = 0.0f;
                for(s32 j=0; j<V.getCols(); ++j){
                    sum += H[i][j];
                }
                if(epsilon<sum){
                    f32 inv = 1.0f / sum;
                    for(s32 j = 0; j < V.getCols(); ++j){
                        H[i][j] = H[i][j] * inv;
                    }
                }
            }//for(s32 i
            Matrix::mul(tmp_, W, H);

#if _DEBUG
            f32 error = 0.0f;
            for(s32 i=0; i<V.getRows(); ++i){
                for(s32 j=0; j<V.getCols(); ++j){
                    f32 t = tmp_[i][j];
                    if(t<=epsilon){
                        continue;
                    }
                    f32 l = (epsilon<V[i][j])? logf(V[i][j]/t) : 0.0f;
                    error += V[i][j] * l
                        - V[i][j]
                        + tmp_[i][j];
                }
            }
            printf("error %f\n", error);
#endif
        }//for(s32 itr
    }

    void NMF::swap(NMF& rhs)
    {
        tmp_.swap(rhs.tmp_);
        lcore::swap(seed_, rhs.seed_);
    }

}
