/**
@file Input.cpp
@author t-sakai
@date 2011/02/18
*/
#include "stdafx.h"
#include "Input.h"

#include <linput/linput.h>

#include <linput/Keyboard.h>
#include <linput/Mouse.h>

namespace viewer
{
    namespace
    {
        inline void clamp(s32& x, s32 min, s32 max)
        {
            if(min>x) x = min;
            if(x>max) x = max;
        }

        linput::KeyCode KeyTable[Input::Key_Num] =
        {
            linput::Key_SPACE,
            linput::Key_C,
        };
    }

    //---------------------------------------------
    //---
    //--- Input
    //---
    //---------------------------------------------
    Input::Impl Input::impl_;

    void Input::initialize(HWND__* hWnd, u32 viewWidth, u32 viewHeight)
    {
        LASSERT(hWnd != NULL);

        linput::Input::InitParam initParam;
        initParam.hWnd_ = hWnd;

        initParam.initDevices_[linput::DevType_Keyboard] = true;
        initParam.initDevices_[linput::DevType_Mouse] = true;


        linput::Input::initialize(initParam);


        for(s32 i=0; i<Axis_Num; ++i){
            impl_.angle_[i] = 0.0f;
        }

        impl_.width_ = viewWidth;
        impl_.height_ = viewHeight;
        impl_.halfWidth_ = viewWidth>>1;
        impl_.halfHeight_ = viewHeight>>1;
        impl_.invHalfWidth_ = (1.0f/static_cast<f32>(impl_.halfWidth_));
        impl_.invHalfHeight_ = (1.0f/static_cast<f32>(impl_.halfHeight_));
    }

    void Input::terminate()
    {
        linput::Input::terminate();
    }

    void Input::acquire()
    {
        linput::Input::getInstance().acquire();
    }

    void Input::unacquire()
    {
        linput::Input::getInstance().unacquire();
    }

    void Input::update()
    {
        linput::Input::getInstance().update();

        const linput::Keyboard* keyboard = linput::Input::getInstance().getKeyboard();
        if(keyboard->isOn(linput::Key_LALT)){
            const linput::Mouse* mouse = linput::Input::getInstance().getMouse();

            if(mouse->isOn(linput::MouseButton_0)){
                s32 x = mouse->getX();
                s32 y = mouse->getY();

                clamp(x, 0, impl_.width_);
                clamp(y, 20, impl_.height_ - 20);

                x -= impl_.halfWidth_;
                y -= impl_.halfHeight_;


                impl_.angle_[Axis_Roll] = impl_.invHalfWidth_ * x * PI;
                impl_.angle_[Axis_Pitch] = impl_.invHalfHeight_ * y * (PI*0.5f);

            }else if(mouse->isOn(linput::MouseButton_1)){
                s32 x = mouse->getDuration(linput::MouseAxis_X);
                clamp(x, 0, impl_.width_);
                x -= impl_.halfWidth_;
                impl_.angle_[Axis_Yaw] = impl_.invHalfWidth_ * x * PI;;
            }
        }
    }

    f32 Input::getAnalogDuration(Analog analog)
    {
        return linput::Input::getInstance().getMouse()->getDuration((linput::MouseAxis)analog);
    }

    f32 Input::getAnalogPosition(Analog analog)
    {
        return linput::Input::getInstance().getMouse()->get((linput::Mouse::Axis)analog);
    }

    bool Input::isOn(Button button)
    {
        return linput::Input::getInstance().getMouse()->isOn((linput::MouseButton)button);
    }

    bool Input::isClick(Button button)
    {
        return linput::Input::getInstance().getMouse()->isClick((linput::MouseButton)button);
    }

    bool Input::isOn(Key key)
    {
        return linput::Input::getInstance().getKeyboard()->isOn( KeyTable[key] );
    }

    bool Input::isClick(Key key)
    {
        return linput::Input::getInstance().getKeyboard()->isClick( KeyTable[key] );
    }

    f32 Input::getAngle(AngularSensorAxis axis)
    {
        LASSERT(0<=axis && axis<Axis_Num);
        return impl_.angle_[axis];
    }
}
