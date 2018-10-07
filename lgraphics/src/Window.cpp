/**
@file Window.cpp
@author t-sakai
@date 2017/11/03
*/
#include "Window.h"

#ifdef LIME_USE_XCB
#include <xcb/xcb_icccm.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifdef LIME_USE_XLIB
#include <X11/Xatom.h>
#include <string.h>
#endif

namespace lgfx
{
namespace
{
#ifdef LIME_USE_WIN32
    const char* CLASSNAME_ = "LWINDOW";
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wPara, LPARAM lParam);
#endif
}
    Window::Window()
#ifdef LIME_USE_WIN32
        :instance_(NULL)
        ,handle_(NULL)
#endif
#ifdef LIME_USE_XCB
        :connection_(NULL)
        ,handle_(0)
        ,delete_(NULL)
        ,wndProc_(NULL)
#endif
#ifdef LIME_USE_XLIB
        :display_(NULL)
        ,handle_(0)
        ,wndProc_(NULL)
#endif
    {
    }

    Window::~Window()
    {
        destroy();
    }

    bool Window::create(InitParam& param)
    {
        LASSERT(NULL == handle_);
        LASSERT(NULL != param.title_);

#ifdef LIME_USE_WIN32
        instance_ = (NULL != param.instance_)? param.instance_ : GetModuleHandle(0);

        //Create and register window class
        //-----------------------------------------------------------
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex.lpfnWndProc	= (NULL == param.wndProc_)? WndProc : param.wndProc_;
        wcex.cbClsExtra		= 0;
        wcex.cbWndExtra		= 0;
        wcex.hInstance		= instance_;
        wcex.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
        wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground	= NULL;//(HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName	= NULL;
        wcex.lpszClassName	= CLASSNAME_;
        wcex.hIconSm		= LoadIcon(wcex.hInstance, NULL);

        if(!RegisterClassEx(&wcex)){
            return false;
        }

        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = param.width_;
        rect.bottom = param.height_;

        u32 style = param.style_;
        u32 exStyle = param.exStyle_;
        if(param.windowed_){
            exStyle |= WS_EX_CLIENTEDGE;
            style |= WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_VISIBLE;
        }else{
            style |= WS_POPUP | WS_VISIBLE;
            exStyle |= WS_EX_APPWINDOW;
            ShowCursor(FALSE);
        }

        AdjustWindowRectEx(&rect, style, FALSE, exStyle);

        handle_ = CreateWindowEx(
            exStyle,
            CLASSNAME_,
            param.title_,
            style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            NULL,
            NULL,
            instance_,
            NULL);

        if(NULL == handle_){
            UnregisterClass(CLASSNAME_, instance_);
            return false;
        }

        UpdateWindow(handle_);
        SetFocus(handle_);
#endif

#ifdef LIME_USE_XCB
        int screenIndex = 0;
        connection_ = xcb_connect(NULL, &screenIndex);
        if(xcb_connection_has_error(connection_)){
            connection_ = NULL;
            return false;
        }
        const xcb_setup_t* setup = xcb_get_setup(connection_);
        xcb_screen_iterator_t screenIterator = xcb_setup_roots_iterator(setup);
        for(int i=0; i<screenIndex; ++i){
            xcb_screen_next(&screenIterator);
        }
        xcb_screen_t* screen = screenIterator.data;
        handle_ = xcb_generate_id(connection_);

        u32 valueList[] =
        {
            screen->white_pixel,
            XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY,
        };

        xcb_create_window(
            connection_,
            XCB_COPY_FROM_PARENT,
            handle_,
            screen->root,
            -1, -1, param.width_, param.height_,
            0,
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            screen->root_visual,
            XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK,
            valueList);

        wndProc_ = param.wndProc_;

        xcb_flush(connection_);
        xcb_change_property(
            connection_,
            XCB_PROP_MODE_REPLACE,
            handle_,
            XCB_ATOM_WM_NAME,
            XCB_ATOM_STRING,
            8,
            strlen(param.title_),
            param.title_);

        xcb_intern_atom_cookie_t protocols_cookie = xcb_intern_atom(connection_, 1, 12, "WM_PROTOCOLS");
        xcb_intern_atom_reply_t* protocol_reply = xcb_intern_atom_reply(connection_, protocols_cookie, 0);
        xcb_intern_atom_cookie_t delete_cookie = xcb_intern_atom(connection_, 0, 16, "WM_DELETE_WINDOW");
        delete_ = xcb_intern_atom_reply(connection_, delete_cookie, 0);
        xcb_change_property(connection_, XCB_PROP_MODE_REPLACE, handle_, (*protocol_reply).atom, 4, 32, 1, &(*delete_).atom);
        free(protocol_reply);
        
        xcb_size_hints_t hints;
        xcb_icccm_size_hints_set_min_size(&hints, param.width_, param.height_);
        xcb_icccm_size_hints_set_max_size(&hints, param.width_, param.height_);
        xcb_icccm_set_wm_size_hints(connection_, handle_, XCB_ATOM_WM_NORMAL_HINTS, &hints);

        width_ = param.width_;
        height_ = param.height_;
        xcb_map_window(connection_, handle_);
        xcb_flush(connection_);
#endif
#ifdef LIME_USE_XLIB
        display_ = XOpenDisplay(NULL);
        if(NULL == display_){
            return false;
        }
        int default_screen = DefaultScreen(display_);
        handle_ = XCreateWindow(
            display_,
            DefaultRootWindow(display_),
            -1, -1, param.width_, param.height_,
            0, //border width
            CopyFromParent, //color depth
            InputOutput, //
            DefaultVisual(display_, DefaultScreen(display_)),
            0, NULL);
        if(handle_<=0){
            XCloseDisplay(display_);
            display_ = NULL;
            return false;
        }
        wndProc_ = param.wndProc_;

        //------------------------
        XTextProperty name;
        name.value = reinterpret_cast<unsigned char*>(const_cast<char*>(param.title_));
        name.encoding = XA_STRING;
        name.format = 8;
        name.nitems = strlen(param.title_);
        XSetWMName(display_, handle_, &name);

        delete_ = XInternAtom(display_, "WM_DELETE_WINDOW", false);
        XSetWMProtocols(display_, handle_, &delete_, 1);
        XSelectInput(display_, handle_, ExposureMask | StructureNotifyMask);

        XSizeHints hints;
        hints.min_width = param.width_;
        hints.min_height = param.height_;
        hints.max_width = param.width_;
        hints.max_height = param.height_;
        hints.flags = PMinSize | PMaxSize;
        XSetWMNormalHints(display_, handle_, &hints);

        width_ = static_cast<s32>(param.width_);
        height_ = static_cast<s32>(param.height_);
        XClearWindow(display_, handle_);
        XMapWindow(display_, handle_);
#endif
        return true;
    }

    void Window::destroy()
    {
#ifdef LIME_USE_WIN32
        if(NULL != handle_){
            DestroyWindow(handle_);
            handle_ = NULL;
            UnregisterClass(CLASSNAME_, instance_);
            instance_ = NULL;
        }
#endif
#ifdef LIME_USE_XCB
        if(NULL != delete_){
            free(delete_);
            delete_ = NULL;
        }
        if(NULL != connection_){
            wndProc_ = NULL;
            xcb_destroy_window(connection_, handle_);
            xcb_disconnect(connection_);
            handle_ = 0;
            connection_ = NULL;
        }
#endif
#ifdef LIME_USE_XLIB
        if(NULL != display_){
            wndProc_ = NULL;
            XDestroyWindow(display_, handle_);
            XCloseDisplay(display_);
            handle_ = 0;
            display_ = NULL;
        }
#endif
    }

    const Window::Handle Window::getHandle() const
    {
        return handle_;
    }

    Window::Handle Window::getHandle()
    {
        return handle_;
    }

    void Window::setShow(bool show)
    {
#ifdef LIME_USE_WIN32
        ShowWindow(handle_, (show)?SW_SHOW : SW_HIDE);
#endif
#ifdef LIME_USE_XCB
        if(show){
            xcb_map_window(connection_, handle_);
        }else{
            xcb_unmap_window(connection_, handle_);
        }
#endif
#ifdef LIME_USE_XLIB
        if(show){
            XMapWindow(display_, handle_);
        }else{
            XUnmapWindow(display_, handle_);
        }
#endif
    }

    Window::Vector2 Window::getViewSize()
    {
#ifdef LIME_USE_WIN32
        RECT rect;
        s32 width, height;
        if(TRUE == GetClientRect(handle_, &rect)){
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }else{
            width = 1;
            height = 1;
        }
        return {width, height};
#endif
#ifdef LIME_USE_XCB
        xcb_get_geometry_cookie_t cookie = xcb_get_geometry(connection_, handle_);
        xcb_get_geometry_reply_t* geom = xcb_get_geometry_reply(connection_, cookie, NULL);
        s32 width = geom->width - geom->border_width*2;
        s32 height = geom->height - geom->border_width*2;
        free(geom);
        return {width, height};
#endif
#ifdef LIME_USE_XLIB
        s32 x;
        s32 y;
        s32 width=1;
        s32 height=1;
        ::Window root;
        u32 border;
        u32 depth;
        XGetGeometry(display_, handle_,
            &root,
            &x, &y, reinterpret_cast<u32*>(&width), reinterpret_cast<u32*>(&height),
            &border, //border
            &depth);//depth
        return {width, height};
#endif
    }

    bool Window::peekEvent()
    {
#ifdef LIME_USE_WIN32
        while(PeekMessage(&event_, NULL, 0, 0, PM_REMOVE)){
            if(event_.message==WM_QUIT)
                return false;
            TranslateMessage(&event_);
            DispatchMessage(&event_);
        }
        return true;
#endif
#ifdef LIME_USE_XCB
        for(;;){
            event_ = xcb_poll_for_event(connection_);
            if(NULL == event_){
                return true;
            }
            switch(event_->response_type & 0x7F){
            case XCB_CLIENT_MESSAGE:
                if((*(xcb_client_message_event_t*)event_).data.data32[0] == (*delete_).atom){
                    return false;
                }
                break;
            }
            if(NULL != wndProc_){
                if(!wndProc_(event_)){
                    return false;
                }
            }

        } //for(;;)
#endif
#ifdef LIME_USE_XLIB
        while(XPending(display_)){
            XNextEvent(display_, &event_);
            switch(event_.type){
            case ConfigureNotify:
                break;
            case DestroyNotify:
                return false;
            case ClientMessage:
                if(static_cast<u32>(event_.xclient.data.l[0]) == delete_){
                    return false;
                }
                break;
            }
            if(NULL != wndProc_){
                if(!wndProc_(event_)){
                    return false;
                }
            }
        } //while(XPending
        return true;
#endif
    }


