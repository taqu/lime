#ifndef INC_LMATH_COORDINATE_H__
#define INC_LMATH_COORDINATE_H__
/**
@file Coordinate.h
@author t-sakai
@date 2015/12/20 create
*/
#include "lmathcore.h"
#include "Vector3.h"
#include "Vector4.h"

namespace lmath
{
    Vector3 worldToLocal(const Vector3& v, const Vector3& normal, const Vector3& binormal0, const Vector3& binormal1);
    Vector3 localToWorld(const Vector3& v, const Vector3& normal, const Vector3& binormal0, const Vector3& binormal1);

    class Coordinate
    {
    public:
        inline Coordinate();
        inline explicit Coordinate(const Vector3& normal);
        inline Coordinate(const Vector3& normal, const Vector3& binormal0, const Vector3& binormal1);

        Vector3 worldToLocal(const Vector3& v) const;
        Vector3 localToWorld(const Vector3& v) const;


        Vector3 normal_;
        Vector3 binormal0_;
        Vector3 binormal1_;
    };

    inline Coordinate::Coordinate()
    {
    }

    inline Coordinate::Coordinate(const Vector3& normal)
        :normal_(normal)
    {
        orthonormalBasis(binormal0_, binormal1_, normal_);
    }

    inline Coordinate::Coordinate(const Vector3& normal, const Vector3& binormal0, const Vector3& binormal1)
        :normal_(normal)
        ,binormal0_(binormal0)
        ,binormal1_(binormal1)
    {
    }
}
#endif //INC_LMATH_COORDINATE_H__
