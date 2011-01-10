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

    app::Application myApp;

    lgraphics::InitParam param;

    param.backBufferWidth_ = app::Application::Width;
    param.backBufferHeight_ = app::Application::Height;

    param.interval_ = lgraphics::PresentInterval_Immediate;
    param.refreshRate_ = 60;

    if(myApp.create(param, "viewer")){
        myApp.run();
    }
    return 0;
}
