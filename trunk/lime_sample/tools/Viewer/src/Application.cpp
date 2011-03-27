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

namespace viewer
{
    Application::Application()
        :animObj_(NULL)
        ,animControler_(NULL)
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

        // シーン初期化
        {
            lscene::Scene& scene = lframework::System::getRenderSys().getScene();

            // カメラ設定
            lscene::Camera& camera = scene.getCamera();
            lmath::Matrix44 mat;
            mat.lookAt(lmath::Vector3(0.0f, 13.0f, -30.0f), lmath::Vector3(0.0f, 13.0f, 0.0f), lmath::Vector3(0.0f, 1.0f, 0.0f));
            //mat.lookAt(lmath::Vector3(0.0f, 0.0f, -2.0f), lmath::Vector3(0.0f, 0.0f, 0.0f), lmath::Vector3(0.0f, 1.0f, 0.0f));
            camera.setViewMatrix(mat);

            mat.perspectiveFovLinearZ(45.0f/180.0f*PI, static_cast<lcore::f32>(Width)/Height, 1.0f, 200.0f);
            camera.setProjMatrix(mat);

            camera.updateMatrix();

            // ライト設定
            lscene::LightEnvironment& lightEnv = scene.getLightEnv();
            lightEnv.getDirectionalLight().setColor(lmath::Vector3(1.0f, 1.0f, 1.0f), 2.0f);
            lightEnv.getDirectionalLight().setDirection(lmath::Vector3(0.0f, 1.0f, 0.0f));
            //lightEnv.getDirectionalLight().setDirection(lmath::Vector3(0.0f, -1.0f, 0.0f));

            lgraphics::Graphics::getDevice().setClearColor(0xFF808080U);


            Input::initialize( window_.getHandle().hWnd_, window_.getViewWidth(), window_.getViewHeight() );
        }
#if 0
        {
            lx::XLoader xLoader;
            animObj_ = LIME_NEW lscene::Object;
            if(xLoader.open("data/pmm/Accessory/negi.x")){
            xLoader.load(*animObj_, "data/pmm/Accessory/", 10.0f, false);
            animObj_->initializeShader();
            animObj_->addDraw();
            }
            return;
        }
#endif

#if 1
        {//PMMロード
            scene_.release(); //携帯機ではメモリが少ないので先に解放する。ロード失敗すれば元には戻らない
            pmm::Loader pmmLoader;
            pmmLoader.load("Sample.pmm", "data/pmm/");
            //pmmLoader.load("sample2.pmm", "data/pmm/");
            //pmmLoader.load("SampleAllStar.pmm", "data/pmm/");
            if(pmm::Loader::Error_None == pmmLoader.getErrorCode()){

                //release時に0になるので先に取得
                u32 numModels = pmmLoader.getNumModels();
                u32 numAccessories = pmmLoader.getNumAccessories();
                viewer::Scene scene(
                    numModels,
                    pmmLoader.releaseModelPacks(),
                    pmmLoader.getCameraAnimPack(),
                    pmmLoader.getLightAnimPack(),
                    numAccessories,
                    pmmLoader.releaseAccessoryPacks()
                    );

                scene_.swap(scene);

                f32 aspect = static_cast<f32>( window_.getViewWidth() ) / window_.getViewHeight();
                scene_.initialize(aspect);
#if defined(_DEBUG)
                lframework::System::attachDebugDraw();
#endif
            }
            return;
        }
#endif

#if 0
        //モデルロード
        {
            pmd::Pack pack;
#if 1
            static const char* name = "data/Lat/Lat_miku2.2_Sailor_NoEdge.pmd";
            static const char* dir = "data/Lat/";
#elif 0
            static const char* name = "data/1052/1052.pmd";
            static const char* dir = "data/1052/";
#elif 1
            static const char* name = "data/1052C-Re0710/1052C-ReB.pmd";
            static const char* dir = "data/1052C-Re0710/";
#endif

            if(true == pack.open(name)){
                animObj_ = LIME_NEW lscene::AnimObject;
                if(false == pack.createObject(*animObj_, dir, false)){
                    LIME_DELETE(animObj_);
                }else{

                    lanim::IKPack::pointer ikPack( pack.releaseIKPack() );
                    animObj_->setIKPack(ikPack);

                    animObj_->initializeShader();
                    animObj_->addDraw();
#if defined(_DEBUG)
                    lframework::System::attachDebugDraw();
#endif
                }
            }
        }

