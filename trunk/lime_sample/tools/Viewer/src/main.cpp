#include "stdafx.h"
#include "Application.h"
#include <lcore/lcore.h>
#include <lgraphics/lgraphics.h>
#include <lgraphics/api/InitParam.h>

int APIENTRY WinMain(HINSTANCE /*hInstance*/,
                       HINSTANCE /*hPrevInstance*/,
                       LPTSTR    /*lpCmdLine*/,
                       int       /*nCmdShow*/)
{

    lcore::System system;

    viewer::Application myApp;

    lgraphics::InitParam param;

    param.backBufferWidth_ = viewer::Application::Width;
    param.backBufferHeight_ = viewer::Application::Height;

    param.interval_ = lgraphics::PresentInterval_Immediate;
    param.refreshRate_ = 60;
    //param.format_ = lgraphics::Display_X8R8G8B8;

    if(myApp.create(param, "viewer", viewer::WndProc)){
        myApp.run();
    }
    return 0;
}
