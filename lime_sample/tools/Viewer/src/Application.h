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

        Application();
        ~Application();

        void initialize();
        void update();
        void terminate();

    private:
        lscene::Object *animObj_;
        lanim::AnimationControler *animControler_;

        viewer::Scene scene_;
        TextRenderer textRenderer_;
    };

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
#endif //INC_APPLICATION_H__
