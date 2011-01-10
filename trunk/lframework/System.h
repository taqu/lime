#ifndef INC_LFRAMEWORK_SYSTEM_H__
#define INC_LFRAMEWORK_SYSTEM_H__
/**
@file System.h
@author t-sakai
@date 2010/11/28 create
*/
#include "lframework.h"

namespace lmath
{
    class Color;
    class Vector3;
}

namespace lrender
{
    class RenderingSystem;
}

namespace lanim
{
    struct InitParam;
    class AnimationSystem;
}

namespace debug
{
    class DebugPrimitive;
}

namespace lframework
{
    class ASCIIRender;

    class System
    {
    public:
        struct InitParam
        {
            /**
            @param debugFont ... デバッグ出力用フォントファイルパス
            */
            InitParam(const char* debugFont,
                u32 debugFontSize);

            const char* debugFont_;
            u32 debugFontSize_;
        };

        static lrender::RenderingSystem& getRenderSys(){ return *renderingSystem_;}
        static lanim::AnimationSystem& getAnimSys(){ return *animationSystem_;}

        static void initialize(const InitParam& sysParam, const lanim::InitParam& animParam);
        static void terminate();

        static void debugPrint(f32 x, f32 y, const char* format, ...);
        static void drawDebugString();
    private:
        System(const System&);
        System& operator=(const System&);

        static lrender::RenderingSystem* renderingSystem_;
        static lanim::AnimationSystem* animationSystem_;

        static ASCIIRender* debugAsciiRender_;

#if defined(_DEBUG)
    public:
        static void debugDraw(const lmath::Vector3& v0, const lmath::Vector3& v1, const lmath::Vector3& v2, u32 color);
        static void debugDrawClear();
        static void attachDebugDraw();
        static void detachDebugDraw();
    private:
        static debug::DebugPrimitive* debugPrimitive_;
#else
    public:
        static void debugDraw(const lmath::Vector3&, const lmath::Vector3&, const lmath::Vector3&, u32){}
        static void debugDrawClear(){}
#endif
    };
}
#endif //INC_LFRAMEWORK_SYSTEM_H__
