#ifndef INC_LFRAMEWORK_APPLICATION_H__
#define INC_LFRAMEWORK_APPLICATION_H__
/**
@file Application.h
@author t-sakai
@date 2016/11/06 create
*/
#include <lgraphics/InitParam.h>
#include <lgraphics/Window.h>
#include "input/linput.h"
#include "ecs/ECSInitParam.h"
#include "render/RendererInitParam.h"
#include "Timer.h"
#include <lcore/io/FileSystem.h>

namespace lfw
{
    class ECSManager;
    class Renderer;

    class Application
    {
    public:
        struct InitParam
        {
            lgfx::InitParam gfxParam_; //グラフィック関係初期化パラメータ
            linput::InitParam inputParam_;
            ECSInitParam ecsParam_;
            RendererInitParam rendererParam_;
        };

        static Application& getInstance(){ return *instance_;}
        static bool initApplication(InitParam& initParam, const char* title, WNDPROC wndProc);
        static void termApplication();

        /// 処理ループ実行
        void run();

        /// 処理ループ実行
        void runEventDriven();

        void setTerminate();

        inline linput::Input& getInput();
        inline Timer& getTimer();
        inline lcore::FileSystem& getFileSystem();
        inline Renderer& getRenderer();
        ECSManager& getECSManager();
    protected:
        Application(const Application&);
        Application& operator=(const Application&);

        static Application* instance_;

        Application();
        virtual ~Application();

        /**
        @brief ウィンドウ作成とグラフィックスドライバ初期化
        @return 成否
        @param initParam ... 初期化パラメータ
        */
        bool initInternal(InitParam& initParam, const char* title, WNDPROC wndProc);
        void termInternal();

        virtual void initialize();
        virtual void update();
        virtual void terminate();

        lgfx::Window window_; /// Window

        linput::Input input_;
        Timer timer_;
        lcore::FileSystem fileSystem_;
        Renderer* renderer_;
    };

    inline linput::Input& Application::getInput()
    {
        return input_;
    }

    inline Timer& Application::getTimer()
    {
        return timer_;
    }

    inline lcore::FileSystem& Application::getFileSystem()
    {
        return fileSystem_;
    }

    inline Renderer& Application::getRenderer()
    {
        return *renderer_;
    }
}
#endif //INC_LFRAMEWORK_APPLICATION_H__
