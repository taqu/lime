/**
@file CubicSplineFilter.cpp
@author t-sakai
@date 2015/09/11 create
*/
#include "filter/CubicSplineFilter.h"

namespace lrender
{
namespace
{
    f32 solveCubic(f32 x)
    {
        f32 u = x;
        for(s32 i=0; i<5; ++i){
            u = (11.0f*x + u*u*(6.0f + u*(8.0f - u*9.0f))) / (4.0f + 12.0f*u*(1.0f + u*(1.0f-u)));
        }
        return u;
    }
}

    f32 CubicSplineFilter::evaluate(f32 x) const
    {
        f32 ret;
        if(x<(1.0f/24.0f)){
            ret = lmath::pow(24.0f*x, 0.25f) - 2.0f;

        }else if(x<0.5f){
            ret = solveCubic(24.0f * (x - (1.0f/24.0f))/11.0f) - 1.0f;

        }else if(x<(23.0f/24.0f)){
            ret = 1.0f - solveCubic(24.0f * (23.0f/24.0f - x)/11.0f);

        }else{
            ret = 2.0f - lmath::pow(24.0f * (1.0f-x), 0.25f);
        }
        return 0.25f * ret;
    }
}
