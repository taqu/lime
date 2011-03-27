#ifndef INC_DEBUGPRIMITIVE_H__
#define INC_DEBUGPRIMITIVE_H__
/**
@file DebugPrimitive.h
@author t-sakai
@date 2011/01/04 create
*/
#include <lcore/lcore.h>

namespace lmath
{
    class Vector3;
}

namespace debug
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::f32;
    using lcore::f64;

    //------------------------------------------
    //---
    //--- DebugPrimitive
    //---
    //------------------------------------------
    class DebugPrimitive
    {
    public:
        class PrimitiveBatch;

        DebugPrimitive();
        ~DebugPrimitive();

        void initialize(u16 numMaxTriangles);
        void terminate();

        void attach();
        void detach();

        void draw(const lmath::Vector3& v0, const lmath::Vector3& v1, const lmath::Vector3& v2, u32 color);
        void clear();
    private:
        PrimitiveBatch *batch_;
    };
}
#endif //INC_DEBUGPRIMITIVE_H__