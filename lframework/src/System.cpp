/**
@file System.cpp
@author t-sakai
@date 2017/06/18 create
*/
#include "System.h"

namespace lfw
{
    Application* System::application_ = NULL;
    lgfx::Window* System::window_ = NULL;
    linput::Input* System::input_ = NULL;
    Timer* System::timer_ = NULL;
    lcore::FileSystem* System::fileSystem_ = NULL;
    Resources* System::resources_ = NULL;
    CollideManager* System::collideManager_ = NULL;
    Renderer* System::renderer_ = NULL;
    ECSManager* System::ecsManager_ = NULL;
    graph::RenderGraph* System::renderGraph_ = NULL;

    void System::clear()
    {
        application_ = NULL;
        window_ = NULL;
        input_ = NULL;
        resources_ = NULL;
        timer_ = NULL;
        fileSystem_ = NULL;
        collideManager_ = NULL;
        renderer_ = NULL;
        ecsManager_ = NULL;
        renderGraph_ = NULL;
    }

    void System::setApplication(Application* application)
    {
        LASSERT(NULL == application_);
        application_ = application;
    }

    void System::setWindow(lgfx::Window* window)
    {
        LASSERT(NULL == window_);
        window_ = window;
    }

    void System::setInput(linput::Input* input)
    {
        LASSERT(NULL == input_);
        input_ = input;
    }

    void System::setTimer(Timer* timer)
    {
        LASSERT(NULL == timer_);
        timer_ = timer;
    }

    void System::setFileSystem(lcore::FileSystem* fileSystem)
    {
        LASSERT(NULL == fileSystem_);
        fileSystem_ = fileSystem;
    }

    void System::setResources(Resources* resources)
    {
        LASSERT(NULL == resources_);
        resources_ = resources;
    }

    void System::setCollideManager(CollideManager* collideManager)
    {
        LASSERT(NULL == collideManager_);
        collideManager_ = collideManager;
    }

    void System::setRenderer(Renderer* renderer)
    {
        LASSERT(NULL == renderer_);
        renderer_ = renderer;
    }

    void System::setECSManager(ECSManager* ecsManager)
    {
        LASSERT(NULL == ecsManager_);
        ecsManager_ = ecsManager;
    }

    void System::setRenderGraph(graph::RenderGraph* renderGraph)
    {
        LASSERT(NULL == renderGraph_);
        renderGraph_ = renderGraph;
    }
}
