/**
@file Application.cpp
@author t-sakai
@date 2010/02/14 create
*/
#include "stdafx.h"
#include "Application.h"
#include <Pmd.h>
#include <Vmd.h>
#include <Pmm.h>
#include <XLoader.h>

#include <lgraphics/lgraphics.h>

#include <lframework/scene/AnimObject.h>
#include <lframework/scene/Object.h>
#include <lframework/System.h>
#include <lframework/anim/AnimationSystem.h>
#include <lframework/render/RenderingSystem.h>

#include <lframework/scene/Scene.h>
#include <lframework/scene/Camera.h>
#include <lframework/scene/LightEnvironment.h>

#include <lframework/anim/AnimationControlerPartial.h>
#include <lframework/anim/JointAnimation.h>
#include <lframework/anim/IKPack.h>


#include "AnimationControlerIK.h"
#include "AnimationControlerImpl.h"

#include "Input.h"
#include "DynamicsWorld.h"

#include <converter.h>

namespace
{
    //デバッグ用ログ
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
        lconverter::DebugLog g_debugLog;
#endif
}

namespace viewer
{
    Application::Application()
        :status_(Status_Load)
        ,animObj_(NULL)
        ,animControler_(NULL)
        ,loader_(NULL)
    {
    }

    Application::~Application()
    {
    }

    void Application::initialize()
    {
        lanim::InitParam animInitParam;
        animInitParam.ctrlVectorIncSize_ = 8;
        animInitParam.maxNumClip_ = 64;
        animInitParam.maxNumSkeleton_ = 16;

        lframework::System::InitParam sysParam("data/evac-fh.otf", 32);
        lframework::System::initialize(sysParam, animInitParam);

#if defined(LIME_GLES2)
        textRenderer_.initialize(64, 9, 20, 6, 16);
        textRenderer_.setTextureFromFile("data/M_plus_1m_regular10.png");
#endif

        // シーン初期化
        {
            lscene::Scene& scene = lframework::System::getRenderSys().getScene();

            // カメラ設定
            lscene::Camera& camera = scene.getCamera();
            lmath::Matrix44 mat;
            mat.lookAt(lmath::Vector3(0.0f, 13.0f, -30.0f), lmath::Vector3(0.0f, 13.0f, 0.0f), lmath::Vector3(0.0f, 1.0f, 0.0f));
            //mat.lookAt(lmath::Vector3(0.0f, 0.0f, -2.0f), lmath::Vector3(0.0f, 0.0f, 0.0f), lmath::Vector3(0.0f, 1.0f, 0.0f));
            camera.setViewMatrix(mat);

            mat.perspectiveFovLinearZ(45.0f/180.0f*PI, static_cast<lcore::f32>(Width)/Height, 0.5f, 3000.0f);
            camera.setProjMatrix(mat);

            camera.updateMatrix();

            // ライト設定
            lscene::LightEnvironment& lightEnv = scene.getLightEnv();
            lightEnv.getDirectionalLight().setColor(lmath::Vector3(1.0f, 1.0f, 1.0f), 2.0f);
            lightEnv.getDirectionalLight().setDirection(lmath::Vector3(0.0f, 1.0f, 0.0f));
            //lightEnv.getDirectionalLight().setDirection(lmath::Vector3(0.0f, -1.0f, 0.0f));

            lgraphics::Graphics::getDevice().setClearColor(0xFF808080U);


            Input::initialize( window_.getHandle().hWnd_, window_.getViewWidth(), window_.getViewHeight() );

            lconverter::Config::getInstance().setAlphaTest(true);
            lconverter::Config::getInstance().setTextureCompress(false);

            dynamics::DynamicsWorld::initialize(false);
        }

#if 1
        {//PMMロード
            scene_.release(); //携帯機ではメモリが少ないので先に解放する。ロード失敗すれば元には戻らない

            loader_ = LIME_NEW pmm::Loader;
            //loader_->open("HelloP.pmm", "data/HelloP/");
            loader_->open("mikumiku.pmm", "data/MikuMiku/");
            //loader_->open("test.pmm", "data/MikuMiku/");
            //loader_->open("kkhscene01.pmm", "data/StrobeNights/");
            //loader_->open("SampleAllStar.pmm", "data/pmm/");
            //loader_->open("Sample.pmm", "data/pmm/");
            if(loader_->getErrorCode() != pmm::Loader::Error_None){
                LIME_DELETE(loader_);
                status_ = Status_Play;
            }else{
                status_ = Status_Load;
            }
        }
#endif

        prevMSec_ = lcore::getTime();
        currentMSec_ = prevMSec_;
        loadingCounter_ = 0;
        loadingIndex_= 0;
    }

