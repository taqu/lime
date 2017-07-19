/**
@file Light.cpp
@author t-sakai
@date 2016/11/30 create
*/
#include "render/Light.h"

namespace lfw
{
    Light::Light()
        :layerMask_(Layer_Default)
        ,sortLayer_(0)
        ,type_(LightType_Direction)
        ,castShadow_(0)
        ,position_(lmath::Vector4::construct(0.0f))
        ,direction_(lmath::Vector4::Forward)
        ,lightColor_(lmath::Vector4::construct(1.0f))
    {
    }

    Light::~Light()
    {
    }

    void Light::getLightView(lmath::Matrix44& view) const
    {
        lmath::Vector4 up = lmath::Vector4::Up;
        if( lmath::isEqual( direction_.y_, 1.0f, 0.01f) ){
            up = lmath::Vector4::Backward;
        }else if(lmath::isEqual( direction_.y_, -1.0f, 0.01f)){
            up = lmath::Vector4::Forward;
        }

        lmath::Vector4 at = lmath::Vector4::construct(add(position_, direction_));
        view.lookAt(position_, at, up);
    }

    void Light::getLightView(lmath::Matrix44& view, lmath::Matrix44& invview) const
    {
        lmath::Vector4 up = lmath::Vector4::Up;
        if( lmath::isEqual( direction_.y_, 1.0f, 0.01f) ){
            up = lmath::Vector4::Backward;
        }else if(lmath::isEqual( direction_.y_, -1.0f, 0.01f)){
            up = lmath::Vector4::Forward;
        }

        lmath::Vector4 at = lmath::Vector4::construct(add(position_, direction_));
        lookAt(view, invview, position_, at, up);
    }

    void Light::setDirectional(const lmath::Vector3& direction)
    {
        type_ = LightType_Direction;
        position_ = lmath::Vector4::zero();
        direction_.set(direction, 0.0f);
    }

    void Light::setPoint(const lmath::Vector3& position, f32 radius)
    {
        type_ = LightType_Point;
        position_.set(position, radius);
        direction_ = lmath::Vector4::zero();
    }

    void Light::setSpot(const lmath::Vector3& position, const lmath::Vector3& direction, f32 angle, f32 falloffAngle)
    {
        type_ = LightType_Spot;
        position_.set(position, lmath::cosf(angle*0.5f));
        direction_.set(direction, lmath::cosf(falloffAngle*0.5f));
    }
}
