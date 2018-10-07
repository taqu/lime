#ifndef INC_LGRAPHICS_WINDOW_H_
#define INC_LGRAPHICS_WINDOW_H_
/**
@file Window.h
@author t-sakai
@date 2017/11/03
*/
#include "lgraphics.h"

#ifndef _MSC_VER
#include <cstdint>
#endif

#ifdef _MSC_VER
#define LIME_USE_WIN32
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
#define LIME_USE_XLIB
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
#define LIME_USE_XCB
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
#define LIME_USE_WIN32
#endif

#ifdef LIME_USE_XLIB
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef LIME_USE_XCB
#include <xcb/xcb.h>
#endif

#ifdef LIME_USE_WIN32
#include <Windows.h>
#endif

#ifndef NULL
#   ifdef __cplusplus
#       if 201103L<=__cplusplus || 1900<=_MSC_VER
#           define NULL nullptr
#       else
#           define NULL 0
#       endif
#   else
#       define NULL (void*)0
#   endif
#endif

namespace lgfx
{
//#ifdef _MSC_VER
//    typedef __int16 s16;
//    typedef __int32 s32;
//
//    typedef unsigned __int16 u16;
//    typedef unsigned __int32 u32;
//#else
//    typedef int16_t s16;
//    typedef int32_t s32;
//
//    typedef uint16_t u16;
//    typedef uint32_t u32;
//#endif

    class Window
    {
    public:
#ifdef LIME_USE_WIN32
        typedef MSG Event;
        typedef HWND Handle;

        struct InitParam
        {
            HINSTANCE instance_;
            u32 width_;
            u32 height_;
            u32 style_;
            u32 exStyle_;
            WNDPROC wndProc_;
            const char* title_;
            bool windowed_;
        };
#endif
#ifdef LIME_USE_XCB
        typedef xcb_generic_event_t* Event;
        typedef xcb_window_t Handle;
        typedef bool (*WNDPROC)(Event);

        struct InitParam
        {
            u16 width_;
            u16 height_;
            WNDPROC wndProc_;
            const char* title_;
        };
#endif
#ifdef LIME_USE_XLIB
        typedef XEvent Event;
        typedef ::Window Handle;
        typedef bool (*WNDPROC)(Event);
        struct InitParam
        {
            u32 width_; 
            u32 height_;
            WNDPROC wndProc_;
            const char* title_;
        };
#endif

#ifdef LIME_USE_WIN32
        HINSTANCE getInstance();
        bool peekEvent(HWND hDlg);
        bool getEvent(HWND hDlg);
#endif
#ifdef LIME_USE_XCB
        xcb_connection_t* getConnection();
#endif
#ifdef LIME_USE_XLIB
        Display* getDisplay();
#endif

        struct Vector2
        {
            s32 x_;
            s32 y_;
        };

        Window();
        ~Window();

        bool create(InitParam& param);
        void destroy();
        const Handle getHandle() const;
        Handle getHandle();
        void setShow(bool show);
        Vector2 getViewSize();

        bool peekEvent();
    private:
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

#ifdef LIME_USE_WIN32
        HINSTANCE instance_;
        Handle handle_;
#endif
#ifdef LIME_USE_XCB
        xcb_connection_t* connection_;
        Handle handle_;
        xcb_intern_atom_reply_t* delete_;
        u16 width_;
        u16 height_;
        WNDPROC wndProc_;
#endif
#ifdef LIME_USE_XLIB
        Display* display_;
        Handle handle_;
        Atom delete_;
        s32 width_;
        s32 height_;
        WNDPROC wndProc_;
#endif
        Event event_;
    };
}
#endif //INC_LGRAPHICS_WINDOW_H_

