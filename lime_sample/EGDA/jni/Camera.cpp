/**
@file Camera.cpp
@author t-sakai
@date 2011/02/18
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

using namespace lmath;

namespace egda
{
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

        virtual void update(u32 counter);
        virtual void reset();
        void set(const lmath::Vector3& position, const lmath::Vector3& target, f32 fov, f32 asect);

        lmath::Vector3 initPosition_;
        lmath::Vector3 initTarget_;
        f32 initFov_;
        f32 initAspect_;

        lmath::Vector3 up_;
        lmath::Vector3 position_;
        lmath::Vector3 target_;
        lmath::Matrix44 viewMat_;
    };

    Camera::Manual::Manual()
        :up_(0.0f, 1.0f, 0.0f)
        ,initPosition_(0.0f, 0.0f, 1.0f)
        ,initTarget_(0.0f, 0.0f, 0.0f)
        ,initFov_((45.0f/180.0f*PI))
        ,initAspect_(1.0f)
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
        lscene::Scene& scene = lframework::System::getRenderSys().getScene();
        lscene::Camera& camera = scene.getCamera();

        Vector3 camX(1.0f, 0.0f, 0.0f);
        Vector3 camY(0.0f, 1.0f, 0.0f);
        Vector3 camZ(0.0f, 0.0f, -1.0f);

        f32 zoomLength = target_.distance(position_);
        Vector3 rotation(0.0f, 0.0f, 0.0f);
#if 1
        // 回転
        {
            f32 angleX = Input::getAngle(Input::Axis_Pitch);
            f32 angleY = Input::getAngle(Input::Axis_Roll);
            rotation._x = angleX;
            rotation._y = angleY;
        }

        // 視線方向移動
        if(Input::isOn(Input::Button_1)){

            f32 zoomTick = Input::getAnalogDuration(Input::Analog_X);

            if(lmath::isEqual(zoomTick, 0.0f) == false){
                zoomLength -= zoomTick;
                if(zoomLength > 100.0f){
                    zoomLength = 100.0f;
                }else if(zoomLength < 1.0f){
                    zoomLength = 1.0f;
                }
            }
        }

#endif
        // カメラをセット

        Quaternion rot;
        rot.setRotateAxis(camY, rotation._y);
        Quaternion rotX;
        rotX.setRotateAxis(camX, rotation._x);

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

        lmath::Matrix44 mat;
        mat.perspectiveFovLinearZ(initFov_, initAspect_, 1.0f, 200.0f);
        camera.setProjMatrix(mat);

        camera.updateMatrix();
    }

    void Camera::Manual::set(const lmath::Vector3& position, const lmath::Vector3& target, f32 fov, f32 aspect)
    {
        initPosition_ = position;
        initTarget_ = target;
        initFov_ = fov;
        initAspect_ = aspect;
        reset();
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

        virtual void update(u32 counter);
        virtual void reset();

        void setMatrix();

        void set(pmm::CameraAnimPack* pack, f32 aspect);

        lmath::Vector3 up_;
        lmath::Vector3 position_;
        lmath::Vector3 target_;
        f32 fov_;
        f32 aspect_;

        lmath::Matrix44 matrix_;

        pmm::CameraAnimPack* cameraAnimPack_;
    };

    Camera::FrameAnim::FrameAnim()
        :cameraAnimPack_(NULL)
        ,up_(0.0f, 1.0f, 0.0f)
        ,position_(0.0f, 0.0f, 1.0f)
        ,target_(0.0f, 0.0f, 0.0f)
        ,fov_((45.0f/180.0f*PI))
        ,aspect_(1.0f)
    {
    }

    Camera::FrameAnim::~FrameAnim()
    {
    }

    void Camera::FrameAnim::update(u32 counter)
    {
        LASSERT(cameraAnimPack_ != NULL);

        u32 animIndex = cameraAnimPack_->binarySearchIndex(counter);
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

        matrix_.perspectiveFovLinearZ(fov_, aspect_, 1.0f, 200.0f);
        //matrix_.perspectiveFov(fov_, aspect_, 1.0f, 200.0f);
        camera.setProjMatrix(matrix_);

        camera.updateMatrix();
    }

    void Camera::FrameAnim::set(pmm::CameraAnimPack* pack, f32 aspect)
    {
        LASSERT(pack != NULL);
        cameraAnimPack_ = pack;
        aspect_ = aspect;

        reset();
    }


    //------------------------------------------------
    //---
    //--- Camera
    //---
    //------------------------------------------------
    Camera::Camera()
        :mode_(Mode_Manual)
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

    void Camera::setInitial(const lmath::Vector3& position, const lmath::Vector3& target, f32 fov, f32 aspect)
    {
        Manual* manual = reinterpret_cast<Manual*>(impl_[Mode_Manual]);
        manual->set(position, target, fov, aspect);

    }

    void Camera::setCameraAnim(pmm::CameraAnimPack* pack, f32 aspect)
    {
        FrameAnim* frameAnim = reinterpret_cast<FrameAnim*>(impl_[Mode_FrameAnim]);
        frameAnim->set(pack, aspect);
    }
}
