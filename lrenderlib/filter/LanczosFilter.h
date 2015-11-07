#ifndef INC_LRENDER_LANCZOSFILTER_H__
#define INC_LRENDER_LANCZOSFILTER_H__
/**
@file LanczosFilter.h
@author t-sakai
@date 2015/09/11 create
*/
#include "Filter.h"

namespace lrender
{
    class LanczosFilter : public Filter
    {
    public:
        LanczosFilter()
            :Filter(2.0f)
        {
            initialize();
        }

        LanczosFilter(f32 radius)
            :Filter(radius)
        {
            initialize();
        }

        virtual ~LanczosFilter()
        {}

        virtual f32 evaluate(f32 x) const;
    };
}
#endif //INC_LRENDER_LANCZOSFILTER_H__