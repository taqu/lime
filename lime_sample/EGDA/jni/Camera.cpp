/**
@file Camera.cpp
@author t-sakai
@date 2011/02/18
@date 2011/04/07 フレーム検索方法を汎用アルゴリズムから一時変更
*/
#include "stdafx.h"
#include "Camera.h"

#include <lmath/Quaternion.h>

#include <lframework/System.h>
#include <lframework/render/RenderingSystem.h>
#include <lframework/scene/Scene.h>
#include <lframework/scene/Camera.h>

#include <PmmDef.h>

#include "Input.h"
#include "Config.h"

using namespace lmath;

namespace egda
{
    namespace
    {
        f32 LimitMax = PI_2*0.99f;
        f32 LimitMin = -PI_2*0.99f;

    }
    
    const f32 Camera::TickZoomRatio = 0.1f;
    const f32 Camera::TickRotationRatio = (1.0f/180.0f);
    
    //------------------------------------------------
    //---
    //--- Camera::Manual
    //---
    //------------------------------------------------
    class Camera::Manual : public Camera::Base
    {
    public:
        Manual();
        virtual ~Manual();

        virtual void initialize(){}

        virtual void update(u32 counter);
        virtual void reset();
        void set(const lmath::Vector3& position, const lmath::Vector3& target, f32 fov);

        void resetProjection();

        lmath::Vector3 initPosition_;
        lmath::Vector3 initTarget_;
        f32 initFov_;
        f32 angleX_;
        f32 angleY_;

        lmath::Vector3 up_;
        lmath::Vector3 position_;
        lmath::Vector3 target_;
        lmath::Matrix44 viewMat_;
    };

    Camera::Manual::Manual()
        :initPosition_(0.0f, 0.0f, 1.0f)
        ,initTarget_(0.0f, 0.0f, 0.0f)
        ,initFov_((45.0f/180.0f*PI))
        ,angleX_(0.0f)
        ,angleY_(0.0f)
        ,up_(0.0f, 1.0f, 0.0f)
        ,position_(0.0f, 0.0f, 1.0f)
        ,target_(0.0f, 0.0f, 0.0f)
    {
        viewMat_.lookAt(position_, target_, up_);
    }

    Camera::Manual::~Manual()
    {
    }

    void Camera::Manual::update(u32)
    {
        Config& config = Config::getInstance();

        lscene::Scene& scene = lframework::System::getRenderSys().getScene();
        lscene::Camera& camera = scene.getCamera();

        Vector3 camX(1.0f, 0.0f, 0.0f);
        Vector3 camY(0.0f, 1.0f, 0.0f);
        Vector3 camZ(0.0f, 0.0f, -1.0f);

        f32 zoomLength = target_.distance(position_);
#if 1
        if(Input::isOn(Input::Button_1)){
            f32 zoomTick;
            f32 angleXTick;
            f32 angleYTick;


            switch(config.getScreeMode())
            {
            case ScreenMode_Rot0:
                angleXTick = Input::getAnalogDuration(Input::Analog_Y);
                angleYTick = Input::getAnalogDuration(Input::Analog_X);
                zoomTick = Input::getAnalogDuration(Input::Analog_Z);

                break;

            case ScreenMode_Rot90:
                angleXTick = Input::getAnalogDuration(Input::Analog_X);
                angleYTick = -Input::getAnalogDuration(Input::Analog_Y);
                zoomTick = Input::getAnalogDuration(Input::Analog_Z);
                break;

            case ScreenMode_Rot180:
                angleXTick = -Input::getAnalogDuration(Input::Analog_Y);
                angleYTick = -Input::getAnalogDuration(Input::Analog_X);
                zoomTick = Input::getAnalogDuration(Input::Analog_Z);
                break;

            case ScreenMode_Rot270:
                angleXTick = -Input::getAnalogDuration(Input::Analog_X);
                angleYTick = Input::getAnalogDuration(Input::Analog_Y);
                zoomTick = Input::getAnalogDuration(Input::Analog_Z);
                break;
            };

            zoomTick *= TickZoomRatio;

            zoomLength -= zoomTick;
            if(zoomLength > 100.0f){
                zoomLength = 100.0f;
            }else if(zoomLength < 1.0f){
                zoomLength = 1.0f;
            }

            angleXTick *= TickRotationRatio;
            angleX_ += angleXTick;

            angleYTick *= TickRotationRatio;
            angleY_ += angleYTick;

             if(angleX_ > LimitMax){
                angleX_ = LimitMax;
            }
            if(angleX_ < LimitMin){
                angleX_ = LimitMin;
            }

            while(angleY_ > PI2){
                angleY_ -= PI2;
            }
            while(angleY_ < (-PI2)){
                angleY_ += PI2;
            }
        }

#endif
        // カメラをセット

        Quaternion rot;
        rot.setRotateAxis(camY, angleY_);
        Quaternion rotX;
        rotX.setRotateAxis(camX, angleX_);

        rot *= rotX;

        Matrix43 mat;
        rot.getMatrix(mat);
        //mat.identity();
        camZ.mul33(camZ, mat);
        camZ.normalize();

        position_ = camZ;
        position_ *= zoomLength;
        position_ += target_;

        viewMat_.lookAt(position_, target_, up_);

        camera.setViewMatrix(viewMat_);
        camera.updateMatrix();
    }

    void Camera::Manual::reset()
    {
        position_ = initPosition_;
        target_ = initTarget_;
        viewMat_.lookAt(position_, target_, up_);

        lscene::Scene& scene = lframework::System::getRenderSys().getScene();
        lscene::Camera& camera = scene.getCamera();
        camera.setViewMatrix(viewMat_);

        Config& config = Config::getInstance();

        lmath::Matrix44 mat;
        config.perspectiveFovLinearZ(mat, initFov_);

        camera.setProjMatrix(mat);

        camera.updateMatrix();
    }