#ifdef LIME_USE_WIN32
    HINSTANCE Window::getInstance()
    {
        return instance_;
    }

    bool Window::peekEvent(HWND hDlg)
    {
        while(PeekMessage(&event_, NULL, 0, 0, PM_REMOVE)){
            if(event_.message==WM_QUIT)
                return false;

            if(NULL != hDlg && IsDialogMessage(hDlg, &event_)){
                continue;
            }

            TranslateMessage(&event_);
            DispatchMessage(&event_);
        }
        return true;
    }

    bool Window::getEvent(HWND hDlg)
    {
        BOOL result;
        while(0 != (result = GetMessage(&event_, NULL, 0, 0))){
            if(result<0){
                return false;
            }
            if(event_.message==WM_QUIT)
                return false;

            if(NULL != hDlg && IsDialogMessage(hDlg, &event_)){
                continue;
            }

            TranslateMessage(&event_);
            DispatchMessage(&event_);
        }
        return true;
    }

#endif

#ifdef LIME_USE_XCB
    xcb_connection_t* Window::getConnection()
    {
        return connection_;
    }
#endif

#ifdef LIME_USE_XLIB
    Display* Window::getDisplay()
    {
        return display_;
    }
#endif

    namespace
    {
#ifdef LIME_USE_WIN32
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            switch(msg)
            {
            case WM_SYSCOMMAND:
            {
                switch(wParam)
                {
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                    return 0;
                }
                return DefWindowProc(hWnd, msg, wParam, lParam);
            }
            break;

            //case WM_CLOSE:
            //    DestroyWindow(hWnd);
            //    break;

            case WM_PAINT:
            {
                ValidateRect(hWnd, NULL);
            }

            break;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 1;
                break;

            default:
                return DefWindowProc(hWnd, msg, wParam, lParam);
                break;
            }
            return 0;
        }
#endif
    }
}
