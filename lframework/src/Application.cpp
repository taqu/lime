/**
@file Application.cpp
@author t-sakai
@date 2016/11/06 create
*/
#include "Application.h"
#include <lcore/io/FileSystem.h>
#include <lgraphics/Graphics.h>
#include <lsound/Context.h>
#include "System.h"
#include "ecs/ECSManager.h"
#include "physics/CollideManager.h"
#include "render/Renderer.h"
#include "resource/Resources.h"
#include "render/graph/RenderGraph.h"

#include "render/graph/RenderPassGBuffer.h"

namespace lfw
{
    bool Application::initApplication(InitParam& initParam, const char* title, WNDPROC wndProc)
    {
        Application* instance = LNEW Application;
        if(!instance->initInternal(initParam, title, wndProc)){
            LDELETE(instance);
            return false;
        }
        return true;
    }

    void Application::termApplication()
    {
        Application* application = &System::getApplication();
        application->termInternal();
        LDELETE(application);
    }


    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    // �������[�v���s
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

    // �������[�v���s
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
        System::setApplication(this);

        //Initialize Window
        //-----------------------------------------------------------------
        //�E�B���h�E�T�C�Y�A�r���[�|�[�g�T�C�Y�̓o�b�N�o�b�t�@�Ɠ����ɂ���B
        //�f�o�C�X�̃p�����[�^�ɃE�B���h�E�n���h����n��
        lgfx::Window::InitParam windowParam;
        windowParam.style_ = initParam.windowParam_.style_;
        windowParam.exStyle_ = initParam.windowParam_.exStyle_;
        windowParam.width_ = initParam.gfxParam_.backBufferWidth_;
        windowParam.height_ = initParam.gfxParam_.backBufferHeight_;
        windowParam.setTitle(title, static_cast<u32>(lcore::strlen(title)+1));
        windowParam.wndProc_ = wndProc;
        bool ret = window_.initialize(windowParam, true);
        if(false == ret){
            return false;
        }
        System::setWindow(&window_);

        //Initialize Graphics
        //-----------------------------------------------------------------
        initParam.gfxParam_.windowHandle_ = window_.getHandle().hWnd_;
#if defined(_WIN32)
        ImmAssociateContext(initParam.gfxParam_.windowHandle_, NULL); //IME OFF
#endif
        if(false == lgfx::initializeGraphics(initParam.gfxParam_)){
            window_.terminate();
            return false;
        }

        //Initialize Input
        //-----------------------------------------------------------------
        initParam.inputParam_.windowHandle_ = initParam.gfxParam_.windowHandle_;
        if(linput::Error_None != input_.initialize(initParam.inputParam_)){
            return false;
        }
        System::setInput(&input_);

        //Initialize Timer
        //-----------------------------------------------------------------
        timer_.reset();
        System::setTimer(&timer_);

        //Initialize FileSystem
        //-----------------------------------------------------------------
        lcore::FileSystem* fileSystem = LNEW lcore::FileSystem();
        System::setFileSystem(fileSystem);

        //Initialize Sound
        //-----------------------------------------------------------------
        lsound::Context::InitParam soundInitParam;
        soundInitParam.numQueuedBuffers_ = initParam.soundParam_.numQueuedBuffers_;
        soundInitParam.maxPlayers_ = initParam.soundParam_.maxPlayers_;
        soundInitParam.maxUserPlayers_ = initParam.soundParam_.maxUserPlayers_;
        soundInitParam.waitTime_ = initParam.soundParam_.waitTime_;
        if(!lsound::Context::initialize(soundInitParam)){
            return false;
        }
        lsound::Context& context = lsound::Context::getInstance();
        context.setGain(initParam.soundParam_.masterGain_);

        //Initialize Resources
        //-----------------------------------------------------------------
        Resources* resources = Resources::create(4, fileSystem);
        resources->getInputLayoutFactory().loadDefaults();
        resources->getShaderManager().loadDefaultShaders();

        if(initParam.rendererParam_.renderWidth_<=0){
            initParam.rendererParam_.renderWidth_ = initParam.gfxParam_.backBufferWidth_;
        }
        if(initParam.rendererParam_.renderHeight_<=0){
            initParam.rendererParam_.renderHeight_ = initParam.gfxParam_.backBufferHeight_;
        }
        System::setResources(resources);

        //Initialize CollideManager
        //-----------------------------------------------------------------
        CollideManager* collideManager = LNEW CollideManager();
        System::setCollideManager(collideManager);

        //Initialize Renderer
        //-----------------------------------------------------------------
        initParam.rendererParam_.shadowMapFormat_ = initParam.gfxParam_.shadowMapFormat_;
        Renderer* renderer = LNEW Renderer();
        renderer->initialize(initParam.rendererParam_);
        System::setRenderer(renderer);

        //Initialize ECSManager
        //-----------------------------------------------------------------
        ECSManager* ecsManager = ECSManager::create(initParam.ecsParam_);
        if(NULL == ecsManager){
            return false;
        }
        System::setECSManager(ecsManager);

        //Initialize RenderGraph
        //-----------------------------------------------------------------
        graph::RenderGraph* renderGraph = LNEW graph::RenderGraph();
        System::setRenderGraph(renderGraph);

        return true;
    }

    void Application::termInternal()
    {
        ECSManager* ecsManager = &System::getECSManager();
        ECSManager::destroy(ecsManager);

        Renderer* renderer = &System::getRenderer();
        renderer->terminate();
        LDELETE(renderer);

        CollideManager* collideManager = &System::getCollideManager();
        LDELETE(collideManager);

        Resources* resources = &System::getResources();
        Resources::destroy(resources);

        lsound::Context::terminate();

        lcore::FileSystem* fileSystem = &System::getFileSystem();
        LDELETE(fileSystem);

        input_.terminate();

        lgfx::terminateGraphics();
        window_.terminate();

        System::clear();
    }

    // ������
    void Application::initialize()
    {
    }

    // �X�V
    void Application::update()
    {
        input_.update();
        timer_.update();
        lsound::Context::getInstance().updateRequests();

        Renderer& renderer = System::getRenderer();
        ECSManager& ecsManager = System::getECSManager();
        CollideManager& collideManager = System::getCollideManager();

        renderer.begin();
        ecsManager.update();
        collideManager.collideAll();
        ecsManager.postUpdate();
        renderer.update();
    }

    // �I��
    void Application::terminate()
    {
    }

}
