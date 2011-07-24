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

using namespace lmath;

namespace viewer
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

        virtual void initialize(){}
        virtual void update(u32 counter);
        virtual void reset();
        void set(const pmm::CameraPose& pose, f32 aspect);

        lmath::Vector3 initPosition_;
        lmath::Vector3 initTarget_;
        f32 initFov_;
        f32 initAspect_;

        lmath::Vector3 up_;
        lmath::Vector3 position_;
        lmath::Vector3 target_;

        lmath::Vector3 translation_;

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
        ,translation_(0.0f, 0.0f, 0.0f)
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

        // 回転
        {
            f32 angleX = Input::getAngle(Input::Axis_Pitch);
            f32 angleY = Input::getAngle(Input::Axis_Roll);
            rotation.x_ = angleX;
            rotation.y_ = angleY;
        }

        // 視線方向移動
        if(Input::isOn(Input::Button_1)){

            f32 zoomTick = Input::getAnalogDuration(Input::Analog_X);

            if(lmath::isEqual(zoomTick, 0.0f) == false){
                zoomLength -= zoomTick;
                if(zoomLength > 200.0f){
                    zoomLength = 200.0f;
                }else if(zoomLength < 1.0f){
                    zoomLength = 1.0f;
                }
            }
        }

        // 平行移動
        if(Input::isOn(Input::Button_2)){

            f32 tx = -0.2f*Input::getAnalogDuration(Input::Analog_X);
            f32 ty = 0.2f*Input::getAnalogDuration(Input::Analog_Y);

            tx = lmath::clamp(tx, -1.0f, 1.0f);
            ty = lmath::clamp(ty, -1.0f, 1.0f);

            target_.x_ += tx * viewMat_.m_[0][0] + ty * viewMat_.m_[1][0];
            target_.y_ += tx * viewMat_.m_[0][1] + ty * viewMat_.m_[1][1];
            target_.z_ += tx * viewMat_.m_[0][2] + ty * viewMat_.m_[1][2];
        }


        // カメラをセット

        Quaternion rot;
        rot.setRotateAxis(camX, rotation.x_);
        Quaternion rotY;
        rotY.setRotateAxis(camY, rotation.y_);

        rot *= rotY;

        Matrix34 mat;
        rot.getMatrix(mat);
        camZ.mul33(mat, camZ);
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

        camera.getProjMatrix().perspectiveFovLinearZ(initFov_, initAspect_, 0.5f, 3000.0f);

        camera.updateMatrix();
    }

    void Camera::Manual::set(const pmm::CameraPose& pose, f32 aspect)
    {
        initFov_ = pose.fov_;
        initAspect_ = aspect;

        lmath::Vector3 center(pose.center_[0], pose.center_[1], pose.center_[2]);

        initTarget_ = center;

        lmath::Matrix34 rot;
        rot.identity();
        rot.rotateZ(  pose.angle_[2] );
        rot.rotateY( -pose.angle_[1] );
        rot.rotateX( -pose.angle_[0] );


        initPosition_.set(0.0f, 0.0f, pose.length_);
        initPosition_.mul33( rot, initPosition_);
        initPosition_ += center;

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

        virtual void initialize()
        {
            cameraAnimPack_->initialize();
        }

        virtual void update(u32 counter);
        virtual void reset(){}

        void calcMatrix();
        void setMatrix();

        void set(pmm::CameraAnimPack* pack, f32 aspect);

        lmath::Vector3 up_;
        lmath::Vector3 center_;
        lmath::Vector3 angle_;
        f32 length_;
        f32 fov_;
        f32 aspect_;

        lmath::Matrix44 matrix_;

        pmm::CameraAnimPack* cameraAnimPack_;
    };

    Camera::FrameAnim::FrameAnim()
        :cameraAnimPack_(NULL)
        ,up_(0.0f, 1.0f, 0.0f)
        ,center_(0.0f, 0.0f, 0.0f)
        ,angle_(0.0f, 0.0f, 0.0f)
        ,length_(1.0f)
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
        //u32 animIndex = cameraAnimPack_->getNextIndex(counter);
        const pmm::CameraPose& cameraPose = cameraAnimPack_->getPose(animIndex);

        if(animIndex == cameraAnimPack_->getNumPoses() - 1){
            //最後のフレーム
            center_ = cameraPose.center_;
            angle_ = cameraPose.angle_;
            length_ = cameraPose.length_;
            fov_ = cameraPose.fov_;

        }else{
            //補間する

            //次のフレームのポーズ
            const pmm::CameraPose& nextPose = cameraAnimPack_->getPose(animIndex + 1);

            f32 blend0 = static_cast<f32>(counter - cameraPose.frameNo_);
            blend0 /= static_cast<f32>(nextPose.frameNo_ - cameraPose.frameNo_);
            f32 blend1 = (1.0f-blend0);

            center_.lerp(cameraPose.center_, nextPose.center_, blend0, blend1);
            angle_.lerp(cameraPose.angle_, nextPose.angle_,  blend0, blend1);

            length_ = blend1 * cameraPose.length_ + blend0 * nextPose.length_;
            fov_ = blend1 * cameraPose.fov_ + blend0 * nextPose.fov_;

        } //if(animIndex ==
        calcMatrix();
        setMatrix();

    }

    void Camera::FrameAnim::calcMatrix()
    {
        lmath::Matrix34 rot;
        rot.identity();
        //rot.rotateZ(  angle_.z_ );
        rot.rotateY( -angle_.y_ );
        rot.rotateX( -angle_.x_ );

        lmath::Vector3 zaxis(0.0f, 0.0f, length_);
        zaxis.mul33( zaxis, rot );


        lmath::Vector3 pos( zaxis );
        pos += center_;

        f32 sn = -lmath::sinf(angle_.z_);
        f32 cs =  lmath::cosf(angle_.z_);

        up_.x_ = sn;
        up_.y_ = cs;

        matrix_.lookAt(pos, center_, up_);
    }

    void Camera::FrameAnim::setMatrix()
    {
        lscene::Scene& scene = lframework::System::getRenderSys().getScene();
        lscene::Camera& camera = scene.getCamera();

        camera.setViewMatrix(matrix_);

        camera.getProjMatrix().perspectiveFovLinearZ(fov_, aspect_, 0.5f, 3000.0f);

        camera.updateMatrix();
    }

    void Camera::FrameAnim::set(pmm::CameraAnimPack* pack, f32 aspect)
    {
        LASSERT(pack != NULL);
        cameraAnimPack_ = pack;
        aspect_ = aspect;

        cameraAnimPack_->initialize();
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

    void Camera::setInitial(const pmm::CameraPose& pose, f32 aspect)
    {
        Manual* manual = reinterpret_cast<Manual*>(impl_[Mode_Manual]);
        manual->set(pose, aspect);

    }

    void Camera::setCameraAnim(pmm::CameraAnimPack* pack, f32 aspect)
    {
        FrameAnim* frameAnim = reinterpret_cast<FrameAnim*>(impl_[Mode_FrameAnim]);
        frameAnim->set(pack, aspect);
    }
}
