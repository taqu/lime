#ifndef INC_LINPUT_MOUSE_H__
#define INC_LINPUT_MOUSE_H__
/**
@file Mouse.h
@author t-sakai
@date 2009/09/08 create
*/
#include "linputEnum.h"
#include "linputcore.h"

#if defined(LIME_DINPUT)
#include "DirectInput/Device.h"
#include "DirectInput/MouseData.h"
#endif

namespace linput
{
    class Mouse
    {
    public:
        Mouse();
        ~Mouse();

        bool initialize(Device::InitParam& param);
        void poll();

        bool isOn(MouseButton button) const;
        bool isClick(MouseButton button) const;

        s32 getDuration(MouseAxis axis) const;

        s32 getX() const;
        s32 getY() const;

        void clear();
    private:
        inline void getMousePoint();

        BYTE buttonState_[MouseButton_Num];
        LONG axisDuration_[MouseAxis_Num];

        struct MousePoint
        {
            s32 x_;
            s32 y_;
        };

        Device device_;
        MouseData data_;
        MousePoint mousePoint_;
    };
}

#endif //INC_LINPUT_MOUSE_H__
