﻿#ifndef INC_LFW_LINPUT_DINPUT_JOYSTICK_H_
#define INC_LFW_LINPUT_DINPUT_JOYSTICK_H_
/**
@file Joystick.h
@author t-sakai
@date 2012/09/30 create
*/
#include "../linputEnum.h"
#include "Device.h"

namespace linput
{
    class Joystick: public Device
    {
    public:
        static const u32 BufferSize = 64;

        Joystick();
        virtual ~Joystick();

        bool initialize(WINDOW_HANDLE hWnd, DirectInputDevice* device);
        virtual void poll();
        virtual void clear();

        bool isOn(JoystickButton button) const;
        bool isClick(JoystickButton button) const;

        inline s32 get(JoystickAxis axis) const;
        inline s32 getDuration(JoystickAxis axis) const;
    private:
        Joystick(const Joystick&);
        Joystick& operator=(const Joystick&);

        inline void setOn(u32 index);
        inline void resetOn(u32 index);
        inline void setClick(u32 index);

        LONG axisPosition_[JoystickAxis_Num];
        LONG axisDuration_[JoystickAxis_Num];

        DIDEVICEOBJECTDATA devObjectData_[BufferSize];
        bool onState_[JoystickButton_Num];
        bool clickState_[JoystickButton_Num];
    };

    inline bool Joystick::isOn(JoystickButton button) const
    {
        LASSERT(JoystickButton_0<= button && button < JoystickButton_Num);
        return onState_[button];
    }

    inline bool Joystick::isClick(JoystickButton button) const
    {
        LASSERT(JoystickButton_0<= button && button < JoystickButton_Num);
        return clickState_[button];
    }

    inline s32 Joystick::get(JoystickAxis axis) const
    {
        LASSERT(0<=axis && axis<JoystickAxis_Num);
        return axisPosition_[axis];
    }

    inline s32 Joystick::getDuration(JoystickAxis axis) const
    {
        LASSERT(0<= axis && axis < JoystickAxis_Num);
        return axisDuration_[axis];
    }
}

#endif //INC_LFW_LINPUT_DINPUT_JOYSTICK_H_
