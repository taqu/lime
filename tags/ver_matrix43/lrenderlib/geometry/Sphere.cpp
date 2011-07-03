/**
@file Sphere.cpp
@author t-sakai
@date 2010/01/12 create

*/
#include "Sphere.h"

#include <lmath/geometry/Ray.h>
#include <lmath/geometry/RayTest.h>
#include "HitRecord.h"
#include "ShadingGeometry.h"
#include "Face.h"

namespace lrender
{
    bool Sphere::getElements(Element* elements, u32 bufferCount)
    {
        LASSERT(elements != NULL);
        LASSERT(bufferCount >= calcNumElements());

        elements[0].set(this, 0);
        return true;
    }

    void Sphere::calcBBox(u32 index, Vector3& bmin, Vector3& bmax) const
    {
        const Vector3 &pos = impl_.position_;
        const f32 &radius = impl_.radius_;

        bmin.set(pos._x-radius, pos._y-radius, pos._z-radius);
        bmax.set(pos._x+radius, pos._y+radius, pos._z+radius);
    }

    void Sphere::calcMedian(u32 index, Vector3& median) const
    {
        median = impl_.position_;
    }

    bool Sphere::hit(HitRecord& hitRecord, u32 index, const lmath::Ray& ray) const
    {
        bool bHit = lmath::testRaySphere(hitRecord.t_, ray, impl_);
        if(!bHit){
            return false;
        }
        hitRecord.shape_ = this;

        //衝突位置計算
        Vector3 &hitPos = hitRecord.position_;
        hitPos = ray.direction_;
        hitPos *= hitRecord.t_;
        hitPos += ray.origin_;

        ////法線計算
        //Vector3 &hitNormal = hitRecord.normal_;
        //hitNormal = hitPos;
        //hitNormal -= impl_.position_;
        //hitNormal.normalize();

        return true;
    }

    void Sphere::calcShadingGeometry(ShadingGeometry& shadingGeom, const HitRecord& hitRecord) const
    {
        shadingGeom.position_ = hitRecord.position_;

        //法線計算
        Vector3 &hitNormal = shadingGeom.normal_;
        hitNormal = hitRecord.position_;
        hitNormal -= impl_.position_;
        hitNormal.normalize();

        shadingGeom.color_.set(1.0f, 1.0f, 1.0f, 1.0f);
    }
}
