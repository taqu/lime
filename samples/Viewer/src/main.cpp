/**
@file main.cpp
@author t-sakai
@date 2017/01/17 create
*/
#include <Windows.h>
#include <lframework/System.h>
#include <lframework/Application.h>
#include <lframework/ecs/ECSManager.h>
#include <lframework/ecs/ComponentGeometric.h>
#include <lframework/ecs/ComponentLogical.h>
#include "Viewer.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

INT WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, INT /*nCmdShow*/)
{
    {
        lfw::Application::InitParam initParam;
        initParam.windowParam_.style_ = 0;
        initParam.windowParam_.exStyle_ = WS_EX_ACCEPTFILES;
        initParam.gfxParam_.backBufferWidth_ = 1024;
        initParam.gfxParam_.backBufferHeight_ = 768;
        initParam.gfxParam_.format_ = lgfx::Data_R8G8B8A8_UNorm_SRGB;
        initParam.gfxParam_.refreshRate_ = 30;
        initParam.inputParam_.initDevices_[0] = true;
        initParam.inputParam_.initDevices_[1] = true;
        initParam.inputParam_.initDevices_[2] = true;
        initParam.soundParam_.masterGain_ = 0.1f;
        initParam.rendererParam_.numSyncFrames_ = 3;

        if(!lfw::Application::initApplication(initParam, "Alpha", WndProc)){
            return 0;
        }
        lfw::Application& application = lfw::System::getApplication();
        {
            lfw::System::getFileSystem().mountOS(0, "data");

            lfw::Entity viewerEntity = lfw::System::getECSManager().requestCreateGeometric("Viewer");
            viewerEntity.addComponent<viewer::Viewer>();
        }

        application.run();
        lfw::Application::termApplication();
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

    //case WM_CREATE:
    //    DragAcceptFiles(hWnd, TRUE);
    //    break;

    case WM_DROPFILES:
    {
        lcore::Char filepath[256];
        HDROP hDrop = (HDROP)wParam;
        UINT  num = DragQueryFile(hDrop, (UINT)-1, NULL, 0);
        for(lcore::u32 i=0; i<num; ++i){
            DragQueryFile(hDrop, i, filepath, sizeof(filepath));
            lcore::replace(filepath, '/', '\\');
            lfw::ComponentGeometric* geomViewer = lfw::ComponentGeometric::find("/Viewer");
            if(NULL != geomViewer){
                viewer::Viewer* viewer = geomViewer->getEntity().getComponent<viewer::Viewer>();
                viewer->setModelPath(filepath);
            }
        }
        DragFinish(hDrop);
    }
        break;

    case WM_PAINT:
    {
        ValidateRect(hWnd, NULL);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 1;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
        break;
    }
    return 0;
}
