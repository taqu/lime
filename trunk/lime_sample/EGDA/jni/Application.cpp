#include "stdafx.h"
#include "Application.h"
#include "Scene.h"

#include <lcore/liostream.h>
#include <lgraphics/lgraphics.h>
#include <lgraphics/api/InitParam.h>
#include <lgraphics/api/Enumerations.h>

#include <Pmm.h>

#include <lframework/System.h>
#include <lframework/anim/AnimationSystem.h>
#include <lframework/render/RenderingSystem.h>

#include <lframework/scene/Scene.h>
#include <lframework/scene/Camera.h>
#include <lframework/scene/LightEnvironment.h>

#include "Input.h"
#include "Config.h"

#include <time.h>
#include <unistd.h>

namespace
{
    //デバッグ用ログ
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
        //lconverter::DebugLog g_debugLog;
#endif
}

namespace egda
{
    Application::Application()
        :prevMSec_(0)
        ,currentMSec_(0)
    {
        Config::initialize();
    }

    Application::~Application()
    {
    }

    //----------------------------------------------
    bool Application::initialize(const Char* textTexture, u32 size)
    {
        lgraphics::InitParam param;
        param.backBufferWidth_ = 300;
        param.backBufferHeight_ = 400;

        if(false == lgraphics::Graphics::initialize(param)){
            return false;
        }

        lgraphics::Graphics::getDevice().setClearColor(0xFF808080U);

        lanim::InitParam animInitParam;
        animInitParam.ctrlVectorIncSize_ = 8;
        animInitParam.maxNumClip_ = 32;
        animInitParam.maxNumSkeleton_ = 16;

        lframework::System::InitParam sysParam("", 32);
        lframework::System::initialize(sysParam, animInitParam);

        textRenderer_.initialize(MaxChars, CharW, CharH, Rows, Cols);
        textRenderer_.setTextureFromMemory(textTexture, size);


        // シーン初期化
        {
            lscene::Scene& scene = lframework::System::getRenderSys().getScene();

            // カメラ設定
            lscene::Camera& camera = scene.getCamera();
            lmath::Matrix44 mat;

            Config& config = Config::getInstance();

            config.perspectiveFovLinearZ(mat, (45.0f/180.0f*PI));

            camera.setProjMatrix(mat);

            camera.updateMatrix();

            // ライト設定
            lscene::LightEnvironment& lightEnv = scene.getLightEnv();
            lightEnv.getDirectionalLight().setColor(lmath::Vector3(1.0f, 1.0f, 1.0f), 1.0f);
            lightEnv.getDirectionalLight().setDirection(lmath::Vector3(0.0f, 1.0f, 0.0f));
        }

        prevMSec_ = lcore::getTime();
        currentMSec_ = prevMSec_;
        return true;
    }

    //----------------------------------------------
    void Application::terminate()
    {
        scene_.release();
        textRenderer_.terminate();
        lframework::System::terminate();
        lgraphics::Graphics::terminate();
    }

    //----------------------------------------------
    void Application::update()
    {
        static char buffer[128];

        lrender::RenderingSystem &renderSys = lframework::System::getRenderSys();

        scene_.update();


        renderSys.beginDraw();

        currentMSec_ = lcore::getTime();

        u32 d = (prevMSec_>currentMSec_)? (currentMSec_ - prevMSec_) : (0xFFFFFFFFU-prevMSec_+currentMSec_);
        prevMSec_ = currentMSec_;

//デバッグ用ログ
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
        //sprintf(buffer, "total %d msec", d);
        //textRenderer_.print(0, 0, buffer);

        //u32 fps = (6000 * 1667)/(d*10000);

        //sprintf(buffer, "%d fps", fps);
        //textRenderer_.print(1, 0, buffer);


        //sprintf(buffer, "vertices: %d", g_debugLog.getNumVertices());
        //textRenderer_.print(2, 0, buffer);

        //sprintf(buffer, "batches: %d", g_debugLog.getNumBatches());
        //textRenderer_.print(3, 0, buffer);
#endif

        renderSys.draw();

        textRenderer_.draw();

        renderSys.endDraw();

        if(d<MSecPerFrame){ //1フレームの時間が短ければ眠る
            d = (MSecPerFrame - d)*1000;
            usleep(d);//マイクロ秒単位で眠る
        }
    }

    //----------------------------------------------
    void Application::setViewport(s32 left, s32 top, s32 width, s32 height)
    {
        if(width<2){
            width = 2;
        }
        if(height<2){
            height = 2;
        }
        lgraphics::Graphics::getDevice().setViewport(left, top, width, height);

        Config& config = Config::getInstance();

        lscene::Scene& scene = lframework::System::getRenderSys().getScene();

        // カメラ設定
        config.setViewportSize(width, height);

        lscene::Camera& camera = scene.getCamera();
        lmath::Matrix44 mat;

        config.perspectiveFovLinearZ(mat, (45.0f/180.0f*PI));
        camera.setProjMatrix(mat);
        camera.updateMatrix();

        Input::initialize(width, height);

        u32 rows = width/(CharW*RowsRatio);
        u32 cols = height/(CharH*ColsRatio);
        textRenderer_.setViewSize(rows, cols);

        lcore::Log("viewport %d, %d", width, height);
    }

    //----------------------------------------------
    //PMMロード
    bool Application::loadPmm(const Char* filename, const Char* directory)
    {
        scene_.release(); //携帯機ではメモリが少ないので先に解放する。ロード失敗すれば元には戻らない
        pmm::Loader pmmLoader;
        pmmLoader.load(filename, directory);
        if(pmm::Loader::Error_None != pmmLoader.getErrorCode()){
            return false;
        }

        egda::Scene scene(
            pmmLoader.getNumModels(),
            pmmLoader.releaseModelPacks(),
            pmmLoader.getCameraAnimPack(),
            pmmLoader.getLightAnimPack(),
            pmmLoader.getNumAccessories(),
            pmmLoader.releaseAccessoryPacks()
            );

        scene.setCameraMode( scene_.getCameraMode() );

        scene_.swap(scene);

        scene_.initialize();

        //デバッグ用ログ
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
        //g_debugLog = pmmLoader.debugLog_;
#endif

#if defined(_DEBUG)
        lframework::System::attachDebugDraw();
#endif
        return true;
    }
}
