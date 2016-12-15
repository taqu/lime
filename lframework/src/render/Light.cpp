/**
@file Light.cpp
@author t-sakai
@date 2016/11/30 create
*/
#include "render/Light.h"

namespace lfw
{
    Light::Light()
        :sortLayer_(0)
        ,layerMask_(Layer_Default)
        ,radius_(1.0f)
        ,falloffAngle_(30.0f*DEG_TO_RAD)
        ,direction_(lmath::Vector4::Forward)
        ,position_(0.0f)
        ,lightColor_(1.0f)
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

        lmath::Vector4 at = add(position_, direction_);
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

        lmath::Vector4 at = add(position_, direction_);
        lookAt(view, invview, position_, at, up);
    }
}
