#ifndef INC_LRENDER_TENTFILTER_H__
#define INC_LRENDER_TENTFILTER_H__
/**
@file TentFilter.h
@author t-sakai
@date 2015/09/11 create
*/
#include "Filter.h"

namespace lrender
{
    class TentFilter : public Filter
    {
    public:
        TentFilter()
            :Filter(1.0f)
        {
            initialize();
        }

        TentFilter(f32 radius)
            :Filter(radius)
        {
            initialize();
        }

        virtual ~TentFilter()
        {}

        virtual f32 evaluate(f32 x) const;
    };
}
#endif //INC_LRENDER_TENTFILTER_H__
