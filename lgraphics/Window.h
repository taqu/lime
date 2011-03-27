#ifndef INC_WINDOW_H__
#define INC_WINDOW_H__
/**
@file Window.h
@author t-sakai
@date 2009/04/26 create
*/
#include "lgraphicscore.h"
#include "WindowAPIInclude.h"

namespace lgraphics
{
    class Window
    {
    public:
        struct InitParam
        {
            static const s32 MAX_TITLE_SIZE = 32;
            InitParam()
                :width_(0)
                ,height_(0)
                ,wndProc_(NULL)
                ,windowed_(true)
            {
                title_[0] = '\0';
            }

            void setTitle(const char* title, u32 size);

            u32 width_;
            u32 height_;
            char title_[MAX_TITLE_SIZE];
            WNDPROC wndProc_;
            bool windowed_;
        };

        Window();
        ~Window();

        bool initialize(const InitParam& param, bool show);

        void destroy();

        void terminate();

        bool peekMessage();

        bool getMessage();

        const WindowHandle& getHandle() const{ return handle_;}

        void setShow(bool enable);

 // X Window System用
#if defined(__CYGWIN__) || defined(__linux__)
        bool initialize(const WindowHandle& handle, WNDPROC wndProc);
#endif
    private:
        static const char* CLASS_NAME_;
        WindowHandle handle_;

// Windows用
#ifdef _WIN32
        MSG	msg_;

 // X Window System用
#elif defined(__CYGWIN__) || defined(__linux__)
        void setEventMask();

        XEvent event_;
        WNDPROC wndProc_;
#endif

    };


}

#endif //INC_WINDOW_H__