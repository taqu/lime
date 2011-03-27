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
    //--- RenderState
    //---
    //----------------------------------------------------------
    void RenderState::setAlphaTest(bool enable)
    {
        Graphics::getDevice().setAlphaTest(enable);
    }

    bool RenderState::getAlphaTest()
    {
        return Graphics::getDevice().getAlphaTest();
    }

    void RenderState::setAlphaTestFunc(CmpFunc func)
    {
        Graphics::getDevice().setAlphaTestFunc(func);
    }

    CmpFunc RenderState::getAlphaTestFunc()
    {
        return Graphics::getDevice().getAlphaTestFunc();
    }

    void RenderState::setAlphaTestRef(s32 refValue)
    {
        Graphics::getDevice().setAlphaTestRef(refValue);
    }

    s32 RenderState::getAlphaTestRef()
    {
        return Graphics::getDevice().getAlphaTestRef();
    }


    void RenderState::setCullMode(CullMode mode)
    {
        if(mode == CullMode_None){
            glDisable(GL_CULL_FACE);
        }else{
            glEnable(GL_CULL_FACE);
            glFrontFace(mode);
        }
    }

    CullMode RenderState::getCullMode()
    {
        GLboolean cull = glIsEnabled(GL_CULL_FACE);

        if(cull == GL_FALSE){
            return CullMode_None;
        }

        GLint frontFace = CullMode_CCW;
        glGetIntegerv(GL_FRONT_FACE, &frontFace);
        return static_cast<CullMode>(frontFace);
    }

    void RenderState::setMultiSampleAlias(bool )
    {
    }

    bool RenderState::getMultiSampleAlias()
    {
        return false;
    }

    void RenderState::setZEnable(bool enable)
    {
        (enable)? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    }

    bool RenderState::getZEnable()
    {
        GLboolean depth = glIsEnabled(GL_DEPTH_TEST);
        return (depth == GL_TRUE);
    }

    void RenderState::setZWriteEnable(bool enable)
    {
        (enable)? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
    }

    bool RenderState::getZWriteEnable()
    {
        GLboolean depthWrite = GL_FALSE;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWrite);
        return (depthWrite == GL_TRUE);
    }

    void RenderState::setAlphaBlendEnable(bool enable)
    {
        (enable)? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    }

    bool RenderState::getAlphaBlendEnable()
    {
        GLboolean blend = glIsEnabled(GL_BLEND);
        return (blend == GL_TRUE);
    }

    void RenderState::setAlphaBlend(BlendType src, BlendType dst)
    {
        glBlendFunc(src, dst);
    }

    BlendType RenderState::getAlphaBlendSrc()
    {
        GLint blendSrc = Blend_InvDestAlpha;
        glGetIntegerv(GL_BLEND_SRC_RGB, &blendSrc);
        return static_cast<BlendType>(blendSrc);
    }

    BlendType RenderState::getAlphaBlendDst()
    {
        GLint blendDst = Blend_DestAlpha;
        glGetIntegerv(GL_BLEND_DST_RGB, &blendDst);
        return static_cast<BlendType>(blendDst);
    }

    void RenderState::setViewPort(s32 x, s32 y, s32 width, s32 height, f32 minz, f32 maxz)
    {
        glViewport(x, y, width, height);
        glDepthRangef(minz, maxz);
    }

    void RenderState::getViewPort(s32& x, s32& y, s32& width, s32& height, f32& minz, f32& maxz)
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
        ,alphaTestRef_(0.5f)
        ,cullMode_(CullMode_CCW)
        ,alphaBlendSrc_(Blend_SrcAlpha)
        ,alphaBlendDst_(Blend_InvSrcAlpha)
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

        //アルファブレンド有効、無効の設定はレンダリングパスで行う
        if(check(Bit_AlphaBlendEnable)){
            //glEnable(GL_BLEND);
            glBlendFuncSeparate(alphaBlendSrc_, alphaBlendDst_, Blend_SrcAlpha, Blend_DestAlpha);
        }else{
            //glDisable(GL_BLEND);
        }

        GLboolean flag = (check(Bit_ZWriteEnable))? GL_TRUE : GL_FALSE;
        glDepthMask( flag );

        glFrontFace(cullMode_);
    }

    //----------------------------------------------------------
    //---
    //--- RenderStateRef
    //---
    //----------------------------------------------------------
    RenderStateRef::RenderStateRef()
    {
        stateBlock_ = LIME_NEW StateBlock;
        stateBlock_->addRef();
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


    void RenderStateRef::apply()
    {
        stateBlock_->apply();
    }

    //-------------------------------------------------------------------

    void RenderStateRef::setAlphaTest(bool enable)
    {
        stateBlock_->set(StateBlock::Bit_AlphaTest, enable);
    }

    bool RenderStateRef::getAlphaTest() const
    {
        return stateBlock_->check(StateBlock::Bit_AlphaTest);
    }

    void RenderStateRef::setAlphaTestFunc(CmpFunc func)
    {
        stateBlock_->setAlphaTestFuc( func );
    }

    CmpFunc RenderStateRef::getAlphaTestFunc() const
    {
        return stateBlock_->getAlphaTestFunc();
    }

    void RenderStateRef::setAlphaTestRef(s32 refValue)
    {
        stateBlock_->setAlphaTestRef( (1.0f/255.0f) * refValue );
    }

    s32 RenderStateRef::getAlphaTestRef() const
    {
        return static_cast<s32>( 255.0f * stateBlock_->getAlphaTestRef() );
    }


    void RenderStateRef::setCullMode(CullMode mode)
    {
        stateBlock_->setCullMode(mode);
    }

    CullMode RenderStateRef::getCullMode() const
    {
        return stateBlock_->getCullMode();
    }

    void RenderStateRef::setMultiSampleAlias(bool )
    {
    }

    bool RenderStateRef::getMultiSampleAlias() const
    {
        return false;
    }

    void RenderStateRef::setZEnable(bool enable)
    {
        stateBlock_->set(StateBlock::Bit_ZEnable, enable);
    }

    bool RenderStateRef::getZEnable() const
    {
        return stateBlock_->check(StateBlock::Bit_ZEnable);
    }

    void RenderStateRef::setZWriteEnable(bool enable)
    {
        stateBlock_->set(StateBlock::Bit_ZWriteEnable, enable);
    }

    bool RenderStateRef::getZWriteEnable() const
    {
        return stateBlock_->check(StateBlock::Bit_ZWriteEnable);
    }

    void RenderStateRef::setAlphaBlendEnable(bool enable)
    {
        stateBlock_->set(StateBlock::Bit_AlphaBlendEnable, enable);
    }

    bool RenderStateRef::getAlphaBlendEnable() const
    {
        return stateBlock_->check(StateBlock::Bit_AlphaBlendEnable);
    }

    void RenderStateRef::setAlphaBlend(BlendType src, BlendType dst)
    {
        stateBlock_->setAlphaBlendSrc(src);
        stateBlock_->setAlphaBlendSrc(dst);
    }

    BlendType RenderStateRef::getAlphaBlendSrc() const
    {
        return stateBlock_->getAlphaBlendSrc();
    }

    BlendType RenderStateRef::getAlphaBlendDst() const
    {
        return stateBlock_->getAlphaBlendDst();
    }

}
