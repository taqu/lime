#ifndef INC_LGRAPHICS_DX9_RENDERSTATEREF_H__
#define INC_LGRAPHICS_DX9_RENDERSTATEREF_H__
/**
@file RenderStateRef.h
@author t-sakai
@date 2009/01/19 create
*/
#include "../../lgraphicscore.h"
#include "Enumerations.h"

struct IDirect3DStateBlock9;

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

        static void setAlphaTestRef(u32 refValue);
        static u32 getAlphaTestRef();

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

        static void setFillMode(FillMode mode);
        static FillMode getFillMode();

        // Vertex, Pixel以外のステート
        //--------------------------------------------------------
        static void setViewPort(u32 x, u32 y, u32 width, u32 height, f32 minz, f32 maxz);
        static void getViewPort(u32& x, u32& y, u32& width, u32& height, f32& minz, f32& maxz);

    };

    class RenderStateRef
    {
    public:
        void setAlphaTest(bool enable);
        bool getAlphaTest() const;

        void setAlphaTestFunc(CmpFunc func);
        CmpFunc getAlphaTestFunc() const;

        void setAlphaTestRef(u32 refValue);
        u32 getAlphaTestRef() const;

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
#endif //INC_LGRAPHICS_DX9_RENDERSTATEREF_H__
