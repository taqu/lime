#ifndef INC_LRENDER_COORDINATE_H__
#define INC_LRENDER_COORDINATE_H__
/**
@file Coordinate.h
@author t-sakai
@date 2015/09/24 create
*/
#include "../lrender.h"

namespace lrender
{
    class Coordinate
    {
    public:
        inline Coordinate();
        inline explicit Coordinate(const Vector3& normal);
        inline Coordinate(const Vector3& normal, const Vector3& binormal, const Vector3& tangent);

        Vector3 worldToLocal(const Vector3& v) const;
        Vector3 localToWorld(const Vector3& v) const;


        Vector3 normal_;
        Vector3 binormal_;
        Vector3 tangent_;
    };

    inline Coordinate::Coordinate()
    {
    }

    inline Coordinate::Coordinate(const Vector3& normal)
        :normal_(normal)
    {
        lrender::orthonormalBasis(binormal_, tangent_, normal_);
    }

    inline Coordinate::Coordinate(const Vector3& normal, const Vector3& binormal, const Vector3& tangent)
        :normal_(normal)
        ,binormal_(binormal)
        ,tangent_(tangent)
    {
    }

    /**
    @brief ñ@ê¸Ç™zé≤ï˚å¸ÇÃç¿ïWån
    */
    class LocalCoordinate
    {
    public:
        inline static f32 cosTheta(const Vector3& v);
        inline static f32 absCosTheta(const Vector3& v);
        inline static f32 cosTheta2(const Vector3& v);

        inline static f32 sinTheta(const Vector3& v);
        inline static f32 absSinTheta(const Vector3& v);
        inline static f32 sinTheta2(const Vector3& v);

        inline static f32 tanTheta(const Vector3& v);
        inline static f32 absTanTheta(const Vector3& v);
        inline static f32 tanTheta2(const Vector3& v);

        inline static f32 cosPhi(const Vector3& v);
        inline static f32 cosPhi2(const Vector3& v);
        inline static f32 sinPhi(const Vector3& v);
        inline static f32 sinPhi2(const Vector3& v);

        inline static bool isSameHemisphere(const Vector3& lhs, const Vector3& rhs);
    };

    inline f32 LocalCoordinate::cosTheta(const Vector3& v)
    {
        return v.z_;
    }

    inline f32 LocalCoordinate::absCosTheta(const Vector3& v)
    {
        return lcore::absolute(v.z_);
    }

    inline f32 LocalCoordinate::cosTheta2(const Vector3& v)
    {
        return v.z_ * v.z_;
    }

    inline f32 LocalCoordinate::sinTheta(const Vector3& v)
    {
        f32 sn2 = sinTheta2(v);
        return (sn2<=0.0f)? 0.0f : lmath::sqrt(sn2);
    }

    inline f32 LocalCoordinate::absSinTheta(const Vector3& v)
    {
        return sinTheta(v);
    }

    inline f32 LocalCoordinate::sinTheta2(const Vector3& v)
    {
        return 1.0f - cosTheta2(v);
    }

    inline f32 LocalCoordinate::tanTheta(const Vector3& v)
    {
        f32 sn2 = sinTheta2(v);
        if(sn2<=ANGLE_EPSILON){
            return 0.0f;
        }
        return lmath::sqrt(sn2)/v.z_;
    }

    inline f32 LocalCoordinate::absTanTheta(const Vector3& v)
    {
        f32 sn2 = sinTheta2(v);
        if(sn2<=ANGLE_EPSILON){
            return 0.0f;
        }
        return lmath::sqrt(sn2)/lcore::absolute(v.z_);
    }

    inline f32 LocalCoordinate::tanTheta2(const Vector3& v)
    {
        f32 cs2 = cosTheta2(v);
        f32 sn2 = 1.0f - cs2;
        if(sn2<=ANGLE_EPSILON){
            return 0.0f;
        }
        return lmath::sqrt(sn2)/cs2;
    }

    inline f32 LocalCoordinate::cosPhi(const Vector3& v)
    {
        f32 snTheta = sinTheta(v);
        if(snTheta<=ANGLE_EPSILON){
            return 1.0f;
        }
        return lcore::clamp(v.x_/snTheta, -1.0f, 1.0f);
    }

    inline f32 LocalCoordinate::cosPhi2(const Vector3& v)
    {
        return lcore::clamp01(v.x_*v.x_/sinTheta2(v));
    }

    inline f32 LocalCoordinate::sinPhi(const Vector3& v)
    {
        f32 snTheta = sinTheta(v);
        if(snTheta<=ANGLE_EPSILON){
            return 1.0f;
        }
        return lcore::clamp(v.y_/snTheta, -1.0f, 1.0f);
    }

    inline f32 LocalCoordinate::sinPhi2(const Vector3& v)
    {
        return lcore::clamp01(v.y_*v.y_/sinTheta2(v));
    }

    inline bool LocalCoordinate::isSameHemisphere(const Vector3& lhs, const Vector3& rhs)
    {
        const u32& lu = *(u32*)(&lhs.z_);
        const u32& ru = *(u32*)(&rhs.z_);
        u32 r = lu ^ ru;
        return 0 == (r & ~0x7FFFFFFFU);
    }
}
#endif //INC_LRENDER_COORDINATE_H__
