#ifndef INC_LFRAMEWORK_LIGHT_H_
#define INC_LFRAMEWORK_LIGHT_H_
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

        inline s32 getLayerMask() const;
        inline void setLayerMask(s32 mask);
        inline s16 getSortLayer() const;
        inline void setSortLayer(s16 layer);
        inline s8 getType() const;
        inline void setType(s8 type);
        inline bool isCastShadow() const;
        inline void setCastShadow(bool castShadow);

        inline f32 getRadius() const;
        inline void setRadius(f32 radius);
        /**
        */
        inline f32 getHalfAngleCosine() const;
        /**
        */
        inline void setAngle(f32 radian);
        /**
        */
        inline f32 getFalloffHalfAngleCosine() const;
        /**
        */
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

        void setDirectional(const lmath::Vector3& direction);
        void setPoint(const lmath::Vector3& position, f32 radius);
        void setSpot(const lmath::Vector3& position, const lmath::Vector3& direction, f32 angle, f32 falloffAngle);
    private:
        s32 layerMask_;
        s16 sortLayer_;
        s8 type_;
        s8 castShadow_;

        lmath::Vector4 position_; //position, radius
        lmath::Vector4 direction_; //direction, falloffAngle

        /// 色と第４に輝度
        lmath::Vector4 lightColor_;
    };

    inline s32 Light::getLayerMask() const
    {
        return layerMask_;
    }

    inline void Light::setLayerMask(s32 mask)
    {
        layerMask_ = mask;
    }

    inline s16 Light::getSortLayer() const
    {
        return sortLayer_;
    }

    inline void Light::setSortLayer(s16 layer)
    {
        sortLayer_ = layer;
    }

    inline s8 Light::getType() const
    {
        return type_;
    }

    inline void Light::setType(s8 type)
    {
        type_ = type;
    }

    inline bool Light::isCastShadow() const
    {
        return 0 != castShadow_;
    }

    inline void Light::setCastShadow(bool castShadow)
    {
        castShadow_ = (castShadow)? 1 : 0;
    }

    inline f32 Light::getRadius() const
    {
        return position_.w_;
    }

    inline void Light::setRadius(f32 radius)
    {
        position_.w_ = radius;
    }

    inline f32 Light::getHalfAngleCosine() const
    {
        return position_.w_;
    }

    inline void Light::setAngle(f32 radian)
    {
        position_.w_ = lmath::cosf(0.5f*radian);
    }

    inline f32 Light::getFalloffHalfAngleCosine() const
    {
        return direction_.w_;
    }

    inline void Light::setFalloffAngle(f32 radian)
    {
        direction_.w_ = lmath::cosf(0.5f*radian);
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
#endif //INC_LFRAMEWORK_LIGHT_H_
