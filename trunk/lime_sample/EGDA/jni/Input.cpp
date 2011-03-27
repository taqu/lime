/**
@file Input.cpp
@author t-sakai
@date 2011/02/18
*/
#include "stdafx.h"
#include "Input.h"


namespace egda
{
    namespace
    {
        inline void clamp(s32& x, s32 min, s32 max)
        {
            if(min>x) x = min;
            if(x>max) x = max;
        }
    }

    //---------------------------------------------
    //---
    //--- Input
    //---
    //---------------------------------------------
    Input::Impl Input::impl_;

    void Input::initialize(s32 viewWidth, s32 viewHeight)
    {
        LASSERT(viewWidth>0);
        LASSERT(viewHeight>0);

        for(s32 i=0; i<Analog_Num; ++i){
            impl_.positions_[i] = 0.0f;
            impl_.durations_[i] = 0.0f;
        }

        for(s32 i=0; i<Axis_Num; ++i){
            impl_.angles_[i] = 0.0f;
        }

        for(s32 i=0; i<Button_Num; ++i){
            impl_.isOn_[i] = false;
            impl_.isClick_[i] = false;
        }

        impl_.width_ = viewWidth;
        impl_.height_ = viewHeight;
        impl_.halfWidth_ = viewWidth>>1;
        impl_.halfHeight_ = viewHeight>>1;
        impl_.invHalfWidth_ = (1.0f/static_cast<f32>(impl_.halfWidth_));
        impl_.invHalfHeight_ = (1.0f/static_cast<f32>(impl_.halfHeight_));
    }


    f32 Input::getAnalogDuration(Analog analog)
    {
        return impl_.durations_[analog];
    }

    f32 Input::getAnalogPosition(Analog analog)
    {
        return impl_.positions_[analog];
    }

    bool Input::isOn(Button button)
    {
        return impl_.isOn_[button];
    }

    bool Input::isClick(Button button)
    {
        return impl_.isClick_[button];
    }

    bool Input::isOn(Key key)
    {
        return false;
    }

    bool Input::isClick(Key key)
    {
        return false;
    }

    f32 Input::getAngle(AngularSensorAxis axis)
    {
        LASSERT(0<=axis && axis<Axis_Num);
        return impl_.angles_[axis];
    }
}
