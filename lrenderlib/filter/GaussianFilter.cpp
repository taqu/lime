/**
@file GaussianFilter.cpp
@author t-sakai
@date 2015/09/11 create
*/
#include "GaussianFilter.h"

namespace lrender
{
    f32 GaussianFilter::evaluate(f32 x) const
    {
        f32 alpha = -1.0f/(2.0f*standardDeviation_*standardDeviation_);
        f32 ret = lmath::exp(alpha * x * x) - lmath::exp(alpha * radius_ * radius_);
        return lcore::maximum(ret*alpha*(-INV_PI), 0.0f);
    }
}
