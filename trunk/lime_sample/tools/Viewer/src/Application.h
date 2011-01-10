#ifndef INC_APPLICATION_H__
#define INC_APPLICATION_H__
/**
@file Application.h
@author t-sakai
@date 2010/02/14 create
*/
#include <lframework/Application.h>

namespace lanim
{
    class AnimationControler;
}

namespace lscene
{
    class AnimObject;
}

namespace app
{
    class Application : public lframework::Application
    {
    public:
#if 1
        static const lcore::u32 Width = 400;
        static const lcore::u32 Height = 600;
#else
        static const lcore::u32 Width = 1920;
        static const lcore::u32 Height = 1080;
#endif

        Application();
        ~Application();

        void initialize();
        void update();
        void terminate();

    private:
        lscene::AnimObject *animObj_;
        lanim::AnimationControler *animControler_;
    };
}
#endif //INC_APPLICATION_H__
