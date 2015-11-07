#ifndef INC_LRENDER_BSDFSAMPLE_H__
#define INC_LRENDER_BSDFSAMPLE_H__
/**
@file BSDFSample.h
@author t-sakai
@date 2015/09/27 create
*/
#include "../lrender.h"

namespace lrender
{
    class BSDFSample
    {
    public:
        BSDFSample(f32 u0, f32 u1/*, f32 ucomponent*/)
            :u0_(u0)
            ,u1_(u1)
            //,ucomponent_(ucomponent)
        {}

        f32 u0_;
        f32 u1_;
        //f32 ucomponent_;
    };
}
#endif //INC_LRENDER_BSDFSAMPLE_H__
