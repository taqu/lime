#ifndef INC_LFRAMEWORK_JOINTPOSE_H__
#define INC_LFRAMEWORK_JOINTPOSE_H__
/**
@file JointPose.h
@author t-sakai
@date 2016/11/24 create
*/
#include "lanim.h"
#include <lmath/Vector3.h>
#include <lmath/Quaternion.h>

namespace lfw
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
#endif //INC_LFRAMEWORK_JOINTPOSE_H__
