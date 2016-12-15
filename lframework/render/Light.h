#ifndef INC_LFRAMEWORK_LIGHT_H__
#define INC_LFRAMEWORK_LIGHT_H__
/**
@file Light.h
@author t-sakai
@date 2016/11/30 create
*/
#include "../lframework.h"
#include <lmath/lmath.h>
#include <lmath/Vector4.h>
#include <lmath/Matrix44.h>

namespace lfw
{
    class Light
    {
    public:
        Light();
        ~Light();

        inline s32 getSortLayer() const;
        inline void setSortLayer(s32 layer);
        inline s32 getLayerMask() const;
        inline void setLayerMask(s32 mask);
        inline f32 getRadius() const;
        inline void setRadius(f32 radius);
        inline f32 getFalloffAngle() const;
        inline void setFalloffAngle(f32 radian);

        void getLightView(lmath::Matrix44& view) const;
        void getLightView(lmath::Matrix44& view, lmath::Matrix44& invview) const;

        inline const lmath::Vector4& getDirection() const;
        inline lmath::Vector4& getDirection();
        inline void setDirection(const lmath::Vector4& direction);
        inline const lmath::Vector4& getPosition() const;
        inline lmath::Vector4& getPosition();
        inline void setPosition(const lmath::Vector4& position);

        inline const lmath::Vector4& getColor() const;
        inline lmath::Vector4& getColor();
        inline void setColor(const lmath::Vector4& color);
    private:
        s32 sortLayer_;
        s32 layerMask_;
        f32 radius_;
        f32 falloffAngle_;

        //lmath::Matrix44 lightView_;
        lmath::Vector4 direction_;
        lmath::Vector4 position_;

        /// 色と第４に輝度
        lmath::Vector4 lightColor_;
    };

    inline s32 Light::getSortLayer() const
    {
        return sortLayer_;
    }

    inline void Light::setSortLayer(s32 layer)
    {
        sortLayer_ = layer;
    }

    inline s32 Light::getLayerMask() const
    {
        return layerMask_;
    }

    inline void Light::setLayerMask(s32 mask)
    {
        layerMask_ = mask;
    }

    inline f32 Light::getRadius() const
    {
        return radius_;
    }

    inline void Light::setRadius(f32 radius)
    {
        radius_ = radius;
    }

    inline f32 Light::getFalloffAngle() const
    {
        return falloffAngle_;
    }

    inline void Light::setFalloffAngle(f32 radian)
    {
        falloffAngle_ = radian;
    }

    //inline const lmath::Matrix44& Light::getLightView() const
    //{
    //    return lightView_;
    //}

    inline const lmath::Vector4& Light::getDirection() const
    {
        return direction_;
    }

    inline lmath::Vector4& Light::getDirection()
    {
        return direction_;
    }

    inline void Light::setDirection(const lmath::Vector4& direction)
    {
        direction_ = direction;
    }

    inline const lmath::Vector4& Light::getPosition() const
    {
        return position_;
    }

    inline lmath::Vector4& Light::getPosition()
    {
        return position_;
    }

    inline void Light::setPosition(const lmath::Vector4& position)
    {
        position_ = position;
    }

    inline const lmath::Vector4& Light::getColor() const
    {
        return lightColor_;
    }

    inline lmath::Vector4& Light::getColor()
    {
        return lightColor_;
    }

    inline void Light::setColor(const lmath::Vector4& color)
    {
        lightColor_ = color;
    }
}
#endif //INC_LFRAMEWORK_LIGHT_H__
