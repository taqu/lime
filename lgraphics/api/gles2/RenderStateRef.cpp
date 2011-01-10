/**
@file RenderStateRef.cpp
@author t-sakai
@date 2010/06/16 create
@date 2010/12/12 modify for gles2
*/
#include "../../lgraphicsAPIInclude.h"
#include "../../lgraphics.h"
#include "RenderStateRef.h"


using namespace lcore;

namespace lgraphics
{
    //----------------------------------------------------------
    //---
    //--- StateBlock
    //---
    //----------------------------------------------------------
    class RenderStateRef::StateBlock
    {
    public:
        enum Bit
        {
            Bit_AlphaTest = (0x00000001U << 0),
            Bit_MultiSampleAlias = (0x00000001U << 1),
            Bit_ZEnable = (0x00000001U << 2),
            Bit_ZWriteEnable = (0x00000001U << 3),
            Bit_CullingEnable = (0x00000001U << 4),
            Bit_AlphaBlendEnable = (0x00000001U << 5),
        };

        StateBlock();
        ~StateBlock();


        inline void set(Bit bit);
        inline void set(Bit bit, bool enable);
        inline void reset(Bit bit);
        inline bool check(Bit bit);

        inline CmpFunc getAlphaTestFunc() const;
        inline void setAlphaTestFuc(CmpFunc func);


        inline f32 getAlphaTestRef() const;
        inline void setAlphaTestRef(f32 ref);

        inline CullMode getCullMode() const;
        inline void setCullMode(CullMode mode);

        inline BlendType getAlphaBlendSrc() const;
        inline void setAlphaBlendSrc(BlendType type);

        inline BlendType getAlphaBlendDst() const;
        inline void setAlphaBlendDst(BlendType type);

        inline void apply();


        void addRef()
        {
            ++counter_;
        }

        void release()
        {
            if(--counter_ == 0){
                LIME_DELETE_NONULL this;
            }
        }

        u32 counter_;
        u32 flag_; //ON・OFFフラッグ

        CmpFunc alphaTestFunc_;
        f32 alphaTestRef_;
        CullMode cullMode_;
        BlendType alphaBlendSrc_;
        BlendType alphaBlendDst_;
    };

    RenderStateRef::StateBlock::StateBlock()
        :counter_(0)
        ,flag_(0)
        ,alphaTestFunc_(Cmp_Less)
        ,alphaTestRef_(0.0f)
        ,cullMode_(CullMode_CCW)
        ,alphaBlendSrc_(Blend_InvDestAlpha)
        ,alphaBlendDst_(Blend_DestAlpha)
    {
    }

    RenderStateRef::StateBlock::~StateBlock()
    {
    }

    inline void RenderStateRef::StateBlock::set(Bit bit)
    {
        flag_ |= bit;
    }

    inline void RenderStateRef::StateBlock::set(Bit bit, bool enable)
    {
        (enable)? flag_ |= bit : flag_ &= (~bit);
    }

    inline void RenderStateRef::StateBlock::reset(Bit bit)
    {
        flag_ &= ~bit;
    }

    inline bool RenderStateRef::StateBlock::check(Bit bit)
    {
        return (flag_ & bit) != 0;
    }

    inline CmpFunc RenderStateRef::StateBlock::getAlphaTestFunc() const
    {
        return alphaTestFunc_;
    }

    inline void RenderStateRef::StateBlock::setAlphaTestFuc(CmpFunc func)
    {
        alphaTestFunc_ = func;
    }

    inline f32 RenderStateRef::StateBlock::getAlphaTestRef() const
    {
        return alphaTestRef_;
    }

    inline void RenderStateRef::StateBlock::setAlphaTestRef(f32 ref)
    {
        alphaTestRef_ = ref;
    }

    inline CullMode RenderStateRef::StateBlock::getCullMode() const
    {
        return cullMode_;
    }

    inline void RenderStateRef::StateBlock::setCullMode(CullMode mode)
    {
        cullMode_ = mode;
    }

    inline BlendType RenderStateRef::StateBlock::getAlphaBlendSrc() const
    {
        return alphaBlendSrc_;
    }

