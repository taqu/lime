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
            :position_(0.0f, 0.0f, 0.0f),
            direction_(0.0f, -1.0f, 0.0f),
            lightColor_(1.0f, 1.0f, 1.0f, 1.0f)
        {
        }

        ~DirectionalLight()
        {
        }

        const lmath::Vector3& getPosition() const
        {
            return position_;
        }

        lmath::Vector3& getPosition()
        {
            return position_;
        }

        void setPosition(const lmath::Vector3& position)
        {
            position_ = position;
        }

        const lmath::Vector3& getDirection() const
        {
            return direction_;
        }

        lmath::Vector3& getDirection()
        {
            return direction_;
        }

        void setDirection(const lmath::Vector3& direction)
        {
            direction_ = direction;
            direction_.normalize();
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
            lightColor_.set(color._x*intensity, color._y*intensity, color._z*intensity, intensity);
        }

    private:
        lmath::Vector3 position_;
        lmath::Vector3 direction_;

        /// 色と第４に輝度
        lmath::Vector4 lightColor_;
    };
}

#endif //INC_LSCENE_DIRECTIONALLIGHT_H__
