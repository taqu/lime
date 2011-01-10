/**
@file Window.cpp
@author t-sakai
@date 2009/04/26
*/
#include "WindowAPIInclude.h"
#include "Window.h"

namespace lgraphics
{
    void Window::InitParam::setTitle(const char* title, u32 size)
    {
        size = ((MAX_TITLE_SIZE-1) < size)? (MAX_TITLE_SIZE-1) : size;
        for(u32 i=0; i<size; ++i){
            title_[i] = title[i];
        }
        title_[size-1] = '\0';
    }

#ifdef _WIN32
    const char* Window::CLASS_NAME_ = "LWINDOW";

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wPara, LPARAM lParam);

    Window::Window()
    {
        handle_.hWnd_ = NULL;
    }

    Window::~Window()
    {
        terminate();
    }

    bool Window::initialize(const InitParam& param, bool show)
    {
        LASSERT(handle_.hWnd_ == NULL);
        LASSERT(param.title_ != NULL);

        HINSTANCE hInstance = GetModuleHandle(0);

        //Window Class 作成・登録
        //-----------------------------------------------------------
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex.lpfnWndProc	= (param.wndProc_ == NULL)? WndProc : param.wndProc_;
        wcex.cbClsExtra		= 0;
        wcex.cbWndExtra		= 0;
        wcex.hInstance		= hInstance;
        wcex.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
        wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground	= NULL;//(HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName	= NULL;
        wcex.lpszClassName	= CLASS_NAME_;
        wcex.hIconSm		= LoadIcon(wcex.hInstance, NULL);

        if(!RegisterClassEx(&wcex)){
            return false;
        }

        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = param.width_;
        rect.bottom = param.height_;

        u32 style = 0;
        u32 exStyle = 0;
        if(param.windowed_){
            exStyle = WS_EX_CLIENTEDGE;
            style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_VISIBLE;
        }else{
            style = WS_POPUP | WS_VISIBLE;
            exStyle = WS_EX_APPWINDOW;
            ShowCursor(FALSE);
        }

        AdjustWindowRectEx(&rect, style, FALSE, exStyle);

        handle_.hWnd_ = CreateWindowEx(
            exStyle,
            CLASS_NAME_,
            param.title_,
            style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            NULL,
            NULL,
            hInstance,
            NULL);

        if(handle_.hWnd_ == NULL){
            UnregisterClass(CLASS_NAME_, hInstance);
            return false;
        }

        if(show){
            ShowWindow(handle_.hWnd_, SW_SHOW);
        }
        UpdateWindow(handle_.hWnd_);
        SetFocus(handle_.hWnd_);
        return true;
    }


    void Window::destroy()
    {
        DestroyWindow(handle_.hWnd_);
    }

    void Window::terminate()
    {
        handle_.hWnd_ = NULL;
        UnregisterClass(CLASS_NAME_, GetModuleHandle(0));
    }


    bool Window::peekMessage()
    {
        while(PeekMessage(&msg_, NULL, 0, 0, PM_REMOVE)){
            if(msg_.message==WM_QUIT)
                return false;
            TranslateMessage(&msg_);
            DispatchMessage(&msg_);
        }

        return true;
    }

    bool Window::getMessage()
    {
        if(GetMessage(&msg_, NULL, 0, 0) > 0){
            TranslateMessage(&msg_);
            DispatchMessage(&msg_);
            return true;
        }

        return false;
    }

    void Window::setShow(bool enable)
    {
        ShowWindow(handle_.hWnd_, (enable)?SW_SHOW : SW_HIDE);
    }

    

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

   //     case WM_CLOSE:
			//DestroyWindow(hWnd);
   //         break;

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


#if defined(__CYGWIN__) || defined(__linux__)
    const char* Window::CLASS_NAME_ = NULL;

    static int WndProc(XEvent& event);

    Window::Window()
        :wndProc_(WndProc)
    {
        handle_.display_ = NULL;
        handle_.window_ = None;
    }

    Window::~Window()
    {
        terminate();
    }

    bool Window::initialize(const InitParam& param)
    {
        LASSERT(handle_.display_ == NULL);
        LASSERT(param.title_ != NULL);
        
        // Xサーバと接続
        Display *display = XOpenDisplay(NULL);
        if(display == NULL){
            return false;
        }

        // ウィンドウ作成
        handle_.display_ = display;
        ::Window window = XCreateWindow(
            display, //display
            DefaultRootWindow(display), //parent window
            0, 0, param.width_, param.height_, //x y w h
            0, // window border width
            CopyFromParent, // color depth
            InputOutput, //InputOnly:入力のみ、InputOutput:入力と表示領域
            DefaultVisual(display, DefaultScreen(display)), //visual
            0, NULL); //visual mask, attributes

        if(window <= 0){
            XCloseDisplay(display);
            handle_.display_ = NULL;
            return false;
        }
        handle_.window_ = window;

        // ウィンドウ名セット
        if(param.title_ != NULL){
            XTextProperty name;
            name.value = reinterpret_cast<unsigned char*>( const_cast<char*>(param.title_) );
            name.encoding = XA_STRING;
            name.format = 8;
            name.nitems = strlen(param.title_);
            XSetWMName(display, window, &name);
        }

        if(param.wndProc_ != NULL){
            wndProc_ = param.wndProc_;
        }

        //
        setEventMask();

        XMapWindow(display, window);
        XRaiseWindow(display, window);

        return true;
    }


    bool Window::initialize(const WindowHandle& handle, WNDPROC wndProc)
    {
        LASSERT(handle_.display_ == NULL);
        handle_ = handle;
        

        if(wndProc != NULL){
            wndProc_ = wndProc;
        }

        //
        setEventMask();

        Display *display = handle_.display_;
        ::Window window = handle_.window_;
        XRaiseWindow(display, window);

        return true;
    }


    void Window::destroy()
    {
        XUnmapWindow(handle_.display_, handle_.window_);
        XDestroyWindow(handle_.display_, handle_.window_);
        handle_.window_ = None;
    }

    void Window::terminate()
    {
        if(handle_.display_ != NULL){
            XCloseDisplay(handle_.display_);
            handle_.display_ = NULL;
        }
    }


    bool Window::peekMessage()
    {
        //イベントがあれば処理する
        while(XPending(handle_.display_)){
            XNextEvent(handle_.display_, &event_);
            if(wndProc_(event_) != 0){
                return false;
            }
        }
        return true;
    }

    bool Window::getMessage()
    {
        XNextEvent(handle_.display_, &event_);
        if(wndProc_(event_) != 0){
            return false;
        }
        return true;
    }

    void Window::setEventMask()
    {
        XSelectInput(handle_.display_, handle_.window_, StructureNotifyMask);
    }


    static int WndProc(XEvent& event)
    {
        switch(event.type)
        {
        case DestroyNotify: //ウィンドウ破棄イベント
            return -1;
            break;

        default:
            break;
        };
        return 0;
    }
#endif
}

