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

namespace lsound
{
    class Context;
};

namespace lfw
{
    class ECSManager;
    class Renderer;
    class CollideManager;

    class Application
    {
    public:
        struct SoundInitParam
        {
            SoundInitParam()
                :numQueuedBuffers_(3)
                ,maxPlayers_(128)
                ,maxUserPlayers_(4)
                ,waitTime_(30)
                ,masterGain_(1.0f)
            {}

            s32 numQueuedBuffers_;
            s32 maxPlayers_;
            s32 maxUserPlayers_;
            u32 waitTime_;
            f32 masterGain_;
        };

        struct WindowParam
        {
            WindowParam()
                :style_(0)
                ,exStyle_(0)
            {}

            u32 style_;
            u32 exStyle_;
        };

        struct InitParam
        {
            WindowParam windowParam_;
            lgfx::InitParam gfxParam_; //グラフィック関係初期化パラメータ
            linput::InitParam inputParam_;
            ECSInitParam ecsParam_;
            RendererInitParam rendererParam_;
            SoundInitParam soundParam_;
        };

        static Application& getInstance(){ return *instance_;}
        static bool initApplication(InitParam& initParam, const char* title, WNDPROC wndProc);
        static void termApplication();

        /// 処理ループ実行
        void run();

        /// 処理ループ実行
        void runEventDriven();

        void setTerminate();

        inline lgfx::Window& getWindow();
        inline linput::Input& getInput();
        inline Timer& getTimer();
        inline lcore::FileSystem& getFileSystem();
        inline CollideManager& getCollideManager();
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
        CollideManager* collideManager_;
        Renderer* renderer_;
    };

    inline lgfx::Window& Application::getWindow()
    {
        return window_;
    }

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

    inline CollideManager& Application::getCollideManager()
    {
        return *collideManager_;
    }

    inline Renderer& Application::getRenderer()
    {
        return *renderer_;
    }
}
#endif //INC_LFRAMEWORK_APPLICATION_H__
