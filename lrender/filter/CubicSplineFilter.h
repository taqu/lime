#ifndef INC_LRENDER_CUBICSPLINEFILTER_H__
#define INC_LRENDER_CUBICSPLINEFILTER_H__
/**
@file CubicSplineFilter.h
@author t-sakai
@date 2015/09/11 create
*/
#include "Filter.h"

namespace lrender
{
    class CubicSplineFilter : public Filter
    {
    public:
        CubicSplineFilter()
            :Filter(1.0f)
        {
            initialize();
        }

        CubicSplineFilter(f32 radius)
            :Filter(radius)
        {
            initialize();
        }

        virtual ~CubicSplineFilter()
        {}

        virtual f32 evaluate(f32 x) const;
    };
}
#endif //INC_LRENDER_CUBICSPLINEFILTER_H__
