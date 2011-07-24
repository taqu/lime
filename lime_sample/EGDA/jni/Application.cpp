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

namespace egda
{
    Application::Application()
        :prevMSec_(0)
        ,currentMSec_(0)
        ,loadingIndex_(0)
        ,loadingCounter_(0)
        ,loader_(NULL)
    {
    }

    Application::~Application()
    {
    }

    //----------------------------------------------
    bool Application::initialize(const Char* textTexture, u32 size)
    {
        Config::initialize();

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
        LIME_DELETE(loader_);
        scene_.release();

        textRenderer_.terminate();

        lframework::System::terminate();
        lgraphics::Graphics::terminate();
    }

    //----------------------------------------------
    void Application::update()
    {
        scene_.update();

//デバッグ用ログ
#if defined(LIME_EGDA_DISP_FPS)
        currentMSec_ = lcore::getTime();

        u32 d = (prevMSec_>currentMSec_)? (currentMSec_ - prevMSec_) : (0xFFFFFFFFU-prevMSec_+currentMSec_);
        prevMSec_ = currentMSec_;

        static char buffer[128];
        sprintf(buffer, "total %d msec", d);
        textRenderer_.print(0, 0, buffer);

        u32 fps = (6000 * 1667)/(d*10000);

        sprintf(buffer, "%d fps", fps);
        textRenderer_.print(1, 0, buffer);
#endif

        lrender::RenderingSystem &renderSys = lframework::System::getRenderSys();

        renderSys.beginDraw();

        renderSys.draw();

#if defined(LIME_EGDA_DISP_FPS)
        textRenderer_.draw();
#endif
        renderSys.endDraw();
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
    // PMMロード
    bool Application::loadPmm(const Char* filename, const Char* directory)
    {
        scene_.release(); //携帯機ではメモリが少ないので先に解放する。ロード失敗すれば元には戻らない

        LIME_DELETE(loader_);

        loader_ = LIME_NEW pmm::Loader;
        if(NULL == loader_){
            return false;
        }

        loader_->open(filename, directory);

        if(loader_->getErrorCode() != pmm::Loader::Error_None){
            return false;
        }

        loadingCounter_ = 0;
        loadingIndex_= 0;
        return true;
    }

    //----------------------------------------------
    // PMMロード
    bool Application::updateLoad()
    {
        if(NULL == loader_){
            return true;
        }

        loader_->load();

        if(loader_->getStatus() == pmm::Loader::Status_Finish){
            if(loader_->getErrorCode() == pmm::Loader::Error_None){
                egda::Scene scene(*loader_);

                scene.setCameraMode( scene_.getCameraMode() );

                scene_.swap(scene);
                scene_.initialize();

#if defined(_DEBUG)
                lframework::System::attachDebugDraw();
#endif
            }
            LIME_DELETE(loader_);
            return true;
        }


        {//なうろーでぃんぐ
            lrender::RenderingSystem &renderSys = lframework::System::getRenderSys();

            static const Char NowLoading[] = "Now Loading";
            static const u32 Len = sizeof(NowLoading) - 1;

            currentMSec_ = lcore::getTime();

            u32 d = (prevMSec_>currentMSec_)? (currentMSec_ - prevMSec_) : (0xFFFFFFFFU-prevMSec_+currentMSec_);
            prevMSec_ = currentMSec_;
            loadingCounter_ += d;

            d = loadingCounter_ >> LoadingCountCycleBits;
            loadingCounter_ -= d<<LoadingCountCycleBits;

            loadingIndex_ += d;
            if(loadingIndex_>=Len){
                loadingIndex_ = 0;
            }

            Char buffer[Len+1];
            u32 i,j;
            for(i=0; i<loadingIndex_; ++i){
                buffer[i] = NowLoading[i];
            }
            buffer[i] = '\0';
            u32 col = 2;
            u32 row = Cols - 1;
            textRenderer_.print(row, col, buffer);
            col += loadingIndex_;

            buffer[0] = NowLoading[loadingIndex_];
            buffer[1] = '\0';
            textRenderer_.print(row-1, col, buffer);
            col += 1;


            for(i=loadingIndex_+1, j=0; i<Len; ++i, ++j){
                buffer[j] = NowLoading[i];
            }
            buffer[j] = '\0';
            textRenderer_.print(row, col, buffer);

            renderSys.beginDraw();

            textRenderer_.draw();
            renderSys.endDraw();
        }
        return false;
    }

    //----------------------------------------------
    // ロードキャンセル
    void Application::cancelLoad()
    {
        LIME_DELETE(loader_);
    }
}
