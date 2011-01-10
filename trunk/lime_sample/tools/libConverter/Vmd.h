#ifndef INC_VMD_H__
#define INC_VMD_H__
/**
@file Vmd.h
@author t-sakai
@date 2010/12/30 create
*/
#include <lcore/lcore.h>
#include <lcore/liofwd.h>
#include <lframework/anim/AnimationClip.h>

namespace vmd
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

    static const u32 NameSize = 20;
    static const u32 BoneNameSize = 15;
    static const u32 JointNameSize = 16;

    static const u32 DefaultVectorSize = 64;

    //------------------------------------------------------------
    //--- VMDファイルフォーマット用構造体
    //------------------------------------------------------------
    //--------------------------------------
    //---
    //--- Header
    //---
    //--------------------------------------
    struct Header
    {
        static const u32 MagicSize = 30;

        Header(){}
        ~Header(){}

        Char magic_[MagicSize];
        Char name_[NameSize];

        /// フレームデータ数
        u32 frameNum_;

        //ストリームロード
        friend lcore::istream& operator>>(lcore::istream& is, Header& rhs);
    };

    lcore::istream& operator>>(lcore::istream& is, Header& rhs);

    //--------------------------------------
    //---
    //--- Frame
    //---
    //--------------------------------------
    struct Frame
    {
        Frame(){}
        ~Frame(){}

        Char name_[BoneNameSize];

        u32 frameNo_;
        f32 position_[3];
        f32 rotation_[4];
        u32 interp_[16];

        //ストリームロード
        friend lcore::istream& operator>>(lcore::istream& is, Header& rhs);
    };

    lcore::istream& operator>>(lcore::istream& is, Frame& rhs);


    //--------------------------------------
    //---
    //--- Pack
    //---
    //--------------------------------------
    class Pack
    {
    public:
        static lanim::AnimationClip::pointer loadFromFile(const Char* filepath);
        static lanim::AnimationClip::pointer loadFromMemory(const u8* data, u32 size);

    private:
        static void loadInternal(lanim::AnimationClip::pointer& dst, lcore::istream& is);

    };
}

#endif //INC_VMD_H__
