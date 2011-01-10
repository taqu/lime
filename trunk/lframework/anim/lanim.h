#ifndef INC_LANIM_H__
#define INC_LANIM_H__
/**
@file lanim.h
@author t-sakai
@date 2010/11/12 create

*/
#include <lcore/lcore.h>
#include <lcore/String.h>

namespace lanim
{
    using lcore::Char;
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::f32;
    using lcore::f64;

    typedef const char* HANDLE;

#if LIME_USE_DUAL_QUATERNION_FOR_SKINNING
    static const u8 LANIM_MAX_SKINNING_MATRICES = (100-1);
#else
    static const u8 LANIM_MAX_SKINNING_MATRICES = (64-1);
#endif

    static const u32 LANIM_MAX_PALLET_MATRICES = 256;

    static const u32 LANIM_MAX_NAME_SIZE = 24;

    static const u32 LANIM_MAX_ANIM_NODE = 512;


    typedef lcore::String<LANIM_MAX_NAME_SIZE> Name;

#define LANIM_FRAME_DURATION_MIN (1.192092896e-04F)

    static const u16 InvalidJointIndex = 0xFFFFU;

    enum JointType
    {
        JointType_Rotation =0, //‰ñ“]
        JointType_RotTrans,    //‰ñ“]EˆÚ“®
        JointType_IK,          //IK
        JointType_UnderIK0,    //IK‰e‹¿‰º
        JointType_UnderIK1,    //IK‰e‹¿‰º
        JointType_UnderRot,    //‰ñ“]‰e‹¿‰º
        JointType_ToIK,        //IKÚ‘±æ
        JointType_Hide,        //”ñ•\¦
        JointType_Twist,       //”P‚è
        JointType_FreeRot,     //‰ñ“]‰^“®B”P‚è‰e‹¿‰ºBæ’[‚©‚ç‚Ì”P‚è‚Ì‰e‹¿‚ğó‚¯‚é
        JointType_Num,
    };

    enum JointFlag
    {
        JointFlag_IKLimitHeading = (0x01U << 0), //‹È‚°‚é²‚ğ§ŒÀ‚·‚é
        JointFlag_Num = 1,
    };
}

#endif //INC_LANIM_H__
