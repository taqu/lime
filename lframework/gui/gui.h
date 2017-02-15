#ifndef INC_LFRAMEWORK_GUI_H__
#define INC_LFRAMEWORK_GUI_H__
/**
@file gui.h
@author t-sakai
@date 2017/01/14 create
*/
#include "../lframework.h"

#ifdef LFW_ENABLE_GUI
#include "imgui.h"
#endif

namespace lfw
{
    class GUI
    {
    public:
        static bool initialize();
        static void terminate();
        static bool valid(){return NULL != instance_;}
        static GUI& getInstance(){return *instance_;}

        void begin();
        void render();
    private:
        GUI(const GUI&);
        GUI& operator=(const GUI&);

        class Resource;

        GUI();
        ~GUI();

        bool init();

        static GUI* instance_;

        Resource* resource_;
    };
}
#endif //INC_LFRAMEWORK_GUI_H__
