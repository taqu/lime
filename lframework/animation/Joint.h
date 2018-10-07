#ifndef INC_LFRAMEWORK_JOINT_H_
#define INC_LFRAMEWORK_JOINT_H_
/**
@file Joint.h
@author t-sakai
@date 2016/11/24 create
*/
#include <lmath/Vector4.h>
#include <lmath/Quaternion.h>
#include <lmath/Matrix34.h>
#include "lanim.h"

namespace lfw
{
    class Joint
    {
    public:
        Joint()
            :parentIndex_(InvalidJointIndex)
            ,subjectTo_(InvalidJointIndex)
            ,type_(0)
            ,flag_(0)
        {}

        ~Joint()
        {}

        u8 getParentIndex() const{ return parentIndex_;}
        void setParentIndex(u8 index){ parentIndex_ = index;}

        u8 getSubjectTo() const{ return subjectTo_;}
        void setSubjectTo(u8 index){ subjectTo_ = index;}

        /// 初期姿勢取得
        const lmath::Matrix34& getInitialMatrix() const
        {
            return initialMatrix_;
        }

        /// 初期姿勢セット
        void setInitialMatrix(const lmath::Matrix34& matrix)
        {
            initialMatrix_ = matrix;
        }

        /// 初期姿勢取得
        const lmath::Matrix34& getInvInitialMatrix() const
        {
            return invInitialMatrix_;
        }

        /// 初期姿勢セット
        void setInvInitialMatrix(const lmath::Matrix34& matrix)
        {
            invInitialMatrix_ = matrix;
        }

        u8 getType() const{ return type_;}
        void setType(u8 type){ type_ = type;}

        u8 getFlag() const{ return flag_;}
        void setFlag(u8 flag){ flag_ = flag;}

    private:
        u8 parentIndex_;
        u8 subjectTo_;
        u8 type_;
        u8 flag_;
        lmath::Matrix34 initialMatrix_; /// 初期姿勢
        lmath::Matrix34 invInitialMatrix_; /// 初期姿勢
    };
}
#endif //INC_LFRAMEWORK_JOINT_H_
