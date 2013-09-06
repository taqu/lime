/**
@file Camera.cpp
@author t-sakai
@date 2011/11/03 create

*/
#include "Camera.h"

namespace lscene
{
    Camera::Camera()
        :znear_(0.1f)
        ,zfar_(1000.0f)
        ,width_(1.0f)
        ,height_(1.0f)
        ,aspect_(1.0f)
    {
        viewMatrix_.identity();
        projMatrix_.identity();
        viewProjMatrix_.identity();
    }

    void Camera::setProjMatrix(const lmath::Matrix44& proj)
    {
        projMatrix_ = proj;

        if(proj.m_[3][3] < F32_EPSILON){
            //透視投影
            znear_ = -proj.m_[2][3]/proj.m_[2][2];
            
            f32 zratio = -proj.m_[2][3]/proj.m_[2][2];
            if(lmath::isEqual(zratio, znear_)){
                zfar_ = 1.0f/proj.m_[2][3] + znear_;
            }else{
                zfar_ = proj.m_[2][2]/proj.m_[2][3] * znear_;
            }

            width_ = 2.0f*znear_/proj.m_[0][0];
            height_ = 2.0f*znear_/proj.m_[1][1];

        }else{
            //平行投影
            znear_ = -proj.m_[2][3]/proj.m_[2][2];
            zfar_ = 1.0f/proj.m_[2][3] + znear_;
            width_ = 2.0f/proj.m_[0][0];
            height_ = 2.0f/proj.m_[1][1];
        }
    }

    void Camera::updateMatrix()
    {
        viewProjMatrix_ = projMatrix_;
        viewProjMatrix_ *= viewMatrix_;
    }

    void Camera::getEyePosition(lmath::Vector4& eye) const
    {
        const lmath::Matrix44& view = viewMatrix_;

        eye.x_ = -(view.m_[0][0] * view.m_[0][3] + view.m_[1][0] * view.m_[1][3] + view.m_[2][0] * view.m_[2][3]);
        eye.y_ = -(view.m_[0][1] * view.m_[0][3] + view.m_[1][1] * view.m_[1][3] + view.m_[2][1] * view.m_[2][3]);
        eye.z_ = -(view.m_[0][2] * view.m_[0][3] + view.m_[1][2] * view.m_[1][3] + view.m_[2][2] * view.m_[2][3]);
    }
}
