/**
@file Scene.cpp
@author t-sakai
@date 2011/02/06
*/
#include "stdafx.h"
#include "Scene.h"

#include "Camera.h"
#include "Light.h"

#include <Pmd.h>
#include <Vmd.h>
#include <PmmDef.h>

#include <lcore/liostream.h>
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
    typedef lanim::AnimCtrlSimpleIK<
        lanim::AnimationControlerResource,
        lanim::AnimationControlerPartial,
        lanim::AnimationControlerIK
    > AnimCtrlIK;

    typedef lanim::AnimCtrlSimple<
        lanim::AnimationControlerResource,
        lanim::AnimationControlerPartial
    > AnimCtrl;

    //---------------------------------------------
    //---
    //--- Scene::Impl
    //---
    //---------------------------------------------
    struct Scene::Impl
    {
        Impl();
        Impl(
            u32 numModels,
            pmm::ModelPack* modelPacks,
            pmm::CameraAnimPack& cameraAnimPack,
            pmm::LightAnimPack& lightAnimPack,
            u32 numAccessories,
            pmm::AccessoryPack* accPacks);

        ~Impl();

        void release();

        void initialize(f32 aspect);

        void update();

        void setState(Scene::State state);

        Scene::State state_;

        u32 frameCounter_;
        u32 lastFrame_;

        u32 numModels_;
        pmm::ModelPack* modelPacks_;

        pmm::CameraAnimPack cameraAnimPack_;
        pmm::LightAnimPack lightAnimPack_;

        u32 numAccessories_;
        pmm::AccessoryPack* accPacks_;

        Camera camera_;
        Light light_;
    };

    Scene::Impl::Impl()
        :state_(Scene::State_Stop)
        ,frameCounter_(0)
        ,lastFrame_(0)
        ,numModels_(0)
        ,modelPacks_(NULL)
        ,numAccessories_(0)
        ,accPacks_(NULL)
    {
    }

    Scene::Impl::Impl(
        u32 numModels,
        pmm::ModelPack* modelPacks,
        pmm::CameraAnimPack& cameraAnimPack,
        pmm::LightAnimPack& lightAnimPack,
        u32 numAccessories,
        pmm::AccessoryPack* accPacks)
        :state_(Scene::State_Stop)
        ,frameCounter_(0)
        ,lastFrame_(0)
        ,numModels_(numModels)
        ,modelPacks_(modelPacks)
        ,numAccessories_(numAccessories)
        ,accPacks_(accPacks)
    {
        cameraAnimPack_.swap( cameraAnimPack );
        lightAnimPack_.swap( lightAnimPack );
    }

    Scene::Impl::~Impl()
    {
        release();
    }

    void Scene::Impl::release()
    {
        state_ = Scene::State_Stop;

        numAccessories_= 0;
        LIME_DELETE_ARRAY(accPacks_);

        {
            pmm::LightAnimPack empty;
            empty.swap(lightAnimPack_);
        }

        {
            pmm::CameraAnimPack empty;
            empty.swap(cameraAnimPack_);
        }

        lanim::AnimationSystem &animSys = lframework::System::getAnimSys();
        for(u32 i=0; i<numModels_; ++i){
            lanim::AnimationControler* animCtrl = modelPacks_[i].getAnimationControler();
            if(animCtrl != NULL){
                animSys.remove( animCtrl );
            }
        }

        numModels_= 0;
        LIME_DELETE_ARRAY(modelPacks_);
    }

    // シーン初期化
    void Scene::Impl::initialize(f32 aspect)
    {
        frameCounter_ = 0;

        lanim::AnimationSystem &animSys = lframework::System::getAnimSys();

        f32 lastFrame = 0.0f; //フレーム数を統一する

        for(u32 i=0; i<numModels_; ++i){
            lanim::AnimationClip::pointer& animClip = modelPacks_[i].getAnimationClip();
            if(animClip){
                if(animClip->getLastFrame() > lastFrame){
                    lastFrame = animClip->getLastFrame();
                }
            }

        }

        for(u32 i=0; i<numModels_; ++i){
            lscene::AnimObject* object = modelPacks_[i].getObject();
            LASSERT(object != NULL);
            object->initializeShader();
            object->addDraw();

            lanim::AnimationClip::pointer& animClip = modelPacks_[i].getAnimationClip();

            if(object->getIKPack() != NULL
                && object->getIKPack()->getNumIKs() > 0)
            {
                AnimCtrlIK* animCtrlIK = LIME_NEW AnimCtrlIK;
                animCtrlIK->setSkeleton( object->getSkeleton() );
                object->setSkeletonPose( &(animCtrlIK->getSkeletonPose()) );
                animCtrlIK->setIKPack( object->getIKPack() );
                animCtrlIK->setClip(animClip);

                //animCtrlIK->getFlags().set( lanim::AnimFlag_Active );
                animClip->setLastFrame(lastFrame);

                animCtrlIK->initialize();
                animSys.add(animCtrlIK);
                modelPacks_[i].setAnimationControler(animCtrlIK);

                modelPacks_[i].resetMorph();
            }else{

                AnimCtrl* animCtrl = LIME_NEW AnimCtrl;
                animCtrl->setSkeleton( object->getSkeleton() );
                object->setSkeletonPose( &(animCtrl->getSkeletonPose()) );
                animCtrl->setClip(animClip);

                //animCtrlIK->getFlags().set( lanim::AnimFlag_Active );
                animClip->setLastFrame(lastFrame);

                animCtrl->initialize();
                animSys.add(animCtrl);
                modelPacks_[i].setAnimationControler(animCtrl);

                modelPacks_[i].resetMorph();
            }
        }


        //アクセサリ描画登録
        for(u32 i=0; i<numAccessories_; ++i){
            lscene::Object* object = accPacks_[i].getObject();
            LASSERT(object != NULL);

            if(accPacks_[i].isDisp()){
                object->initializeShader();
                object->addDraw();
                accPacks_[i].reset(numModels_, modelPacks_);
            }
        }


        {
            // カメラ初期化
            const pmm::CameraPose& cameraPose = cameraAnimPack_.getPose(0);
            camera_.setInitial(cameraPose.position_, cameraPose.target_, cameraPose.fov_, aspect);
            camera_.setCameraAnim(&cameraAnimPack_, aspect);

            camera_.setMode(Camera::Mode_FrameAnim);
            //camera_.setMode(Camera::Mode_Manual);
        }

        {
            //ライト初期化
            light_.setLightAnim(&lightAnimPack_, static_cast<u32>(lastFrame));
        }

        lastFrame_ = static_cast<u32>(lastFrame);
        state_ = State_Play;
        //state_ = State_Stop;

        frameCounter_ = 0;
    }


    void Scene::Impl::update()
    {
        if(Input::isClick(Input::Key_0)){
            State trans[State_Num] = 
            {
                State_Play,
                State_Stop,
            };
            state_ = trans[state_];
        }

        if(Input::isClick(Input::Key_1)){
            camera_.changeMode();
        }

        switch(state_)
        {
        case State_Stop:
            if(camera_.getMode() == Camera::Mode_Manual){
                camera_.update(frameCounter_);
            }
            break;

        case State_Play:
            {
                lanim::AnimationSystem &animSys = lframework::System::getAnimSys();
                animSys.update();

                //if(frameCounter_>=369){
                //    if(Input::isClick(Input::Key_0)){
                //        animSys.update();
                //        ++frameCounter_;
                //    }

                //}else{
                //    animSys.update();
                //    ++frameCounter_;
                //}
                if(++frameCounter_>lastFrame_){
                //if(frameCounter_>lastFrame_){
                    frameCounter_ = 0;
                    for(u32 i=1; i<numAccessories_; ++i){
                        accPacks_[i].initialize();
                    }

                    camera_.initialize();
                    light_.initialize();
                }

                for(u32 i=0; i<numModels_; ++i){
                    modelPacks_[i].updateMorph(frameCounter_);
                }

                //アクセサリ更新
                for(u32 i=0; i<numAccessories_; ++i){
                    accPacks_[i].update(frameCounter_);
                }

                camera_.update(frameCounter_);
                light_.update();
            }
            break;
        };

    }

    void Scene::Impl::setState(Scene::State state)
    {
        state_ = state;
    }

    //---------------------------------------------
    //---
    //--- Scene
    //---
    //---------------------------------------------
    Scene::Scene()
    {
        impl_ = LIME_NEW Impl;

    }

    Scene::Scene(
        u32 numModels,
        pmm::ModelPack* modelPacks,
        pmm::CameraAnimPack& cameraAnimPack,
        pmm::LightAnimPack& lightAnimPack,
        u32 numAccessories,
        pmm::AccessoryPack* accPacks)
    {
        impl_ = LIME_NEW Impl(
            numModels,
            modelPacks,
            cameraAnimPack,
            lightAnimPack,
            numAccessories,
            accPacks);
    }

    Scene::~Scene()
    {
        LIME_DELETE(impl_);
    }

    void Scene::release()
    {
        impl_->release();
    }

    void Scene::initialize(f32 aspect)
    {
        impl_->initialize(aspect);
    }

    void Scene::update()
    {
        impl_->update();
    }

    void Scene::setState(State state)
    {
        impl_->setState(state);
    }

    void Scene::swap(Scene& rhs)
    {
        lcore::swap(impl_, rhs.impl_);
    }

}