    void Application::update()
    {
        Input::update();

        lrender::RenderingSystem &renderSys = lframework::System::getRenderSys();
        //lanim::AnimationSystem &animSys = lframework::System::getAnimSys();


        static lcore::s32 count = 0;
        static const lcore::s32 max_count = 180;
        if(++count>max_count){
            count = 0;
        }

        switch(status_)
        {
        case Status_Load:
            {
                loader_->load();
                if(loader_->getStatus() == pmm::Loader::Status_Finish){
                    if(loader_->getErrorCode() == pmm::Loader::Error_None){

                        //release時に0になるので先に取得
                        viewer::Scene scene(*loader_);

                        scene_.swap(scene);

                        f32 aspect = static_cast<f32>( window_.getViewWidth() ) / window_.getViewHeight();
                        scene_.initialize(aspect);

                    }

                    LIME_DELETE(loader_);
                    status_ = Status_Play;
                }
            }
            break;
        default:
            break;
        };

        scene_.update();

        //textRenderer_.print(0, 0, " !\"#$%&\'()*+,-./");
        //textRenderer_.print(1, 0, "0123456789:;<=>?");
        //textRenderer_.print(2, 0, "@ABCDEFGHIJKLMNO");
        //textRenderer_.print(3, 0, "PQRSTUVWXYZ[\\]^_");
        //textRenderer_.print(4, 0, "`abcdefghijklmno");
        //textRenderer_.print(5, 0, "pqrstuvwxyz{|}");
#if 1
        {//なうろーでぃんぐ

            static const lcore::Char NowLoading[] = "Now Loading";
            static const lcore::u32 Len = sizeof(NowLoading) - 1;

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

            lcore::Char buffer[Len+1];
            u32 i,j;
            for(i=0; i<loadingIndex_; ++i){
                buffer[i] = NowLoading[i];
            }
            buffer[i] = '\0';
            u32 col = 3;
            textRenderer_.print(5-1, col, buffer);
            col += loadingIndex_;

            buffer[0] = NowLoading[loadingIndex_];
            buffer[1] = '\0';
            textRenderer_.print(5-2, col, buffer);
            col += 1;


            for(i=loadingIndex_+1, j=0; i<Len; ++i, ++j){
                buffer[j] = NowLoading[i];
            }
            buffer[j] = '\0';
            textRenderer_.print(5-1, col, buffer);

        }
#endif
        char str[128];
//デバッグ用ログ
#if defined(LIME_LIBCONVERTER_DEBUGLOG_ENABLE)
        sprintf(str, "vertices: %d", g_debugLog.getNumVertices());
        textRenderer_.print(3, 0, str);

        sprintf(str, "batches: %d", g_debugLog.getNumBatches());
        textRenderer_.print(4, 0, str);
#endif
        renderSys.beginDraw();
        renderSys.draw();

        //物理演算デバッグ描画
        dynamics::DynamicsWorld::getInstance().debugDraw();

#if defined(LIME_GLES2)
        textRenderer_.draw();
#endif

        renderSys.endDraw();

        lframework::System::debugDrawClear();

        static lcore::u32 prevTime = 0;
        lcore::u32 time = lcore::getTimeFromPerformanCounter();
        lcore::u32 t = time - prevTime;
        prevTime = time;

        t = 60 * (16.67*1000.0/t);

        sprintf(str, "%d", t);
        SetWindowText(window_.getHandle().hWnd_, str);

    }

    void Application::terminate()
    {
        Input::terminate();

        lanim::AnimationSystem &animSys = lframework::System::getAnimSys();
        if(animControler_ != NULL){
            animSys.remove(animControler_);
            LIME_DELETE(animControler_);
        }

        LIME_DELETE(animObj_);

        scene_.release();

#if defined(LIME_GLES2)
        textRenderer_.terminate();
#endif

        dynamics::DynamicsWorld::terminate();
        lframework::System::terminate();
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

            //     case WM_CLOSE:
            //DestroyWindow(hWnd);
            //         break;

        case WM_PAINT:
            {
                ValidateRect(hWnd, NULL);
            }

            break;

        case WM_ACTIVATE:
            {
                if(WA_INACTIVE == wParam){
                    Input::unacquire();
                }else{
                    Input::acquire();
                }
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
}
