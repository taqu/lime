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

namespace lsound
{
    class Context;
};

namespace lfw
{
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
            lgfx::InitParam gfxParam_; //�O���t�B�b�N�֌W�������p�����[�^
            linput::InitParam inputParam_;
            ECSInitParam ecsParam_;
            RendererInitParam rendererParam_;
            SoundInitParam soundParam_;
        };

        static bool initApplication(InitParam& initParam, const char* title, WNDPROC wndProc);
        static void termApplication();

        /// �������[�v���s
        void run();

        /// �������[�v���s
        void runEventDriven();

        void setTerminate();

    protected:
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        Application();
        virtual ~Application();

        /**
        @brief �E�B���h�E�쐬�ƃO���t�B�b�N�X�h���C�o������
        @return ����
        @param initParam ... �������p�����[�^
        */
        bool initInternal(InitParam& initParam, const char* title, WNDPROC wndProc);
        void termInternal();

        virtual void initialize();
        virtual void update();
        virtual void terminate();

        lgfx::Window window_; /// Window

        linput::Input input_;
        Timer timer_;
    };
}
#endif //INC_LFRAMEWORK_APPLICATION_H__
