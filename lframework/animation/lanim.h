#ifndef INC_LFRAMEWORK_LANIM_H_
#define INC_LFRAMEWORK_LANIM_H_
/**
@file lanim.h
@author t-sakai
@date 2016/11/24 create
*/
#include "../lframework.h"

#include <lgraphics/lgraphics.h>

namespace lcore
{
    class File;
}

namespace lfw
{
#if LIME_USE_DUAL_QUATERNION_FOR_SKINNING
    static const u32 LANIM_MAX_SKINNING_MATRICES = (100-1);
#else
    static const u32 LANIM_MAX_SKINNING_MATRICES = lgfx::LGFX_MAX_SKINNING_MATRICES;
#endif
    static const u8 LANIM_INVALID_SKINNING_MATRIX = 255;

    static const u32 LANIM_MAX_PALLET_MATRICES = LANIM_MAX_SKINNING_MATRICES;

    static const u32 LANIM_MAX_NAME_SIZE = LFW_CONFIG_NAMESIZE;

    typedef lcore::StaticString<LANIM_MAX_NAME_SIZE> Name;

#define LANIM_FRAME_DURATION_MIN (1.192092896e-04F)

    static const u8 InvalidJointIndex = 0xFFU;
    static const u8 MaxJoints = 0xFEU;

    enum JointType
    {
        JointType_Rotation =0, //回転
        JointType_RotTrans,    //回転・移動
        JointType_IK,          //IK
        JointType_UnderIK,    //IK影響下
        JointType_UnderRot,    //回転影響下
        JointType_ToIK,        //IK接続先
        JointType_Hide,        //非表示
        JointType_Twist,       //捻り
        JointType_FreeRot,     //回転運動。捻り影響下。先端からの捻りの影響を受ける

        JointType_End, //ジョイント連結の終端
        JointType_Num,
    };

    enum JointFlag
    {
        JointFlag_IKLimitHeading = (0x01U << 0), //曲げる軸を制限する
        JointFlag_Num = 1,
    };

    enum BlendMode
    {
        BlendMode_None =0,
        BlendMode_Mix,
        BlendMode_Add,
    };

    enum AnimationFlag
    {
        AnimationFlag_None = 0,
        AnimationFlag_Play = (0x01U<<0),
        AnimationFlag_Delay = (0x01U<<1),
        AnimationFlag_Loop = (0x01U<<2),
    };

#define LANIM_ANIMATION_EPSILON (0.001f)
#define LANIM_ANIMATION_TIME_EPSILON (0.0001f)

    class AnimationSystem
    {
    public:
        AnimationSystem();
        ~AnimationSystem();

        inline static f32 getUnitTime();
        inline static f32 getInvUnitTime();
        inline static void setUnitTime(f32 time);

        inline static f32 getAnimationSpeed();
        inline static void setAnimationSpeed(f32 speed);

    private:
        static AnimationSystem instance_;

        f32 unitTime_;
        f32 invUnitTime_;
        f32 animationSpeed_;
    };

    inline f32 AnimationSystem::getUnitTime()
    {
        return instance_.unitTime_;
    }

    inline f32 AnimationSystem::getInvUnitTime()
    {
        return instance_.invUnitTime_;
    }

    inline void AnimationSystem::setUnitTime(f32 time)
    {
        LASSERT(LANIM_ANIMATION_EPSILON<=time);
        instance_.unitTime_ = time;
        instance_.invUnitTime_ = 1.0f/time;
    }

    inline f32 AnimationSystem::getAnimationSpeed()
    {
        return instance_.animationSpeed_;
    }

    inline void AnimationSystem::setAnimationSpeed(f32 speed)
    {
        LASSERT(LANIM_ANIMATION_EPSILON<=speed);
        instance_.animationSpeed_ = speed;
    }

    void write(lcore::File& os, const Name& name);
    void read(Name& name, lcore::File& is);
}
#endif //INC_LFRAMEWORK_LANIM_H_
