/**
@file TentFilter.cpp
@author t-sakai
@date 2015/09/11 create
*/
#include "TentFilter.h"

namespace lrender
{
    f32 TentFilter::evaluate(f32 x) const
    {
        return lcore::maximum(1.0f-lcore::absolute(x/radius_), 0.0f);
    }
}
