#ifndef INC_LRENDER_BOXFILTER_H__
#define INC_LRENDER_BOXFILTER_H__
/**
@file BoxFilter.h
@author t-sakai
@date 2015/09/11 create
*/
#include "Filter.h"

namespace lrender
{
    class BoxFilter : public Filter
    {
    public:
        BoxFilter()
            :Filter(0.5f+FILTER_EPSILON)
        {
            initialize();
        }

        BoxFilter(f32 radius)
            :Filter(radius+FILTER_EPSILON)
        {
            initialize();
        }

        virtual ~BoxFilter()
        {}

        virtual f32 evaluate(f32 x) const;
    };
}
#endif //INC_LRENDER_BOXFILTER_H__
