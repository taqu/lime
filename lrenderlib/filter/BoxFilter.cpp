/**
@file BoxFilter.cpp
@author t-sakai
@date 2015/09/11 create
*/
#include "BoxFilter.h"

namespace lrender
{
    f32 BoxFilter::evaluate(f32 x) const
    {
        return (lcore::absolute(x)<=radius_)? 1.0f : 0.0f;
    }
}
