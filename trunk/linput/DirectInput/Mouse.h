#ifndef INC_LINPUT_DINPUT_MOUSE_H__
#define INC_LINPUT_DINPUT_MOUSE_H__
/**
@file Mouse.h
@author t-sakai
@date 2009/09/08 create
*/
#include "../linputEnum.h"
#include "../DeviceBase.h"
#include "Device.h"

namespace linput
{
    class Mouse : public DeviceBase
    {
    public:
        enum Axis
        {
            Axis_X =0,
            Axis_Y,
            Axis_Num,
        };

        Mouse();
        ~Mouse();

        bool initialize(HWND__* hWnd, IDirectInputDevice8* device);
        void poll();

        bool isOn(MouseButton button) const;
        bool isClick(MouseButton button) const;

        s32 getDuration(MouseAxis axis) const;

        inline s32 getX() const;
        inline s32 getY() const;
        inline s32 get(Axis axis) const;

        void clear();
    private:
        inline void getMousePoint();

        BYTE buttonState_[MouseButton_Num];
        LONG axisDuration_[MouseAxis_Num];

        struct MousePoint
        {
            s32 value_[Axis_Num];
        };

        HWND__* hWnd_;
        Device device_;
        MousePoint mousePoint_;
    };

    inline s32 Mouse::getX() const
    {
        return mousePoint_.value_[Axis_X];
    }

    inline s32 Mouse::getY() const
    {
        return mousePoint_.value_[Axis_Y];
    }

    inline s32 Mouse::get(Axis axis) const
    {
        LASSERT(0<=axis && axis<Axis_Num);
        return mousePoint_.value_[axis];
    }
}

#endif //INC_LINPUT_DINPUT_MOUSE_H__
