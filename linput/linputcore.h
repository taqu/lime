#ifndef INC_LINPUTCORE_H__
#define INC_LINPUTCORE_H__
/**
@file linputcore.h
@author t-sakai
@date 2009/05/13 create
*/
#include <lcore/lcore.h>

#if defined(_WIN32)
#define LIME_DINPUT (1)
#endif

#define SAFE_RELEASE(p) {if(p!=NULL){(p)->Release();(p)=NULL;}}

namespace linput
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::f32;
    using lcore::f64;
}
#endif //INC_LINPUTCORE_H__
