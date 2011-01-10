/**
@file Light.cpp
@author t-sakai
@date 2010/01/05
*/
#include "Light.h"

namespace lrender
{
    //----------------------------------------------------------------
    //---
    //--- DirectionalLight
    //---
    //----------------------------------------------------------------
    class DirectionalLight : public Light
    {
    public:
        DirectionalLight();
        virtual ~DirectionalLight();

        virtual LightType getType() const;
        virtual void calc(f32& intensity, Vector3& color, Vector3& direction, const Vector3& position);
    };

    LightType DirectionalLight::getType() const
    {
        return Light_Direction;
    }

    void DirectionalLight::calc(f32& intensity, Vector3& color, Vector3& direction, const Vector3& position)
    {
        intensity = intensity_;
        color = color_;
        direction = direction_;
    }


    //----------------------------------------------------------------
    //---
    //--- Light
    //---
    //----------------------------------------------------------------
    Light::Light()
        :intensity_(1.0f)
        ,color_(0.0f, 0.0f, 0.0f)
        ,direction_(0.0f, 0.0f, 1.0f)
    {
    }

    Light::~Light()
    {
    }

    f32 Light::getIntensity() const
    {
        return intensity_;
    }

    void Light::setIntensity(f32 intensity)
    {
        intensity_ = intensity;
    }

    const Vector3& Light::getColor() const
    {
        return color_;
    }

    void Light::setColor(const Vector3& color)
    {
        color_ = color;
    }

    const Vector3& Light::getDirection() const
    {
        return direction_;
    }

    void Light::setDirection(const Vector3& direction)
    {
        direction_ = direction;
    }
}
