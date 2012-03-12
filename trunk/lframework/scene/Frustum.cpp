/**
@file Frustum.cpp
@author t-sakai
@date 2011/11/03 create

*/
#include "Frustum.h"

#include <lmath/geometry/Sphere.h>

#include "Camera.h"

namespace lscene
{
    //---------------------------------------------------
    // �r���[��Ԃ̎�����v�Z
    void Frustum::calc(const Camera& camera)
    {
        const lmath::Matrix44& proj = camera.getProjMatrix();

        znear_ = camera.getZNear();
        zfar_ = camera.getZFar();
        f32 aspect = camera.getAspect();

        f32 z2 = znear_ * znear_;
        f32 invL = 1.0f/lmath::sqrt(z2 + 1.0f);

        leftRightX_ = znear_ * invL;
        leftRightZ_ = -invL;

        f32 invLA = 1.0f/lmath::sqrt(z2 + aspect*aspect);
        topBottomY_ = -znear_*invL;
        topBottomZ_ = -aspect*invL;
    }

    //---------------------------------------------------
    // �r���[��Ԃ̋�����������ɂ��邩
    bool Frustum::include(const lmath::Vector3& position, f32 radius)
    {
        f32 disc;

        //������
        disc = zfar_ + radius - position.z_;
        if(disc<0.0f){
            return false;
        }

        //�ߕ���
        disc = znear_ - radius - position.z_;
        if(disc<0.0f){
            return false;
        }

        //�E����
        f32 xx = leftRightX_ * position.x_;
        f32 zz = leftRightZ_ * position.z_;
        disc = zz + xx - radius;
        if(disc<0.0f){
            return false;
        }

        //������
        disc = zz - xx - radius;
        if(disc<0.0f){
            return false;
        }

        //�㕽��
        f32 yy = topBottomY_ * position.y_;
        zz = topBottomZ_ * position.z_;
        disc = zz + yy - radius;
        if(disc<0.0f){
            return false;
        }

        //������
        disc = zz - yy - radius;
        if(disc<0.0f){
            return false;
        }

        return true;
    }
}
