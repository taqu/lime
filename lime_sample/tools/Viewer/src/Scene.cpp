/**
@file Scene.cpp
@author t-sakai
@date 2011/02/06
*/
#include "stdafx.h"
#include "Scene.h"

#include "Camera.h"
#include "Light.h"

#include "libConverter/Pmd.h"
#include "libConverter/Vmd.h"
#include "libConverter/PmmDef.h"
#include "libConverter/Pmm.h"

#include <lcore/liostream.h>
#include <lgraphics/lgraphics.h>

#include "scene/AnimObject.h"
#include "scene/Object.h"
#include "System.h"
#include <lframework/anim/AnimationSystem.h>
#include "render/RenderingSystem.h"

#include <lframework/scene/Scene.h>
#include <lframework/scene/Camera.h>
#include <lframework/scene/LightEnvironment.h>

#include <lframework/anim/AnimationControlerPartial.h>
#include <lframework/anim/JointAnimation.h>
#include <lframework/anim/IKPack.h>


#include "AnimationControlerIK.h"
#include "AnimationControlerImpl.h"
#include "RigidBodySkeleton.h"
#include "DynamicsWorld.h"

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
        Impl(pmm::Loader& loader);

        ~Impl();

        void release();

        void initialize(f32 aspect);

        void update();

        void setState(Scene::State state);

        /// フレーム進行リセット
        void resetFrame();

        Scene::State state_;

        u32 frameCounter_;
        u32 startFrame_;
        u32 lastFrame_;

        u32 numModels_;
        pmm::ModelPack* modelPacks_;

        pmm::CameraAnimPack cameraAnimPack_;
        pmm::LightAnimPack lightAnimPack_;

        u32 numAccessories_;
        pmm::AccessoryPack* accPacks_;

        Camera camera_;
        Light light_;

        dynamics::RigidBodySkeleton* rigidBodySkeletones_;
    };

    Scene::Impl::Impl()
        :state_(Scene::State_Stop)
        ,frameCounter_(0)
        ,startFrame_(0)
        ,lastFrame_(0)
        ,numModels_(0)
        ,modelPacks_(NULL)
        ,numAccessories_(0)
        ,accPacks_(NULL)
        ,rigidBodySkeletones_(NULL)
    {
    }

    Scene::Impl::Impl(pmm::Loader& loader)
        :state_(Scene::State_Stop)
        ,frameCounter_(0)
        ,startFrame_(0)
        ,lastFrame_(0)
        ,rigidBodySkeletones_(NULL)
    {
        numModels_ = loader.getNumModels();
        numAccessories_ = loader.getNumAccessories();

        modelPacks_ = loader.releaseModelPacks();
        accPacks_ = loader.releaseAccessoryPacks();

        cameraAnimPack_.swap( loader.getCameraAnimPack() );
        lightAnimPack_.swap( loader.getLightAnimPack() );

        startFrame_ = loader.getStartFrame();
        lastFrame_ = loader.getLastFrame();

    }

    Scene::Impl::~Impl()
    {
        release();
    }

    void Scene::Impl::release()
    {
        state_ = Scene::State_Stop;

        LIME_DELETE_ARRAY(rigidBodySkeletones_);

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
        lanim::AnimationSystem &animSys = lframework::System::getAnimSys();

        f32 startFrame = static_cast<f32>(startFrame_);
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

                animCtrlIK->initialize();
                animSys.add(animCtrlIK);
                modelPacks_[i].setAnimationControler(animCtrlIK);

                modelPacks_[i].resetMorph();
            }else{

                AnimCtrl* animCtrl = LIME_NEW AnimCtrl;
                animCtrl->setSkeleton( object->getSkeleton() );
                object->setSkeletonPose( &(animCtrl->getSkeletonPose()) );
                animCtrl->setClip(animClip);

                animCtrl->initialize();
                animSys.add(animCtrl);
                modelPacks_[i].setAnimationControler(animCtrl);

                modelPacks_[i].resetMorph();
            }

            modelPacks_[i].getAnimationControler()->reset(startFrame);
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


        {// カメラ初期化
            const pmm::CameraPose& cameraPose = cameraAnimPack_.getPose(0);
            camera_.setInitial(cameraPose, aspect);
            camera_.setCameraAnim(&cameraAnimPack_, aspect);
            camera_.update(Camera::Mode_FrameAnim, startFrame_);

            camera_.setMode(Camera::Mode_FrameAnim);
            //camera_.setMode(Camera::Mode_Manual);
        }

        {//ライト初期化
            light_.setLightAnim(&lightAnimPack_);
            light_.update(startFrame_);
        }

        {//剛体物理初期化
            rigidBodySkeletones_ = LIME_NEW dynamics::RigidBodySkeleton[numModels_];
            for(u32 i=0; i<numModels_; ++i){
                pmd::RigidBodyPack& pack = modelPacks_[i].getRigidBodyPack();

                if(pack.numRigidBodies_>0){
                    rigidBodySkeletones_[i].create(
                        modelPacks_[i].getAnimationControler()->getSkeletonPose(),
                        *( modelPacks_[i].getObject()->getSkeleton() ),
                        pack);
                }
            }
        }

        state_ = State_Play;
        //state_ = State_Stop;

        frameCounter_ = startFrame_;
    }


    void Scene::Impl::update()
    {
        //lgraphics::Graphics::getDevice().flush(); //モーフィング更新のためフラッシュ
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

        u32 nextFrame;

        switch(state_)
        {
        case State_Stop:
            if(camera_.getMode() == Camera::Mode_Manual){
                camera_.update(frameCounter_);
            }

            if(Input::isClick(Input::Key_2)){
                if(++frameCounter_>lastFrame_){
                    resetFrame();
                    nextFrame = (lastFrame_>1)? 1 : 0;
                }else{
                    nextFrame = (frameCounter_ == lastFrame_)? 0 : (frameCounter_+1);

                    lanim::AnimationSystem &animSys = lframework::System::getAnimSys();
                    animSys.update();
                }

                //アクセサリ更新
                for(u32 i=0; i<numAccessories_; ++i){
                    accPacks_[i].update(frameCounter_);
                }

                //カメラ・ライト更新
                camera_.update(frameCounter_);
                light_.update(frameCounter_);

                //モーフィング
                for(u32 i=0; i<numModels_; ++i){
                    modelPacks_[i].updateMorph(frameCounter_, nextFrame);
                }

            }
            break;

        case State_Play:
            {
                if(++frameCounter_>lastFrame_){
                    resetFrame();
                    nextFrame = (lastFrame_>1)? 1 : 0;
                }else{
                    nextFrame = (frameCounter_ == lastFrame_)? startFrame_ : (frameCounter_+1);

                    lanim::AnimationSystem &animSys = lframework::System::getAnimSys();
                    animSys.update();
                }

                //物理演算
                for(u32 i=0; i<numModels_; ++i){
                    rigidBodySkeletones_[i].setRigidBodyPosition();
                }

                dynamics::DynamicsWorld::getInstance().step(1000.0f/30.0f);

                for(u32 i=0; i<numModels_; ++i){
                    rigidBodySkeletones_[i].updateBoneMatrix();
                }

                //アクセサリ更新
                for(u32 i=0; i<numAccessories_; ++i){
                    accPacks_[i].update(frameCounter_);
                }

                //カメラ・ライト更新
                camera_.update(frameCounter_);
                light_.update(frameCounter_);

                //モーフィング
                for(u32 i=0; i<numModels_; ++i){
                    modelPacks_[i].updateMorph(frameCounter_, nextFrame);
                }

            }
            break;
        };

    }

    void Scene::Impl::setState(Scene::State state)
    {
        state_ = state;
    }

    // フレーム進行リセット
    void Scene::Impl::resetFrame()
    {
        frameCounter_ = startFrame_;
        f32 startFrame = static_cast<f32>(startFrame_);

        for(u32 i=0; i<numModels_; ++i){

            modelPacks_[i].getAnimationControler()->reset(startFrame);
            rigidBodySkeletones_[i].reset();
        }

        for(u32 i=1; i<numAccessories_; ++i){
            accPacks_[i].initialize();
        }

        camera_.update(Camera::Mode_FrameAnim, startFrame_);
        light_.update(startFrame_);
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

    Scene::Scene(pmm::Loader& loader)
    {
        impl_ = LIME_NEW Impl(loader);
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
