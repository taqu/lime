#ifndef INC_LFRAMEWORK_ECSINITPARAM_H_
#define INC_LFRAMEWORK_ECSINITPARAM_H_
/**
@file ECSInitParam.h
@author t-sakai
@date 2016/11/12 create
*/
#include "lecs.h"

namespace lfw
{
    struct ECSInitParam
    {
        s32 maxTypes_;
        u32 flags_;

        ECSInitParam()
            :maxTypes_(lfw::ECSType_Max)
            ,flags_(Flag_CreateRequiredComponentManagers)
        {}
    };
}
#endif //INC_LFRAMEWORK_ECSINITPARAM_H_
