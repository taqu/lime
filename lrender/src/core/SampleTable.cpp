/**
@file SampleTable.cpp
@author t-sakai
@date 2015/11/26 create
*/
#include "core/SampleTable.h"

namespace lrender
{
    SampleTable::SampleTable()
        :resolution_(0)
        ,table_(NULL)
    {
    }

    SampleTable::~SampleTable()
    {
        LDELETE_ARRAY(table_);
    }

    f32 SampleTable::interpolateLinear(f32 x) const
    {
        x = lcore::clamp01(x);

        f32 findex = x*resolution_;
        s32 index = static_cast<s32>(findex);
        if(index<resolution_) {
            f32 t = findex-index;
            return lcore::lerp(table_[index], table_[index+1], t);
        } else {
            return table_[index];
        }
    }
}
