#ifndef INC_LRENDER_FILTER_H__
#define INC_LRENDER_FILTER_H__
/**
@file Filter.h
@author t-sakai
@date 2013/05/24 create
*/
#include "../lrender.h"

namespace lrender
{
    class Filter
    {
    public:
        static const s32 FilterSize = 31;

        virtual ~Filter()
        {}

        inline f32 getRadius() const;
        virtual f32 evaluate(f32 x) const =0;

        inline f32 evaluateDiscretized(f32 x) const;

        void initialize();
    protected:
        Filter()
            :radius_(0.0f)
            ,scale_(0.0f)
        {}

        Filter(f32 radius)
            :radius_(radius)
            ,scale_(0.0f)
        {
            LASSERT(0.0f<radius_);
        }

        f32 radius_;
        f32 scale_;
        f32 values_[FilterSize+1];
    };

    inline f32 Filter::getRadius() const
    {
        return radius_;
    }

    inline f32 Filter::evaluateDiscretized(f32 x) const
    {
        s32 i = lcore::minimum(static_cast<s32>(lcore::absolute(x)*scale_), FilterSize);
        return values_[i];
    }
}
#endif //INC_LRENDER_FILTER_H__
