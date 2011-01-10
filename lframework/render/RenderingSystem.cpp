/**
@file RenderingSystem.cpp
@author t-sakai
@date 2010/11/18 create

*/
#include "lrender.h"
#include "RenderingSystem.h"
#include "../System.h"
#include <lgraphics/lgraphics.h>
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

    void RenderingSystem::add(Pass pass, Batch* batch)
    {
        passMain_.add(batch);
    }

    void RenderingSystem::remove(Pass pass, Batch* batch)
    {
        passMain_.remove(batch);
    }

    void RenderingSystem::beginDraw()
    {
        lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();
#if defined(LIME_GLES2)
        device.present();
#else
        device.present(NULL, NULL, NULL);
#endif

        if(clearTarget_ != 0){
            device.clear(clearTarget_);
        }

        device.beginScene();
    }

    void RenderingSystem::draw()
    {
        passMain_.draw();
        lframework::System::drawDebugString();
    }

    void RenderingSystem::endDraw()
    {
        lgraphics::GraphicsDeviceRef& device = lgraphics::Graphics::getDevice();
        device.endScene();
    }
}
