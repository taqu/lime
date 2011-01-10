#include "stdafx.h"
#include "Application.h"


#include <lcore/liostream.h>
#include <lgraphics/lgraphics.h>
#include <lgraphics/api/InitParam.h>
#include <lgraphics/api/Enumerations.h>

#include <Pmd.h>
#include <Vmd.h>

#include <lframework/scene/AnimObject.h>
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
namespace
{
    typedef lanim::AnimCtrlSimple<
                    lanim::AnimationControlerResource,
                    lanim::AnimationControlerPartial,
                    lanim::AnimationControlerIK
                > AnimCtrl;
}

namespace egda
{
    Application::Application()
        :counter_(0.0f)
        ,animObj_(NULL)
        ,animControler_(NULL)
    {
    }

    Application::~Application()
    {
    }

    //----------------------------------------------
    bool Application::initialize()
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

        lframework::System::InitParam sysParam("data/evac-fh.otf", 32);
        lframework::System::initialize(sysParam, animInitParam);

        // シーン初期化
        {
            lscene::Scene& scene = lframework::System::getRenderSys().getScene();

            // カメラ設定
            lscene::Camera& camera = scene.getCamera();
            lmath::Matrix44 mat;
            mat.lookAt(lmath::Vector3(0.0f, 10.0f, -25.0f), lmath::Vector3(0.0f, 10.0f, 0.0f), lmath::Vector3(0.0f, 1.0f, 0.0f));
            camera.setViewMatrix(mat);

            mat.perspectiveFov(45.0f, 600.0f/400.0f, 0.01f, 100.0f);
            camera.setProjMatrix(mat);

            camera.updateMatrix();

            // ライト設定
            lscene::LightEnvironment& lightEnv = scene.getLightEnv();
            lightEnv.getDirectionalLight().setColor(lmath::Vector3(1.0f, 1.0f, 1.0f), 1.0f);
            lightEnv.getDirectionalLight().setDirection(lmath::Vector3(0.0f, 1.0f, 0.0f));

            lgraphics::Graphics::getDevice().setClearColor(0xFF808080U);
        }

        counter_ = 0.0f;
        return true;
    }

    //----------------------------------------------
    void Application::terminate()
    {
        lanim::AnimationSystem &animSys = lframework::System::getAnimSys();
        if(animControler_ != NULL){
            animSys.remove(animControler_);
            LIME_DELETE(animControler_);
        }

        LIME_DELETE(animObj_);

        lframework::System::terminate();
        lgraphics::Graphics::terminate();
    }

    //----------------------------------------------
    void Application::update()
    {
        lrender::RenderingSystem &renderSys = lframework::System::getRenderSys();
        lanim::AnimationSystem &animSys = lframework::System::getAnimSys();

        animSys.update();

        renderSys.beginDraw();
        renderSys.draw();
        renderSys.endDraw();
    }

    //----------------------------------------------
    void Application::setViewport(s32 left, s32 top, s32 width, s32 height)
    {
        lgraphics::Graphics::getDevice().setViewport(left, top, width, height);

        lscene::Scene& scene = lframework::System::getRenderSys().getScene();

        // カメラ設定
        lscene::Camera& camera = scene.getCamera();
        lmath::Matrix44 mat;
        mat.perspectiveFov(45.0f, static_cast<f32>(width)/height, 0.01f, 100.0f);
        camera.setProjMatrix(mat);
        camera.updateMatrix();
    }

    //----------------------------------------------
    bool Application::openModel(const Char* path, const Char* directory)
    {
        lcore::ifstream infile(path);
        if(false == infile.is_open()){
            return false;
        }

        bool ret = false;
        pmd::Pack pack;
        if(true == pack.open(path)){
            lscene::AnimObject* animObj = LIME_NEW lscene::AnimObject;
            if(false == pack.createObject(*animObj, directory, false)){
                LIME_DELETE(animObj);
            }else{
                lanim::IKPack::pointer ikPack( pack.releaseIKPack() );
                animObj->setIKPack(ikPack);

                animObj->initializeShader();
                LIME_DELETE(animObj_);
                animObj->addDraw();
                
                animObj_ = animObj;

                if(animControler_){
                    initAnimControler();

                    AnimCtrl* animCtrl = reinterpret_cast<AnimCtrl*>(animControler_);
                    if(animObj_->getIKPack() != NULL){
                        animCtrl->setIKPack(animObj_->getIKPack());
                    }else{
                        lanim::IKPack::pointer nullIK;
                        animCtrl->setIKPack(nullIK);
                    }
                }

                ret = true;
            }
        }

        infile.close();
        return ret;
    }

    //----------------------------------------------
    bool Application::openAnim(const Char* path)
    {
        if(animObj_ == NULL){
            lcore::Log("need a model to load anim");
            return false;
        }

        //スケルトンなかったらロードしない
        if(animObj_->getSkeleton() == NULL){
            lcore::Log("need a skeleton of model to load anim");
            return false;
        }

        lanim::AnimationClip::pointer animClip = vmd::Pack::loadFromFile(path);
        if(animClip == NULL){
            lcore::Log("fail to load clip");
            return false;
        }

        if(animControler_ == NULL){
            lcore::Log("start to create anim controler");
            animControler_ = LIME_NEW AnimCtrl;

            initAnimControler();

            lanim::AnimationSystem &animSys = lframework::System::getAnimSys();
            animSys.add(animControler_);
        }else{
            initAnimControler();
        }

        AnimCtrl* animCtrl = reinterpret_cast<AnimCtrl*>(animControler_);
        animCtrl->setClip(animClip);
        return true;
    }

    void Application::initAnimControler()
    {
        animControler_->setSkeleton(animObj_->getSkeleton());
        animObj_->setSkeletonPose( &(animControler_->getSkeletonPose()) );

        if(animObj_->getIKPack() != NULL){
            lcore::Log("set ik to anim controler");
            AnimCtrl* animCtrl = reinterpret_cast<AnimCtrl*>(animControler_);
            animCtrl->setIKPack(animObj_->getIKPack());
        }
        animControler_->initialize();
    }
}
