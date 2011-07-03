/**
@file SurfaceRef.cpp
@author t-sakai
@date 2009/05/01
*/
#include "../../lgraphics.h"
#include "GraphicsDeviceRef.h"
#include "SurfaceRef.h"

namespace lgraphics
{
    //-----------------------------------------------
    //---
    //--- SurfaceRef
    //---
    //-----------------------------------------------
    SurfaceRef::SurfaceRef(const SurfaceRef& rhs)
        :surface_(rhs.surface_)
    {
        if(surface_ != NULL){
            surface_->AddRef();
        }
    }

    void SurfaceRef::destroy()
    {
        SAFE_RELEASE(surface_);
    }

    void SurfaceRef::attach(u32 index) const
    {
        GraphicsDeviceRef &device = Graphics::getDevice();
        device.setRenderTarget(index, const_cast<IDirect3DSurface9*>(surface_) );
    }

    bool SurfaceRef::getData(SurfaceOffscreenRef& offscreen)
    {
        GraphicsDeviceRef &device = Graphics::getDevice();
        return device.getRenderTargetData(surface_,  offscreen.surface_);
    }

    bool SurfaceRef::getDesc(SurfaceDesc& desc)
    {
        return SUCCEEDED( surface_->GetDesc((D3DSURFACE_DESC*)&desc) );
    }

    //-----------------------------------------------
    //---
    //--- Surface
    //---
    //-----------------------------------------------
    SurfaceRef Surface::create(
            u32 width,
            u32 height,
            BufferFormat format,
            MutiSampleType sampleType,
            u32 multiSampleQuality)
    {
        GraphicsDeviceRef &device = Graphics::getDevice();

        IDirect3DSurface9 *d3dsurface = device.createRenderTarget(
            width,
            height,
            format,
            sampleType,
            multiSampleQuality,
            false);

        return (d3dsurface==NULL)? SurfaceRef(d3dsurface) : SurfaceRef();
    }


    //-----------------------------------------------
    //---
    //--- SurfaceOffscreenRef
    //---
    //-----------------------------------------------
    //-----------------------------------------------
    //---
    //--- SurfaceOffscreen
    //---
    //-----------------------------------------------
    SurfaceOffscreenRef SurfaceOffscreen::create(
        u32 width,
        u32 height,
        BufferFormat format)
    {
        GraphicsDeviceRef &device = Graphics::getDevice();

        IDirect3DSurface9 *d3dsurface = device.createOffscreenPlaneSurface(
            width,
            height,
            format);

        return (d3dsurface != NULL)? SurfaceOffscreenRef( SurfaceRef(d3dsurface) )
            : SurfaceOffscreenRef( SurfaceRef() );
    }
}