    inline void RenderStateRef::StateBlock::setAlphaBlendSrc(BlendType type)
    {
        alphaBlendSrc_ = type;
    }

    inline BlendType RenderStateRef::StateBlock::getAlphaBlendDst() const
    {
        return alphaBlendDst_;
    }

    inline void RenderStateRef::StateBlock::setAlphaBlendDst(BlendType type)
    {
        alphaBlendDst_ = type;
    }

    inline void RenderStateRef::StateBlock::apply()
    {
        (check(Bit_ZEnable))? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        (check(Bit_CullingEnable))? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        (check(Bit_AlphaBlendEnable))? glEnable(GL_BLEND) : glDisable(GL_BLEND);

        glDepthMask( (check(Bit_ZWriteEnable))? GL_TRUE : GL_FALSE );

        glFrontFace(cullMode_);
        glBlendFunc(alphaBlendSrc_, alphaBlendDst_);
    }

    //----------------------------------------------------------
    //---
    //--- RenderStateRef
    //---
    //----------------------------------------------------------
    RenderStateRef::RenderStateRef()
        :stateBlock_(NULL)
    {
    }

    RenderStateRef::RenderStateRef(const RenderStateRef& rhs)
        :stateBlock_(rhs.stateBlock_)
    {
        if(stateBlock_ != NULL){
            stateBlock_->addRef();
        }
    }

    RenderStateRef::RenderStateRef(StateBlock *stateBlock)
        :stateBlock_(stateBlock)
    {
        if(stateBlock_ != NULL){
            stateBlock_->addRef();
        }
    }


    RenderStateRef::~RenderStateRef()
    {
        LSAFE_RELEASE(stateBlock_);
    }

    bool RenderStateRef::begin()
    {
        return true;
    }

    void RenderStateRef::end(RenderStateRef& renderState)
    {
        bool alphaTest = Graphics::getDevice().getAlphaTest();

        f32 alphaTestRef = (1.0f/255.0f) * Graphics::getDevice().getAlphaTestRef();


        GLint alphaTestFunc = Graphics::getDevice().getAlphaTestFunc();

        GLboolean blend = glIsEnabled(GL_BLEND);

        GLint blendSrc = Blend_InvDestAlpha;
        glGetIntegerv(GL_BLEND_SRC_RGB, &blendSrc);

        GLint blendDst = Blend_DestAlpha;
        glGetIntegerv(GL_BLEND_DST_RGB, &blendDst);

        GLboolean cull = glIsEnabled(GL_CULL_FACE);

        GLboolean depth = glIsEnabled(GL_DEPTH_TEST);

        GLboolean depthWrite = GL_FALSE;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWrite);

        GLint frontFace = CullMode_CCW;
        glGetIntegerv(GL_FRONT_FACE, &frontFace);


        StateBlock *stateBlock = LIME_NEW StateBlock;

        stateBlock->set(StateBlock::Bit_AlphaTest, alphaTest==GL_TRUE);
        stateBlock->setAlphaTestRef(alphaTestRef);
        stateBlock->setAlphaTestFuc( static_cast<CmpFunc>(alphaTestFunc) );

        stateBlock->set(StateBlock::Bit_AlphaBlendEnable, blend==GL_TRUE);
        stateBlock->setAlphaBlendSrc( static_cast<BlendType>(blendSrc) );
        stateBlock->setAlphaBlendDst( static_cast<BlendType>(blendDst) );

        stateBlock->set(StateBlock::Bit_CullingEnable, cull==GL_TRUE);
        stateBlock->setCullMode( static_cast<CullMode>(frontFace) );

        stateBlock->set(StateBlock::Bit_ZEnable, depth==GL_TRUE);
        stateBlock->set(StateBlock::Bit_ZWriteEnable, depthWrite==GL_TRUE);

        RenderStateRef tmp(stateBlock);

