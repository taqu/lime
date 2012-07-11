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




    RenderStateRef::RenderStateRef()
        :flag_(0)
        ,alphaTestFunc_(Cmp_Less)
        //,alphaTestRef_(128)
        ,cullMode_(CullMode_CCW)
        ,alphaBlendSrc_(Blend_InvDestAlpha)
        ,alphaBlendDst_(Blend_DestAlpha)
    {
    }


    RenderStateRef::RenderStateRef(const RenderStateRef& rhs)
        :flag_(rhs.flag_)
        ,alphaTestFunc_(rhs.alphaTestFunc_)
        //,alphaTestRef_(rhs.alphaTestRef_)
        ,cullMode_(rhs.cullMode_)
        ,alphaBlendSrc_(rhs.alphaBlendSrc_)
        ,alphaBlendDst_(rhs.alphaBlendDst_)
    {
    }


    RenderStateRef::~RenderStateRef()
    {
    }


    void RenderStateRef::apply()
    {
        GraphicsDeviceRef& device = Graphics::getDevice();

        u32 enable = check(Bit_ZEnable)? D3DZB_TRUE : D3DZB_FALSE;

        device.setRenderState(D3DRS_ZENABLE, enable);

        device.setRenderState(D3DRS_CULLMODE, cullMode_);

        //アルファブレンド有効、無効の設定はレンダリングパスで行う
        if(check(Bit_AlphaBlendEnable)){
            device.setRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
            device.setRenderState(D3DRS_SRCBLEND, alphaBlendSrc_);
            device.setRenderState(D3DRS_DESTBLEND, alphaBlendDst_);
        }else{
            device.setRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        }

        enable = check(Bit_ZWriteEnable)? TRUE : FALSE;
        device.setRenderState(D3DRS_ZWRITEENABLE, enable );

        enable = check(Bit_AlphaTest)? TRUE : FALSE;
        device.setRenderState(D3DRS_ALPHATESTENABLE, enable);
    }

    //-------------------------------------------------------------------

    void RenderStateRef::setAlphaTest(bool enable)
    {
        set(Bit_AlphaTest, enable);
    }

    bool RenderStateRef::getAlphaTest() const
    {
        return check(Bit_AlphaTest);
    }

    void RenderStateRef::setAlphaTestFunc(CmpFunc func)
    {
        alphaTestFunc_ = func;
    }

    CmpFunc RenderStateRef::getAlphaTestFunc() const
    {
        return alphaTestFunc_;
    }

    //void RenderStateRef::setAlphaTestRef(u32 refValue)
    //{
    //    alphaTestRef_ = refValue;
    //}

    //u32 RenderStateRef::getAlphaTestRef() const
    //{
    //    return alphaTestRef_;
    //}


    void RenderStateRef::setCullMode(CullMode mode)
    {
        LASSERT(mode<=255);
        cullMode_ = (u8)mode;
    }

    CullMode RenderStateRef::getCullMode() const
    {
        return (CullMode)cullMode_;
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
        set(Bit_ZEnable, enable);
    }

    bool RenderStateRef::getZEnable() const
    {
        return check(Bit_ZEnable);
    }

    void RenderStateRef::setZWriteEnable(bool enable)
    {
        set(Bit_ZWriteEnable, enable);
    }

    bool RenderStateRef::getZWriteEnable() const
    {
        return check(Bit_ZWriteEnable);
    }

    void RenderStateRef::setAlphaBlendEnable(bool enable)
    {
        set(Bit_AlphaBlendEnable, enable);
    }

    bool RenderStateRef::getAlphaBlendEnable() const
    {
        return check(Bit_AlphaBlendEnable);
    }

    void RenderStateRef::setAlphaBlend(BlendType src, BlendType dst)
    {
        LASSERT(src<=255);
        LASSERT(dst<=255);

        alphaBlendSrc_ = (u8)src;
        alphaBlendDst_ = (u8)dst;
    }

    BlendType RenderStateRef::getAlphaBlendSrc() const
    {
        return (BlendType)alphaBlendSrc_;
    }

    BlendType RenderStateRef::getAlphaBlendDst() const
    {
        return (BlendType)alphaBlendDst_;
    }

    RenderStateRef& RenderStateRef::operator=(const RenderStateRef& rhs)
    {
        flag_ = rhs.flag_;
        alphaTestFunc_ = rhs.alphaTestFunc_;
        //alphaTestRef_ = rhs.alphaTestRef_;
        cullMode_ = rhs.cullMode_;
        alphaBlendSrc_ = rhs.alphaBlendSrc_;
        alphaBlendDst_ = rhs.alphaBlendDst_;
        return *this;
    }


    void RenderStateRef::swap(RenderStateRef& rhs)
    {
        lcore::swap(flag_, rhs.flag_);
        lcore::swap(alphaTestFunc_, rhs.alphaTestFunc_);
        //lcore::swap(alphaTestRef_, rhs.alphaTestRef_);
        lcore::swap(cullMode_, rhs.cullMode_);
        lcore::swap(alphaBlendSrc_, rhs.alphaBlendSrc_);
        lcore::swap(alphaBlendDst_, rhs.alphaBlendDst_);
    }
}
