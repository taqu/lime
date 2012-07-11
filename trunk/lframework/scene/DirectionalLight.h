#ifndef INC_LSCENE_DIRECTIONALLIGHT_H__
#define INC_LSCENE_DIRECTIONALLIGHT_H__
/**
@file DirectionalLight.h
@author t-sakai
@date 2009/10/06 create
*/
#include "lscene.h"
#include <lmath/lmath.h>

namespace lscene
{
    class DirectionalLight
    {
    public:
        DirectionalLight()
            :direction_(0.0f, 1.0f, 0.0f, 0.0f),
            lightColor_(1.0f, 1.0f, 1.0f, 1.0f)
        {
            createLightView();
        }

        ~DirectionalLight()
        {
        }

        const lmath::Matrix44& getLightView() const
        {
            return lightView_;
        }

        const lmath::Vector4& getDirection() const
        {
            return direction_;
        }

        lmath::Vector4& getDirection()
        {
            return direction_;
        }

        void setDirection(const lmath::Vector4& direction)
        {
            direction_ = direction;
            direction_.normalize();
            createLightView();
        }

        const lmath::Vector4& getColor() const
        {
            return lightColor_;
        }

        lmath::Vector4& getColor()
        {
            return lightColor_;
        }

        void setColor(const lmath::Vector3& color, f32 intensity)
        {
            //先に明るさをかけておく
            lightColor_.set(color.x_*intensity, color.y_*intensity, color.z_*intensity, intensity);
        }

    private:
        inline void createLightView()
        {
            lmath::Vector4 up(0.0f, 1.0f, 0.0f, 0.0f);
            if( lmath::isEqual( direction_.y_, 1.0f, 0.01f) ){
                up.set(0.0f, 0.0f, -1.0f, 0.0f);
            }else if(lmath::isEqual( direction_.y_, -1.0f, 0.01f)){
                up.set(0.0f, 0.0f, 1.0f, 0.0f);
            }
#if 1
            lmath::Vector4 eye;
            eye.zero();
            lightView_.lookAt(eye, -direction_, up);
#else
            lmath::Vector4 at;
            at.zero();
            lightView_.lookAt(direction_, at, up);
#endif
        }

        lmath::Matrix44 lightView_;
        lmath::Vector4 direction_;

        /// 色と第４に輝度
        lmath::Vector4 lightColor_;
    };
}

#endif //INC_LSCENE_DIRECTIONALLIGHT_H__
