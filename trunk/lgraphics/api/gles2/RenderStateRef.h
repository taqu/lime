#ifndef INC_LGRAPHICS_ES2_RENDERSTATEREF_H__
#define INC_LGRAPHICS_ES2_RENDERSTATEREF_H__
/**
@file RenderStateRef.h
@author t-sakai
@date 2010/06/16 create
@date 2010/12/12 modify for gles2
*/
#include "../../lgraphicscore.h"
#include "Enumerations.h"

namespace lgraphics
{
    //----------------------------------------------------------
    //---
    //--- RenderState
    //---
    //----------------------------------------------------------
    class RenderState
    {
    public:
        static void setAlphaTest(bool enable);
        static bool getAlphaTest();

        static void setAlphaTestFunc(CmpFunc func);
        static CmpFunc getAlphaTestFunc();

        static void setAlphaTestRef(s32 refValue);
        static s32 getAlphaTestRef();

        static void setCullMode(CullMode mode);
        static CullMode getCullMode();

        static void setMultiSampleAlias(bool enable);
        static bool getMultiSampleAlias();

        static void setZEnable(bool enable);
        static bool getZEnable();

        static void setZWriteEnable(bool enable);
        static bool getZWriteEnable();

        static void setAlphaBlendEnable(bool enable);
        static bool getAlphaBlendEnable();

        static void setAlphaBlend(BlendType src, BlendType dst);
        static BlendType getAlphaBlendSrc();
        static BlendType getAlphaBlendDst();

        // Vertex, Pixel以外のステート
        //--------------------------------------------------------
        static void setViewPort(s32 x, s32 y, s32 width, s32 height, f32 minz, f32 maxz);
        static void getViewPort(s32& x, s32& y, s32& width, s32& height, f32& minz, f32& maxz);

    };


    //----------------------------------------------------------
    //---
    //--- RenderStateRef
    //---
    //----------------------------------------------------------
    class RenderStateRef
    {
    public:

        void setAlphaTest(bool enable);
        bool getAlphaTest() const;

        void setAlphaTestFunc(CmpFunc func);
        CmpFunc getAlphaTestFunc() const;

        void setAlphaTestRef(s32 refValue);
        s32 getAlphaTestRef() const;

        void setCullMode(CullMode mode);
        CullMode getCullMode() const;

        void setMultiSampleAlias(bool enable);
        bool getMultiSampleAlias() const;

        void setZEnable(bool enable);
        bool getZEnable() const;

        void setZWriteEnable(bool enable);
        bool getZWriteEnable() const;

        void setAlphaBlendEnable(bool enable);
        bool getAlphaBlendEnable() const;

        void setAlphaBlend(BlendType src, BlendType dst);
        BlendType getAlphaBlendSrc() const;
        BlendType getAlphaBlendDst() const;


        RenderStateRef();
        RenderStateRef(const RenderStateRef& rhs);

        ~RenderStateRef();

        RenderStateRef& operator=(const RenderStateRef& rhs)
        {
            RenderStateRef tmp(rhs);
            tmp.swap(*this);
            return *this;
        }

        void apply();

        void swap(RenderStateRef& rhs)
        {
            lcore::swap(stateBlock_, rhs.stateBlock_);
        }
    private:
        class StateBlock;

        RenderStateRef(StateBlock *stateBlock);
        StateBlock *stateBlock_;
    };
}

//---------------------------------------------------
//---
//--- lcore::swap特殊化
//---
//---------------------------------------------------
namespace lcore
{
    template<>
    inline void swap<lgraphics::RenderStateRef>(lgraphics::RenderStateRef& l, lgraphics::RenderStateRef& r)
    {
        l.swap(r);
    }
}
#endif //INC_LGRAPHICS_ES2_RENDERSTATEREF_H__
