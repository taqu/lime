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

        void setPosition(const lmath::Vector3& position)
        {
            position_ = position;
        }

        const lmath::Vector3& getDirection() const
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

        void setColor(const lmath::Vector3& color, f32 intensity)
        {
            intensity = lmath::absolute(intensity);

            f32 sqr = color.lengthSqr();
            if(lmath::isEqual(0.0f, sqr)){
                lightColor_.set(0.0f, 0.0f, 0.0f, intensity);
            }else{
#if defined(ANDROID)
                f32 rsqrt = 1.0f/lmath::sqrt(sqr);
#else
                f32 rsqrt = lmath::rcp_22bit(sqr);
#endif
                lightColor_.set(color._x * rsqrt, color._y * rsqrt, color._z * rsqrt, intensity);
            }
        }

    private:
        lmath::Vector3 position_;
        lmath::Vector3 direction_;

        /// 色と第４に輝度
        lmath::Vector4 lightColor_;
    };
}

#endif //INC_LSCENE_DIRECTIONALLIGHT_H__
