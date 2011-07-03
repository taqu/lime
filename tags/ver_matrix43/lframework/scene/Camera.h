#ifndef INC_LSCENE_CAMERA_H__
#define INC_LSCENE_CAMERA_H__
/**
@file Camera.h
@author t-sakai
@date 2009/12/20 create
*/
#include <lmath/lmath.h>
namespace lscene
{
    class Camera
    {
    public:
        Camera()
        {
            viewMatrix_.identity();
            projMatrix_.identity();
            viewProjMatrix_.identity();
        }

        const lmath::Matrix44& getViewMatrix() const
        {
            return viewMatrix_;
        }

        lmath::Matrix44& getViewMatrix()
        {
            return viewMatrix_;
        }

        void setViewMatrix(const lmath::Matrix44& view)
        {
            viewMatrix_ = view;
        }

        const lmath::Matrix44& getProjMatrix() const
        {
            return projMatrix_;
        }

        lmath::Matrix44& getProjMatrix()
        {
            return projMatrix_;
        }

        void setProjMatrix(const lmath::Matrix44& proj)
        {
            projMatrix_ = proj;
        }

        const lmath::Matrix44& getViewProjMatrix() const
        {
            return viewProjMatrix_;
        }

        void updateMatrix()
        {
            viewProjMatrix_ = viewMatrix_;
            viewProjMatrix_ *= projMatrix_;
        }

    protected:
        lmath::Matrix44 viewMatrix_;
        lmath::Matrix44 projMatrix_;
        lmath::Matrix44 viewProjMatrix_;
    };
}

#endif //INC_LSCENE_CAMERA_H__
