/**
@file lmathutil.cpp
@author t-sakai
@date 2010/09/09 create

*/
#include "lmathutil.h"
#include "Vector3.h"
#include "Vector4.h"

namespace lmath
{
#if defined(LMATH_USE_SSE)
namespace
{
    inline void store(Vector3& v, const lm128& r)
    {
        _mm_storel_pi(reinterpret_cast<__m64*>(&v.x_), r);

        static const u32 Shuffle = 170;
        lm128 t = _mm_shuffle_ps(r, r, Shuffle);
        _mm_store_ss(&v.z_, t);
    }
}
#endif

    void calcBBox(Vector3& bmin, Vector3& bmax, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
#if defined(LMATH_USE_SSE)
        lm128 x0 = Vector3::load(v0);
        lm128 x1 = Vector3::load(v1);
        lm128 x2 = Vector3::load(v2);

        lm128 retMin = _mm_min_ps(x0, x1);
        lm128 retMax = _mm_max_ps(x0, x1);

        retMin = _mm_min_ps(retMin, x2);
        retMax = _mm_max_ps(retMax, x2);

        store(bmin, retMin);
        store(bmax, retMax);

#else
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
#endif
    }


    void calcMedian(Vector3& median, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
#if defined(LMATH_USE_SSE)
        lm128 x0 = Vector3::load(v0);
        lm128 x1 = Vector3::load(v1);
        lm128 x2 = Vector3::load(v2);


        lm128 inv = _mm_set1_ps((1.0f/3.0f));

        lm128 mRet = _mm_add_ps(x0, x1);
        mRet = _mm_add_ps(mRet, x2);
        mRet = _mm_mul_ps(mRet, inv);

        store(median, mRet);
#else
        median = v0;
        median += v1;
        median += v2;
        median /= 3.0f;
#endif
    }

    void calcAABBPoints(Vector4* AABB, const Vector4& aabbMin, const Vector4& aabbMax)
    {
        AABB[0].set(aabbMax.x_, aabbMax.y_, aabbMin.z_, 1.0f);
        AABB[1].set(aabbMin.x_, aabbMax.y_, aabbMin.z_, 1.0f);
        AABB[2].set(aabbMax.x_, aabbMin.y_, aabbMin.z_, 1.0f);
        AABB[3].set(aabbMin.x_, aabbMin.y_, aabbMin.z_, 1.0f);

        AABB[4].set(aabbMax.x_, aabbMax.y_, aabbMax.z_, 1.0f);
        AABB[5].set(aabbMin.x_, aabbMax.y_, aabbMax.z_, 1.0f);
        AABB[6].set(aabbMax.x_, aabbMin.y_, aabbMax.z_, 1.0f);
        AABB[7].set(aabbMin.x_, aabbMin.y_, aabbMax.z_, 1.0f);
    }
}
