/**
@file LanczosFilter.cpp
@author t-sakai
@date 2015/09/11 create
*/
#include "filter/LanczosFilter.h"

namespace lrender
{
    f32 LanczosFilter::evaluate(f32 x) const
    {
        x = lcore::absolute(x);
        if(x<FILTER_EPSILON){
            return 1.0f;
        }else if(radius_<x){
            return 0.0f;
        }
        x *= PI;
        f32 tx = x/radius_;
        return (lmath::sin(x) * lmath::sin(tx)) / (x*tx);
    }
}
