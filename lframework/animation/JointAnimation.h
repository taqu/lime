#ifndef INC_LFRAMEWORK_JOINTANIMATION_H__
#define INC_LFRAMEWORK_JOINTANIMATION_H__
/**
@file JointAnimation.h
@author t-sakai
@date 2016/11/24 create
*/
#include "lanim.h"
#include "JointPose.h"

namespace lfw
{
    class JointAnimation
    {
    public:
        inline JointAnimation();
        inline JointAnimation(s32 numPoses);
        inline ~JointAnimation();

        inline void initialize();

        /// ポーズ数取得
        inline s32 getNumPoses() const;

        inline void setNumPoses(s32 numPoses);

        /// 名前取得
        inline const Name& getName() const;

        /// 名前セット
        inline void setName(const Name& name);
        inline void setName(s32 length, const Char* name);

        /// ジョイントポーズ取得
        inline const JointPoseWithTime& getPose(s32 index) const;
        inline JointPoseWithTime& getPose(s32 index);

        /// ジョイントポーズ設定
        inline void setPose(s32 index, JointPoseWithTime& pose);

        s32 binarySearchIndex(f32 time) const;

        /// 次のフレームのインデックスを探索。順方向のみ
        inline s32 getNextIndex(f32 time, s32 index) const;

        /// 次のフレームのインデックスを探索。逆方向のみ
        inline s32 getPrevIndex(f32 time, s32 index) const;

        inline const JointPoseWithTime& binarySearch(f32 time) const
        {
            return poses_[ binarySearchIndex(time) ];
        }
    private:
        Name name_;
        s32 numPoses_;
        //u32 currentIndex_;
        JointPoseWithTime *poses_;
    };

    inline JointAnimation::JointAnimation()
        :numPoses_(0)
        //,currentIndex_(0)
        ,poses_(NULL)
    {
    }


    inline JointAnimation::JointAnimation(s32 numPoses)
        :numPoses_(numPoses)
        //,currentIndex_(0)
        ,poses_(NULL)
    {
        LASSERT(0<=numPoses_);
        poses_ = static_cast<JointPoseWithTime*>(LMALLOC(numPoses_*sizeof(JointPoseWithTime)));
    }

    inline JointAnimation::~JointAnimation()
    {
        LFREE(poses_);
    }

    inline void JointAnimation::initialize()
    {
        //currentIndex_ = 0;
    }

    // ポーズ数取得
    inline s32 JointAnimation::getNumPoses() const
    {
        return numPoses_;
    }

    inline void JointAnimation::setNumPoses(s32 numPoses)
    {
        LASSERT(0<=numPoses);
        numPoses_ = numPoses;
        LFREE(poses_);
        poses_ = static_cast<JointPoseWithTime*>(LMALLOC(numPoses_*sizeof(JointPoseWithTime)));
    }

    // 名前取得
    inline const Name& JointAnimation::getName() const
    {
        return name_;
    }

    // 名前セット
    inline void JointAnimation::setName(const Name& name)
    {
        name_ = name;
    }

    inline void JointAnimation::setName(s32 length, const Char* name)
    {
        name_.assign(length, name);
    }

    // ジョイントポーズ取得
    inline const JointPoseWithTime& JointAnimation::getPose(s32 index) const
    {
        LASSERT(0<=index && index<numPoses_);
        return poses_[index];
    }

    // ジョイントポーズ取得
    inline JointPoseWithTime& JointAnimation::getPose(s32 index)
    {
        LASSERT(0<=index && index<numPoses_);
        return poses_[index];
    }

    // ジョイントポーズ設定
    inline void JointAnimation::setPose(s32 index, JointPoseWithTime& pose)
    {
        LASSERT(0<=index && index<numPoses_);
        poses_[index] = pose;
    }

    // 次のフレームのインデックスを探索。順方向のみ
    inline s32 JointAnimation::getNextIndex(f32 time, s32 index) const
    {
        for(s32 i=index+1; i<numPoses_; ++i){
            if(time<=poses_[i].time_){
                index = i;
                break;
            }
        }
        return index;
    }

    // 次のフレームのインデックスを探索。逆方向のみ
    inline s32 JointAnimation::getPrevIndex(f32 time, s32 index) const
    {
        for(s32 i=index-1; 0<=i; --i){
            if(poses_[i].time_<=time){
                index = i;
                break;
            }
        }
        return index;
    }
}
#endif //INC_LFRAMEWORK_JOINTANIMATION_H__
