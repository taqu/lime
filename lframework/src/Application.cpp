/**
@file Application.cpp
@author t-sakai
@date 2016/11/06 create
*/
#include "Application.h"
#include <lgraphics/Graphics.h>
#include "ecs/ECSManager.h"
#include "render/Renderer.h"
#include "resource/Resources.h"

namespace lfw
{
    Application* Application::instance_ = NULL;

    bool Application::initApplication(InitParam& initParam, const char* title, WNDPROC wndProc)
    {
        LDELETE(instance_);
        instance_ = LNEW Application;
        if(!instance_->initInternal(initParam, title, wndProc)){
            LDELETE(instance_);
            return false;
        }
        return true;
    }

    void Application::termApplication()
    {
        if(NULL != instance_){
            instance_->termInternal();
            LDELETE(instance_);
        }
    }


    Application::Application()
        :renderer_(NULL)
    {
    }

    Application::~Application()
    {
    }

    // 処理ループ実行
    void Application::run()
    {
        initialize();

        lgfx::Window::EVENT ev;
        for(;;){
            if(false == window_.peekEvent(ev)){
                break;
            }
            update();
        }

        terminate();
    }

    // 処理ループ実行
    void Application::runEventDriven()
    {
        initialize();

        lgfx::Window::EVENT ev;
        for(;;){
            if(false == window_.getEvent(ev)){
                break;
            }
            update();
        }

        terminate();
    }

    void Application::setTerminate()
    {
        window_.destroy();
    }

    bool Application::initInternal(InitParam& initParam, const char* title, WNDPROC wndProc)
    {
        //ウィンドウサイズ、ビューポートサイズはバックバッファと同じにする。

        //ウィンドウ作成。デバイスのパラメータにウィンドウハンドルを渡す
        lgfx::Window::InitParam windowParam;
        windowParam.width_ = initParam.gfxParam_.backBufferWidth_;
        windowParam.height_ = initParam.gfxParam_.backBufferHeight_;
        windowParam.setTitle(title, static_cast<u32>(lcore::strlen(title)+1));
        windowParam.wndProc_ = wndProc;
        bool ret = window_.initialize(windowParam, true);
        if(false == ret){
            return false;
        }

        initParam.gfxParam_.windowHandle_ = window_.getHandle().hWnd_;

#if defined(_WIN32)
        ImmAssociateContext(initParam.gfxParam_.windowHandle_, NULL); //IME OFF
#endif
        if(false == lgfx::initializeGraphics(initParam.gfxParam_)){
            window_.terminate();
            return false;
        }

        if(!ECSManager::create(initParam.ecsParam_)){
            return false;
        }
        initParam.inputParam_.windowHandle_ = initParam.gfxParam_.windowHandle_;
        if(linput::Error_None != input_.initialize(initParam.inputParam_)){
            return false;
        }

        timer_.reset();
        Resources::initialize(4, &fileSystem_);
        Resources::getInstance().getInputLayoutFactory().loadDefaults();
        Resources::getInstance().getShaderManager().loadDefaultShaders();
        ECSManager::getInstance().initialize();

        renderer_ = LNEW Renderer();
        renderer_->initialize(initParam.rendererParam_);
        return true;
    }

    void Application::termInternal()
    {
        renderer_->terminate();
        LDELETE(renderer_);
        ECSManager::getInstance().terminate();
        Resources::terminate();
        input_.terminate();
        ECSManager::destroy();

        lgfx::terminateGraphics();
        window_.terminate();
    }

    // 初期化
    void Application::initialize()
    {
    }

    // 更新
    void Application::update()
    {
        input_.update();
        timer_.update();
        ECSManager::getInstance().update();
        renderer_->update();
    }

    // 終了
    void Application::terminate()
    {
    }

    ECSManager& Application::getECSManager()
    {
        return ECSManager::getInstance();
    }
}
