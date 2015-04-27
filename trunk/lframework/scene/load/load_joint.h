#ifndef INC_LSCENE_LLOAD_JOINT_H__
#define INC_LSCENE_LLOAD_JOINT_H__
/**
@file load_joint.h
@author t-sakai
@date 2014/06/02 create
*/
#include "load.h"
#include <lmath/Vector3.h>
#include <lmath/Quaternion.h>
#include <lmath/Matrix34.h>

namespace lscene
{
namespace lload
{
    class Joint
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
}
#endif //INC_LSCENE_LLOAD_JOINT_H__
