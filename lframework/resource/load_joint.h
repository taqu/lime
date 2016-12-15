#ifndef INC_LFRAMEWORK_LOADJOINT_H__
#define INC_LFRAMEWORK_LOADJOINT_H__
/**
@file load_joint.h
@author t-sakai
@date 2016/11/23 create
*/
#include "load.h"
#include <lmath/Vector3.h>
#include <lmath/Quaternion.h>
#include <lmath/Matrix34.h>

namespace lfw
{
    class LoadJoint
    {
    public:
        u8 parent_;
        u8 subjectTo_;
        u8 type_;
        u8 flag_;
        //lmath::Vector3 translation_;
        //lmath::Quaternion rotation_;
        lmath::Matrix34 initialMatrix_;
        Char name_[MaxNameSize];
    };
}
#endif //INC_LFRAMEWORK_LOADJOINT_H__
