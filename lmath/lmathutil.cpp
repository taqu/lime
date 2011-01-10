/**
@file lmathutil.cpp
@author t-sakai
@date 2010/09/09 create

*/
#include "lmathutil.h"
#include "Vector3.h"

namespace lmath
{
    void calcBBox_SSE(Vector3& bmin, Vector3& bmax, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        __m128 x0 = _mm_set_ps(v0._x, v0._y, v0._z, 0.0f);
        __m128 x1 = _mm_set_ps(v1._x, v1._y, v1._z, 0.0f);
        __m128 x2 = _mm_set_ps(v2._x, v2._y, v2._z, 0.0f);

        __m128 retMin = _mm_min_ps(x0, x1);
        __m128 retMax = _mm_max_ps(x0, x1);

        retMin = _mm_min_ps(retMin, x2);
        retMax = _mm_max_ps(retMax, x2);

        LIME_ALIGN16 f32 ret[4];
        _mm_store_ps(ret, retMin);
        bmin.set(ret[3], ret[2], ret[1]);

        _mm_store_ps(ret, retMax);
        bmax.set(ret[3], ret[2], ret[1]);
    }

    void calcBBox_NoSSE(Vector3& bmin, Vector3& bmax, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        bmin = v0;
        bmax = v0;

        const Vector3 *pv = &v1;
        for(int i=0; i<2; ++i){
            for(int j=0; j<3; ++j){
                if(bmin[j] > (*pv)[j]){
                    bmin[j] = (*pv)[j];
                }

                if(bmax[j] < (*pv)[j]){
                    bmax[j] = (*pv)[j];
                }
            }
            pv = &v2;
        }

    }


    void calcMedian_SSE(Vector3& median, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        const f32 invNum = (1.0f/3.0f);

        __m128 x0 = _mm_set_ps(v0._x, v0._y, v0._z, 0.0f);
        __m128 x1 = _mm_set_ps(v1._x, v1._y, v1._z, 0.0f);
        __m128 x2 = _mm_set_ps(v2._x, v2._y, v2._z, 0.0f);
        __m128 inv = _mm_set_ps1(invNum);

        __m128 mRet = _mm_add_ps(x0, x1);
        mRet = _mm_add_ps(mRet, x2);
        mRet = _mm_mul_ps(mRet, inv);


        LIME_ALIGN16 f32 ret[4];
        _mm_store_ps(ret, mRet);
        median.set(ret[3], ret[2], ret[1]);
    }

    void calcMedian_NoSSE(Vector3& median, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
        median = v0;
        median += v1;
        median += v2;
        median /= 3.0f;
    }

}
