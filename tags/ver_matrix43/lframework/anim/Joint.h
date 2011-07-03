#ifndef INC_LANIM_JOINT_H__
#define INC_LANIM_JOINT_H__
/**
@file Joint.h
@author t-sakai
@date 2010/11/12 create

*/
#include <lmath/Vector3.h>
#include "lanim.h"

namespace lanim
{
    class Joint
    {
    public:
        Joint()
            :parentIndex_(InvalidJointIndex)
            ,type_(0)
            ,flag_(0)
        {}

        ~Joint()
        {}

        u16 getParentIndex() const{ return parentIndex_;}
        void setParentIndex(u16 index){ parentIndex_ = index;}

        /// 位置取得
        const lmath::Vector3& getPosition() const{ return position_;}

        /// 位置セット
        void setPosition(const lmath::Vector3& position){ position_ = position;}

        u8 getType() const{ return type_;}
        void setType(u8 type){ type_ = type;}

        u8 getFlag() const{ return flag_;}
        void setFlag(u8 flag){ flag_ = flag;}

    private:
        u16 parentIndex_;
        u8 type_;
        u8 flag_;
        lmath::Vector3 position_; /// オブジェクトローカル位置
    };
}
#endif //INC_LANIM_JOINT_H__
