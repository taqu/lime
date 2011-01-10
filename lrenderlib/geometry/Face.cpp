/**
@file Face.cpp
@author t-sakai
@date 2009/12/21
*/
#include "Face.h"
#include "Shape.h"

namespace lrender
{
    Element::Element(Shape* shape, u32 index)
    {
        set(shape, index);
    }

    void Element::set(Shape* shape, u32 index)
    {
        LASSERT(shape != NULL);
        shape_ = shape;
        index_ = index;
        shape_->calcBBox(index_, bmin_, bmax_);
        shape_->calcMedian(index_, median_);
    }

    bool Element::hit(HitRecord& hitRecord, const lmath::Ray& ray) const
    {
        LASSERT(shape_ != NULL);
        return shape_->hit(hitRecord, index_, ray);
    }
}
