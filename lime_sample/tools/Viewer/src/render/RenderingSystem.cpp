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


namespace lrender
{
    RenderingSystem::RenderingSystem()
        :clearTarget_(lgraphics::ClearTarget_Color|lgraphics::ClearTarget_Depth)
    {
    }

    RenderingSystem::~RenderingSystem()
    {
    }

    void RenderingSystem::initialize()
    {
    }
    
    void RenderingSystem::terminate()
    {
    }

    void RenderingSystem::add(Pass , Drawable* drawable)
    {
        passMain_.add(drawable);
    }

    void RenderingSystem::remove(Pass , Drawable* drawable)
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

}