        renderState.swap(tmp);
    }

    void RenderStateRef::apply()
    {
        stateBlock_->apply();
    }

    bool RenderStateRef::getStateAlphaTest() const
    {
        return stateBlock_->check(StateBlock::Bit_AlphaTest);
    }

    CmpFunc RenderStateRef::getStateAlphaTestFunc() const
    {
        return stateBlock_->getAlphaTestFunc();
    }

    f32 RenderStateRef::getStateAlphaTestRef() const
    {
        return stateBlock_->getAlphaTestRef();
    }

    //-------------------------------------------------------------------

    void RenderStateRef::setAlphaTest(bool enable)
    {
        Graphics::getDevice().setAlphaTest(enable);
    }

    bool RenderStateRef::getAlphaTest()
    {
        return Graphics::getDevice().getAlphaTest();
    }

    void RenderStateRef::setAlphaTestFunc(CmpFunc func)
    {
        Graphics::getDevice().setAlphaTestFunc(func);
    }

    CmpFunc RenderStateRef::getAlphaTestFunc()
    {
        return Graphics::getDevice().getAlphaTestFunc();
    }

    void RenderStateRef::setAlphaTestRef(s32 refValue)
    {
        Graphics::getDevice().setAlphaTestRef(refValue);
    }

    s32 RenderStateRef::getAlphaTestRef()
    {
        return Graphics::getDevice().getAlphaTestRef();
    }


    void RenderStateRef::setCullMode(CullMode mode)
    {
        if(mode == CullMode_None){
            glDisable(GL_CULL_FACE);
        }else{
            glEnable(GL_CULL_FACE);
            glFrontFace(mode);
        }
    }

    CullMode RenderStateRef::getCullMode()
    {
        GLboolean cull = glIsEnabled(GL_CULL_FACE);

        if(cull == GL_FALSE){
            return CullMode_None;
        }

        GLint frontFace = CullMode_CCW;
        glGetIntegerv(GL_FRONT_FACE, &frontFace);
        return static_cast<CullMode>(frontFace);
    }

    void RenderStateRef::setMultiSampleAlias(bool )
    {
    }

    bool RenderStateRef::getMultiSampleAlias()
    {
        return false;
    }

    void RenderStateRef::setZEnable(bool enable)
    {
        (enable)? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    }

    bool RenderStateRef::getZEnable()
    {
        GLboolean depth = glIsEnabled(GL_DEPTH_TEST);
        return (depth == GL_TRUE);
    }

    void RenderStateRef::setZWriteEnable(bool enable)
    {
        glDepthMask( (enable)?GL_TRUE : GL_FALSE );
    }

    bool RenderStateRef::getZWriteEnable()
    {
        GLboolean depthWrite = GL_FALSE;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWrite);
        return (depthWrite == GL_TRUE);
    }

    void RenderStateRef::setAlphaBlendEnable(bool enable)
    {
        (enable)? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    }

    bool RenderStateRef::getAlphaBlendEnable()
    {
        GLboolean blend = glIsEnabled(GL_BLEND);
        return (blend == GL_TRUE);
    }

    void RenderStateRef::setAlphaBlend(BlendType src, BlendType dst)
    {
        glBlendFunc(src, dst);
    }

    BlendType RenderStateRef::getAlphaBlendSrc()
    {
        GLint blendSrc = Blend_InvDestAlpha;
        glGetIntegerv(GL_BLEND_SRC_RGB, &blendSrc);
        return static_cast<BlendType>(blendSrc);
    }

    BlendType RenderStateRef::getAlphaBlendDst()
    {
        GLint blendDst = Blend_DestAlpha;
        glGetIntegerv(GL_BLEND_DST_RGB, &blendDst);
        return static_cast<BlendType>(blendDst);
    }

    void RenderStateRef::setViewPort(s32 x, s32 y, s32 width, s32 height, f32 minz, f32 maxz)
    {
        glViewport(x, y, width, height);
        glDepthRangef(minz, maxz);
    }

    void RenderStateRef::getViewPort(s32& x, s32& y, s32& width, s32& height, f32& minz, f32& maxz)
    {
        GLint values[4];
        glGetIntegerv(GL_VIEWPORT, values);
        x = values[0];
        y = values[1];
        width = values[2];
        height = values[3];

        GLfloat range[2];
        glGetFloatv(GL_DEPTH_RANGE, range);
        minz = range[0];
        maxz = range[1];
    }
}
