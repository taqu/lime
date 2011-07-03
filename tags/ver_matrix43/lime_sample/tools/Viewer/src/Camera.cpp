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
#if 0
        Vector3 camX(viewMat_(0,0), viewMat_(1,0), viewMat_(2,0));
        Vector3 camY(viewMat_(0,1), viewMat_(1,1), viewMat_(2,1));
        Vector3 camZ(viewMat_(0,2), viewMat_(1,2), viewMat_(2,2));
#else
        Vector3 camX(1.0f, 0.0f, 0.0f);
        Vector3 camY(0.0f, 1.0f, 0.0f);
        Vector3 camZ(0.0f, 0.0f, -1.0f);
#endif
        f32 zoomLength = target_.distance(position_);
        Vector3 rotation(0.0f, 0.0f, 0.0f);

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
                if(zoomLength > 200.0f){
                    zoomLength = 200.0f;
                }else if(zoomLength < 1.0f){
                    zoomLength = 1.0f;
                }
            }
        }


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

        camera.getProjMatrix().perspectiveFovLinearZ(initFov_, initAspect_, 0.5f, 3000.0f);

        camera.updateMatrix();
    }

    void Camera::Manual::set(const pmm::CameraPose& pose, f32 aspect)
    {
        initFov_ = pose.fov_;
        initAspect_ = aspect;

        lmath::Vector3 center(pose.center_[0], pose.center_[1], pose.center_[2]);

        initTarget_ = center;

        lmath::Matrix43 rot;
        rot.identity();
        rot.rotateX( -pose.angle_[0] );
        rot.rotateY( -pose.angle_[1] );
        rot.rotateZ(  pose.angle_[2] );


        initPosition_.set(0.0f, 0.0f, pose.length_);
        initPosition_.mul33( initPosition_, rot );
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
        virtual void reset();

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

        //u32 animIndex = cameraAnimPack_->binarySearchIndex(counter);
        u32 animIndex = cameraAnimPack_->getNextIndex(counter);
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

    void Camera::FrameAnim::reset()
    {
        cameraAnimPack_->initialize();

        const pmm::CameraPose& cameraPose = cameraAnimPack_->getPose(0);
        center_ = cameraPose.center_;
        angle_ = cameraPose.angle_;
        length_ = cameraPose.length_;
        fov_ = cameraPose.fov_;

        calcMatrix();
    }

    void Camera::FrameAnim::calcMatrix()
    {
        lmath::Matrix43 rot;
        rot.identity();
        rot.rotateX( -angle_._x );
        rot.rotateY( -angle_._y );
        //rot.rotateZ(  angle_._z );

        lmath::Vector3 zaxis(0.0f, 0.0f, length_);
        zaxis.mul33( zaxis, rot );


        lmath::Vector3 pos( zaxis );
        pos += center_;

        f32 sn = -lmath::sinf(angle_._z);
        f32 cs = lmath::cosf(angle_._z);

        up_._x = sn;
        up_._y = cs;

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
