#ifndef INC_LMATH_LMATHUTIL_H__
#define INC_LMATH_LMATHUTIL_H__
/**
@file lmathutil.h
@author t-sakai
@date 2010/09/09 create

*/
#include "lmathcore.h"

namespace lmath
{
    class Vector3;

    void calcBBox_SSE(Vector3& bmin, Vector3& bmax, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    void calcBBox_NoSIMD(Vector3& bmin, Vector3& bmax, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    void calcMedian_SSE(Vector3& median, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    void calcMedian_NoSIMD(Vector3& median, const Vector3& v0, const Vector3& v1, const Vector3& v2);


    inline void calcBBox(Vector3& bmin, Vector3& bmax, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
#if defined(LMATH_USE_SSE)
        calcBBox_SSE(bmin, bmax, v0, v1, v2);
#else
        calcBBox_NoSIMD(bmin, bmax, v0, v1, v2);
#endif
    }

    inline void calcMedian(Vector3& median, const Vector3& v0, const Vector3& v1, const Vector3& v2)
    {
#if defined(LMATH_USE_SSE)
        calcMedian_SSE(median, v0, v1, v2);
#else
        calcMedian_NoSIMD(median, v0, v1, v2);
#endif
    }
}
#endif //INC_LMATH_LMATHUTIL_H__
