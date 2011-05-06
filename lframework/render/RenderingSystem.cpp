/**
@file RenderingSystem.cpp
@author t-sakai
@date 2010/11/18 create

*/
#include "lrender.h"
#include "RenderingSystem.h"
#include "../System.h"
#include <lgraphics/lgraphics.h>
#include <lgraphics/api/GraphicsDeviceRef.h>
#include <lgraphics/api/Enumerations.h>

#include "RenderBuffer.h"

namespace lrender
{
    RenderingSystem::RenderingSystem()
        ://renderBufferBatchState_(NULL)
        clearTarget_(lgraphics::ClearTarget_Color|lgraphics::ClearTarget_Depth)
    {
        //renderBufferBatchState_ = LIME_NEW RenderBufferBatchState;
        //renderBufferBatchState_->create();
    }

    RenderingSystem::~RenderingSystem()
    {
        //LIME_DELETE(renderBufferBatchState_);
    }

    void RenderingSystem::initialize()
    {
    }
    
    void RenderingSystem::terminate()
    {
    }

    void RenderingSystem::add(Pass pass, Drawable* drawable)
    {
        passMain_.add(drawable);
    }

    void RenderingSystem::remove(Pass pass, Drawable* drawable)
    {
        passMain_.remove(drawable);
    }

    void RenderingSystem::beginDraw()
    {
        lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();

        device.beginScene();

#if defined(LIME_GLES2)
        device.clearActiveTextures();

        if(clearTarget_ != 0){
            device.setZWriteEnable(true); //Œ³‚É–ß‚³‚È‚¢‚ÆglClear‚àŒø‚©‚È‚¢
            device.clear(clearTarget_);
        }
#else
        device.clear(clearTarget_);
#endif
    }

    void RenderingSystem::draw()
    {
        passMain_.draw();
        //lframework::System::drawDebugString();
    }

    void RenderingSystem::endDraw()
    {
        lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();
        device.endScene();

#if defined(LIME_GLES2)
        device.present();
#else
        device.present(NULL, NULL, NULL);
#endif
    }


#if 0
    bool RenderingSystem::createRenderBuffer(RenderBuffer& buffer, u32 width, u32 height, lgraphics::BufferFormat format)
    {
        //lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();
        //IDirect3DSurface9* surface = device.createRenderTarget(width, height, format, lgraphics::MutiSample_None, 0, false);
        //if(NULL == surface){
        //    return false;
        //}

        //RenderBuffer tmp(surface, renderBufferBatchState_);
        //buffer.swap(tmp);
        //buffer.createGeometry();
        return true;
    }

    bool RenderingSystem::createRenderBufferFromTarget(RenderBuffer& buffer)
    {
        //lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();
        //IDirect3DSurface9 *surface = NULL;
        //if(false == device.getRenderTarget(0, &surface)){
        //    return false;
        //}
        //LASSERT(surface != NULL);

        //RenderBuffer tmp(surface, renderBufferBatchState_);
        //buffer.swap(tmp);
        //buffer.createGeometry();
        return true;
    }
#endif
}