    void Camera::Manual::set(const lmath::Vector3& position, const lmath::Vector3& target, f32 fov)
    {
        initPosition_ = position;
        initTarget_ = target;
        initFov_ = fov;
        reset();
    }

    void Camera::Manual::resetProjection()
    {
        lscene::Scene& scene = lframework::System::getRenderSys().getScene();
        lscene::Camera& camera = scene.getCamera();

        Config& config = Config::getInstance();

        lmath::Matrix44 mat;
        config.perspectiveFovLinearZ(mat, initFov_);

        camera.setProjMatrix(mat);

        camera.updateMatrix();
    }

    //------------------------------------------------
    //---
    //--- Camera::SplineAnim
    //---
    //------------------------------------------------
    class Camera::FrameAnim : public Camera::Base
    {
    public:
        FrameAnim();
        virtual ~FrameAnim();

        virtual void initialize()
        {
            cameraAnimPack_->initialize();
        }

        virtual void update(u32 counter);
        virtual void reset();

        void setMatrix();

        void set(pmm::CameraAnimPack* pack);

        lmath::Vector3 up_;
        lmath::Vector3 position_;
        lmath::Vector3 target_;
        f32 fov_;

        lmath::Matrix44 matrix_;

        pmm::CameraAnimPack* cameraAnimPack_;
    };

    Camera::FrameAnim::FrameAnim()
        :up_(0.0f, 1.0f, 0.0f)
        ,position_(0.0f, 0.0f, 1.0f)
        ,target_(0.0f, 0.0f, 0.0f)
        ,fov_((45.0f/180.0f*PI))
        ,cameraAnimPack_(NULL)
    {
    }

    Camera::FrameAnim::~FrameAnim()
    {
    }

    void Camera::FrameAnim::update(u32 counter)
    {
        LASSERT(cameraAnimPack_ != NULL);

        //u32 animIndex = cameraAnimPack_->binarySearchIndex(counter);
        u32 animIndex = cameraAnimPack_->getNextIndex(counter);
        const pmm::CameraPose& cameraPose = cameraAnimPack_->getPose(animIndex);

        if(animIndex == cameraAnimPack_->getNumPoses() - 1){
            //最後のフレーム
            position_ = cameraPose.position_;
            target_ = cameraPose.target_;
            fov_ = cameraPose.fov_;

        }else{
            //補間する

            //次のフレームのポーズ
            const pmm::CameraPose& nextPose = cameraAnimPack_->getPose(animIndex + 1);

            f32 blend = static_cast<f32>(counter - cameraPose.frameNo_);
            blend /= static_cast<f32>(nextPose.frameNo_ - cameraPose.frameNo_);

            position_.lerp(cameraPose.position_, nextPose.position_, blend);

            target_.lerp(cameraPose.target_, nextPose.target_, blend);

            fov_ = blend * nextPose.fov_ + (1.0f-blend) * cameraPose.fov_;

        } //if(animIndex ==
        setMatrix();

    }

    void Camera::FrameAnim::reset()
    {
        cameraAnimPack_->initialize();

        const pmm::CameraPose& cameraPose = cameraAnimPack_->getPose(0);
        position_ = cameraPose.position_;
        target_ = cameraPose.target_;
        fov_ = cameraPose.fov_;

        matrix_.lookAt(position_, target_, up_);
    }

    void Camera::FrameAnim::setMatrix()
    {
        lscene::Scene& scene = lframework::System::getRenderSys().getScene();
        lscene::Camera& camera = scene.getCamera();

        matrix_.lookAt(position_, target_, up_);
        camera.setViewMatrix(matrix_);

        Config& config = Config::getInstance();

        config.perspectiveFovLinearZ(matrix_, fov_);

        camera.setProjMatrix(matrix_);

        camera.updateMatrix();
    }

    void Camera::FrameAnim::set(pmm::CameraAnimPack* pack)
    {
        LASSERT(pack != NULL);
        cameraAnimPack_ = pack;

        reset();
    }

    //------------------------------------------------
    //---
    //--- Camera
    //---
    //------------------------------------------------
    Camera::Camera()
        :mode_(Mode_FrameAnim)
    {
        impl_[Mode_Manual] = LIME_NEW Manual;
        impl_[Mode_FrameAnim] = LIME_NEW FrameAnim;
    }

    Camera::~Camera()
    {
        for(s32 i=0; i<Mode_Num; ++i){
            LIME_DELETE(impl_[i]);
        }
    }

    void Camera::setInitial(const lmath::Vector3& position, const lmath::Vector3& target, f32 fov)
    {
        Manual* manual = reinterpret_cast<Manual*>(impl_[Mode_Manual]);
        manual->set(position, target, fov);

    }

    void Camera::setCameraAnim(pmm::CameraAnimPack* pack)
    {
        FrameAnim* frameAnim = reinterpret_cast<FrameAnim*>(impl_[Mode_FrameAnim]);
        frameAnim->set(pack);
    }

    void Camera::resetProjection()
    {
        if(mode_ == Mode_FrameAnim){
            FrameAnim* frameAnim = reinterpret_cast<FrameAnim*>(impl_[Mode_FrameAnim]);

            frameAnim->setMatrix();
        }else{
            Manual* manual = reinterpret_cast<Manual*>(impl_[Mode_Manual]);
            manual->resetProjection();
        }
    }
}
