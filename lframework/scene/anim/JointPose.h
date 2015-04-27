#ifndef INC_LSCENE_LANIM_JOINTPOSE_H__
#define INC_LSCENE_LANIM_JOINTPOSE_H__
/**
@file JointPose.h
@author t-sakai
@date 2010/11/12 create

*/
#include "lanim.h"
#include <lmath/Vector3.h>
#include <lmath/Quaternion.h>

namespace lscene
{
namespace lanim
{
    struct JointPose
    {
        lmath::Quaternion rotation_;
        lmath::Vector3 offset_;
        lmath::Vector3 translation_;
    };

    struct JointPoseWithTime
    {
        f32 time_;
        lmath::Vector3 translation_;
        lmath::Quaternion rotation_;
    };
}
}
#endif //INC_LSCENE_LANIM_JOINTPOSE_H__
