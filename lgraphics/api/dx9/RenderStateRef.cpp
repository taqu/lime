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

    RenderStateRef::RenderStateRef()
        :stateBlock_(NULL)
    {
    }


    RenderStateRef::RenderStateRef(const RenderStateRef& rhs)
        :stateBlock_(rhs.stateBlock_)
    {
        if(stateBlock_ != NULL){
            stateBlock_->AddRef();
        }
    }


    RenderStateRef::~RenderStateRef()
    {
        SAFE_RELEASE(stateBlock_);
    }

    bool RenderStateRef::begin()
    {
        GraphicsDeviceRef &device = Graphics::getDevice();

        return device.beginStateBlock();
    }

    void RenderStateRef::end(RenderStateRef& renderState)
    {
        GraphicsDeviceRef &device = Graphics::getDevice();

        IDirect3DStateBlock9 *stateBlock = NULL;
        device.endStateBlock(&stateBlock);

        RenderStateRef tmp;
        tmp.stateBlock_ = stateBlock;
        renderState.swap(tmp);
    }

    void RenderStateRef::apply()
    {
        HRESULT hr = stateBlock_->Apply();
        if(FAILED(hr)){
            Debug_LogWarn("Cannot apply stateBlock");
        }
    }

     void RenderStateRef::setAlphaTest(bool enable)
     {
         Graphics::getDevice().setRenderState(D3DRS_ALPHATESTENABLE, ((enable)?TRUE : FALSE) );
     }

     bool RenderStateRef::getAlphaTest()
     {
         u32 value = Graphics::getDevice().getRenderState(D3DRS_ALPHATESTENABLE);
         return (value == 0x01U);
     }

     void RenderStateRef::setAlphaTestFunc(CmpFunc func)
     {
         Graphics::getDevice().setRenderState(D3DRS_ALPHAFUNC, func);
     }

     CmpFunc RenderStateRef::getAlphaTestFunc()
     {
         return (CmpFunc)Graphics::getDevice().getRenderState(D3DRS_ALPHAFUNC);
     }

     void RenderStateRef::setAlphaTestRef(s32 refValue)
     {
         Graphics::getDevice().setRenderState(D3DRS_ALPHAREF, refValue);
     }

     u32 RenderStateRef::getAlphaTestRef()
     {
         return Graphics::getDevice().getRenderState(D3DRS_ALPHAREF);
     }

     void RenderStateRef::setCullMode(CullMode mode)
     {
         Graphics::getDevice().setRenderState(D3DRS_CULLMODE, mode);
     }

     CullMode RenderStateRef::getCullMode()
     {
         return (CullMode)Graphics::getDevice().getRenderState(D3DRS_CULLMODE);
     }

     void RenderStateRef::setMultiSampleAlias(bool enable)
     {
         Graphics::getDevice().setRenderState(D3DRS_MULTISAMPLEANTIALIAS, ((enable)?TRUE : FALSE) );
     }

     bool RenderStateRef::getMultiSampleAlias()
     {
         u32 value = Graphics::getDevice().getRenderState(D3DRS_MULTISAMPLEANTIALIAS);
         return (value == 0x01U);
     }

     void RenderStateRef::setZEnable(bool enable)
     {
         Graphics::getDevice().setRenderState(D3DRS_ZENABLE, ((enable)?TRUE : FALSE) );
     }

     bool RenderStateRef::getZEnable()
     {
         u32 value = Graphics::getDevice().getRenderState(D3DRS_ZENABLE);
         return (value == 0x01U);
     }

     void RenderStateRef::setZWriteEnable(bool enable)
     {
         Graphics::getDevice().setRenderState(D3DRS_ZWRITEENABLE, ((enable)?TRUE : FALSE) );
     }

     bool RenderStateRef::getZWriteEnable()
     {
         u32 value = Graphics::getDevice().getRenderState(D3DRS_ZWRITEENABLE);
         return (value == 0x01U);
     }

     void RenderStateRef::setAlphaBlendEnable(bool enable)
     {
         Graphics::getDevice().setRenderState(D3DRS_ALPHABLENDENABLE, ((enable)?TRUE : FALSE) );
     }

     bool RenderStateRef::getAlphaBlendEnable()
     {
         u32 value = Graphics::getDevice().getRenderState(D3DRS_ALPHABLENDENABLE);
         return (value == 0x01U);
     }

     void RenderStateRef::setAlphaBlend(BlendType src, BlendType dst)
     {
         Graphics::getDevice().setRenderState(D3DRS_SRCBLEND, src);
         Graphics::getDevice().setRenderState(D3DRS_DESTBLEND, dst);
     }

     BlendType RenderStateRef::getAlphaBlendSrc()
     {
         return (BlendType)Graphics::getDevice().getRenderState(D3DRS_SRCBLEND);
     }

     

     BlendType RenderStateRef::getAlphaBlendDst()
     {
         return (BlendType)Graphics::getDevice().getRenderState(D3DRS_DESTBLEND);
     }

     void RenderStateRef::setFillMode(FillMode mode)
     {
         Graphics::getDevice().setRenderState(D3DRS_FILLMODE, mode);
     }

     FillMode RenderStateRef::getFillMode()
     {
         return (FillMode)Graphics::getDevice().getRenderState(D3DRS_FILLMODE);
     }

     void RenderStateRef::setViewPort(u32 x, u32 y, u32 width, u32 height, f32 minz, f32 maxz)
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

     void RenderStateRef::getViewPort(u32& x, u32& y, u32& width, u32& height, f32& minz, f32& maxz)
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
}
