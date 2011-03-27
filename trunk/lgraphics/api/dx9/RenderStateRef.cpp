/**
@file RenderStateRef.cpp
@author t-sakai
@date 2009/01/19 create
*/
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"
#include "RenderStateRef.h"


using namespace lcore;

namespace lgraphics
{

    void RenderState::setAlphaTest(bool enable)
    {
        Graphics::getDevice().setRenderState(D3DRS_ALPHATESTENABLE, ((enable)?TRUE : FALSE) );
    }

    bool RenderState::getAlphaTest()
    {
        u32 value = Graphics::getDevice().getRenderState(D3DRS_ALPHATESTENABLE);
        return (value == 0x01U);
    }

    void RenderState::setAlphaTestFunc(CmpFunc func)
    {
        Graphics::getDevice().setRenderState(D3DRS_ALPHAFUNC, func);
    }

    CmpFunc RenderState::getAlphaTestFunc()
    {
        return (CmpFunc)Graphics::getDevice().getRenderState(D3DRS_ALPHAFUNC);
    }

    void RenderState::setAlphaTestRef(u32 refValue)
    {
        Graphics::getDevice().setRenderState(D3DRS_ALPHAREF, refValue);
    }

    u32 RenderState::getAlphaTestRef()
    {
        return Graphics::getDevice().getRenderState(D3DRS_ALPHAREF);
    }

    void RenderState::setCullMode(CullMode mode)
    {
        Graphics::getDevice().setRenderState(D3DRS_CULLMODE, mode);
    }

    CullMode RenderState::getCullMode()
    {
        return (CullMode)Graphics::getDevice().getRenderState(D3DRS_CULLMODE);
    }

    void RenderState::setMultiSampleAlias(bool enable)
    {
        Graphics::getDevice().setRenderState(D3DRS_MULTISAMPLEANTIALIAS, ((enable)?TRUE : FALSE) );
    }

    bool RenderState::getMultiSampleAlias()
    {
        u32 value = Graphics::getDevice().getRenderState(D3DRS_MULTISAMPLEANTIALIAS);
        return (value == 0x01U);
    }

    void RenderState::setZEnable(bool enable)
    {
        Graphics::getDevice().setRenderState(D3DRS_ZENABLE, ((enable)?TRUE : FALSE) );
    }

    bool RenderState::getZEnable()
    {
        u32 value = Graphics::getDevice().getRenderState(D3DRS_ZENABLE);
        return (value == 0x01U);
    }

    void RenderState::setZWriteEnable(bool enable)
    {
        Graphics::getDevice().setRenderState(D3DRS_ZWRITEENABLE, ((enable)?TRUE : FALSE) );
    }

    bool RenderState::getZWriteEnable()
    {
        u32 value = Graphics::getDevice().getRenderState(D3DRS_ZWRITEENABLE);
        return (value == 0x01U);
    }

    void RenderState::setAlphaBlendEnable(bool enable)
    {
        Graphics::getDevice().setRenderState(D3DRS_ALPHABLENDENABLE, ((enable)?TRUE : FALSE) );
    }

    bool RenderState::getAlphaBlendEnable()
    {
        u32 value = Graphics::getDevice().getRenderState(D3DRS_ALPHABLENDENABLE);
        return (value == 0x01U);
    }

    void RenderState::setAlphaBlend(BlendType src, BlendType dst)
    {
        Graphics::getDevice().setRenderState(D3DRS_SRCBLEND, src);
        Graphics::getDevice().setRenderState(D3DRS_DESTBLEND, dst);
    }

    BlendType RenderState::getAlphaBlendSrc()
    {
        return (BlendType)Graphics::getDevice().getRenderState(D3DRS_SRCBLEND);
    }



    BlendType RenderState::getAlphaBlendDst()
    {
        return (BlendType)Graphics::getDevice().getRenderState(D3DRS_DESTBLEND);
    }

    void RenderState::setFillMode(FillMode mode)
    {
        Graphics::getDevice().setRenderState(D3DRS_FILLMODE, mode);
    }

    FillMode RenderState::getFillMode()
    {
        return (FillMode)Graphics::getDevice().getRenderState(D3DRS_FILLMODE);
    }

    void RenderState::setViewPort(u32 x, u32 y, u32 width, u32 height, f32 minz, f32 maxz)
    {
        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();

        D3DVIEWPORT9 viewport;
        viewport.X = x;
        viewport.Y = y;
        viewport.Width = width;
        viewport.Height = height;
        viewport.MinZ = minz;
        viewport.MaxZ = maxz;

        d3ddevice->SetViewport(&viewport);
    }

    void RenderState::getViewPort(u32& x, u32& y, u32& width, u32& height, f32& minz, f32& maxz)
    {
        IDirect3DDevice9 *d3ddevice = Graphics::getDevice().getD3DDevice();

        D3DVIEWPORT9 viewport;
        d3ddevice->GetViewport(&viewport);

        x = viewport.X;
        y = viewport.Y;
        width = viewport.Width;
        height = viewport.Height;
        minz = viewport.MinZ;
        maxz = viewport.MaxZ;
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
        u32 enable = check(Bit_ZEnable)? TRUE : FALSE;

        Graphics::getDevice().setRenderState(D3DRS_ZENABLE, enable );

        CullMode cullMode = check(Bit_CullingEnable)? CullMode_CCW : CullMode_None;

        Graphics::getDevice().setRenderState(D3DRS_CULLMODE, cullMode);

        //アルファブレンド有効、無効の設定はレンダリングパスで行う
        if(check(Bit_AlphaBlendEnable)){
            RenderState::setAlphaBlend(alphaBlendSrc_, alphaBlendDst_);
        }

        enable = check(Bit_ZWriteEnable)? TRUE : FALSE;
        Graphics::getDevice().setRenderState(D3DRS_ZWRITEENABLE, enable );
    }



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
