#ifndef INC_LRENDER_GAUSSIANFILTER_H__
#define INC_LRENDER_GAUSSIANFILTER_H__
/**
@file GaussianFilter.h
@author t-sakai
@date 2015/09/11 create
*/
#include "Filter.h"

namespace lrender
{
    class GaussianFilter : public Filter
    {
    public:
        GaussianFilter()
            :standardDeviation_(1.0f)
        {
            radius_ = standardDeviation_ * 4.0f;
            initialize();
        }

        GaussianFilter(f32 standardDeviation)
            :standardDeviation_(standardDeviation)
        {
            radius_ = standardDeviation_ * 4.0f;
            initialize();
        }

        virtual ~GaussianFilter()
        {}

        virtual f32 evaluate(f32 x) const;

    private:
        f32 standardDeviation_;
    };
}
#endif //INC_LRENDER_GAUSSIANFILTER_H__