        //アニメーションロード
        {
#if 0
            static const char* name = "data/Lat/Lat式ミクVer2.2_Normal.pmd";
#elif 1
            static const char* name = "data/Dance_ver_B/hinsou.vmd";
#endif
            lanim::AnimationClip::pointer animClip = vmd::Pack::loadFromFile(name);


            //スケルトンあったらアニメーションセットアップ
            if(animClip != NULL
                && animObj_ != NULL
                && animObj_->getSkeleton() != NULL)
            {
                typedef lanim::AnimCtrlSimple<
                    lanim::AnimationControlerResource,
                    lanim::AnimationControlerPartial,
                    lanim::AnimationControlerIK
                > AnimCtrl;
                AnimCtrl* animCtrl = LIME_NEW AnimCtrl;
                animCtrl->setSkeleton(animObj_->getSkeleton());
                animObj_->setSkeletonPose( &(animCtrl->getSkeletonPose()) );

                animControler_ = animCtrl;
                animControler_->initialize();

                animCtrl->setClip(animClip);

                if(animObj_->getIKPack() != NULL){
                    animCtrl->setIKPack(animObj_->getIKPack());
                }
#if 0
                {//少しテスト
                    lanim::JointAnimation& jointAnim = animClip->getJointAnimation( animClip->getNumJoints() / 3 );

                    const lanim::JointPoseWithFrame* pose0 = &(jointAnim.binarySearch(0));
                    const lanim::JointPoseWithFrame& last = jointAnim.getPose( jointAnim.getNumPoses() - 1);
                    const lanim::JointPoseWithFrame* pose1 = &(jointAnim.binarySearch(1));
                    const lanim::JointPoseWithFrame* pose2 = &(jointAnim.binarySearch(last.frameNo_));
                    const lanim::JointPoseWithFrame* pose3 = &(jointAnim.binarySearch(last.frameNo_-1));
                    const lanim::JointPoseWithFrame* pose4 = &(jointAnim.binarySearch(last.frameNo_+1));
                    const lanim::JointPoseWithFrame* pose5 = &(jointAnim.binarySearch(last.frameNo_+2));

                }
#endif
                lanim::AnimationSystem &animSys = lframework::System::getAnimSys();
                animSys.add(animControler_);

            }
        }
#endif
    }

    void Application::update()
    {
        Input::update();

        lrender::RenderingSystem &renderSys = lframework::System::getRenderSys();
        //lanim::AnimationSystem &animSys = lframework::System::getAnimSys();

        lframework::System::debugPrint(10.0f, 10.0f, "test");

        static lcore::s32 count = 0;
        static const lcore::s32 max_count = 180;
        if(++count>max_count){
            count = 0;
        }

        if(animObj_){
            //animObj_->getWorldMatrix().rotateAxis(0.0f, 1.0f, 0.0f, (0.5f/180.0f)*PI);
            //animObj_->getWorldMatrix().setTranslate(0.0f, 2.0f*lmath::sinf_fast(PI2*(lcore::f32)count/max_count), 0.0f);
            //animObj_->getWorldMatrix().setTranslate(0.0f, -5.0f, 0.0f);
        }

        //animSys.update();

        scene_.update();

        renderSys.beginDraw();
        renderSys.draw();
        renderSys.endDraw();

        lframework::System::debugDrawClear();

        static lcore::u32 prevTime = 0;
        lcore::u32 time = lcore::getTimeFromPerformanCounter();
        lcore::u32 t = time - prevTime;
        prevTime = time;

        t = 60 * (16.67*1000.0/t);

        char str[128];
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