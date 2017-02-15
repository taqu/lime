#include <lframework/Application.h>
#include <lgraphics/Display.h>
#include <lframework/ecs/ECSManager.h>
#include <lframework/resource/Resources.h>
#include "Scene00.h"

INT WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, INT /*nCmdShow*/)
{
    lgfx::DXGIDisplayMode mode;
    bool windowed = true;
    lgfx::DXGIDisplayMode request = {0};
    request.width_ = 800;
    request.height_ = 600;
    request.format_ = lgfx::Data_R8G8B8A8_UNorm_SRGB;
    request.refreshRate_ = 60;
    //lgfx::getDisplayMode(hInstance, mode, windowed, request);
    {
        lfw::Application::InitParam initParam;
        initParam.inputParam_.initDevices_[0] = true;
        initParam.inputParam_.initDevices_[1] = true;
        initParam.inputParam_.initDevices_[2] = true;
        initParam.soundParam_.masterGain_ = 0.1f;
        initParam.rendererParam_.numSyncFrames_ = 2;
        initParam.rendererParam_.shadowMapZFar_ = 50.0f;

        if(!lfw::Application::initApplication(initParam, "Empty", NULL)){
            return 0;
        }
        lfw::Application& application = lfw::Application::getInstance();
        application.getFileSystem().mountOS(0, "data");
        lfw::Resources::getInstance().getFontManager().load(0, "mplus.fnt");
        lfw::Entity sceneEntity = application.getECSManager().requestCreateGeometric("Scene00");
        sceneEntity.addComponent<debug::Scene00>();
        application.run();
        lfw::Application::termApplication();
    }
    return 0;
}