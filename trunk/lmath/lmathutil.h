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
    class Vector4;

    void calcBBox(Vector3& bmin, Vector3& bmax, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    void calcMedian(Vector3& median, const Vector3& v0, const Vector3& v1, const Vector3& v2);

    void calcAABBPoints(Vector4* AABB, const Vector4& aabbMin, const Vector4& aabbMax);

}
#endif //INC_LMATH_LMATHUTIL_H__
