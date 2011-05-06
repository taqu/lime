#ifndef INC_APPLICATION_H__
#define INC_APPLICATION_H__
/**
@file Application.h
@author t-sakai
@date 2010/02/14 create
*/
#include <lframework/Application.h>
#include "Scene.h"
#include "TextRenderer.h"

namespace lanim
{
    class AnimationControler;
}

namespace lscene
{
    class AnimObject;
    class Object;
}

namespace pmm
{
    class Loader;
}

namespace viewer
{
    class Application : public lframework::Application
    {
    public:
#if 0
        static const lcore::u32 Width = 1920;
        static const lcore::u32 Height = 1080;
#elif 1
        static const lcore::u32 Width = 800;
        static const lcore::u32 Height = 600;
#else
        static const lcore::u32 Width = 256;
        static const lcore::u32 Height = 128;
#endif

        static const u32 LoadingCountCycleBits = 10;

        enum Status
        {
            Status_Load,
            Status_Play,
        };

        Application();
        ~Application();

        void initialize();
        void update();
        void terminate();

    private:
        Status status_;
        lscene::Object *animObj_;
        lanim::AnimationControler *animControler_;

        viewer::Scene scene_;
        TextRenderer textRenderer_;

        u32 prevMSec_;
        u32 currentMSec_;
        u32 loadingIndex_;
        u32 loadingCounter_;

        pmm::Loader* loader_;
    };

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
#endif //INC_APPLICATION_H__
