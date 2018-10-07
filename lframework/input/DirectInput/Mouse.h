﻿#ifndef INC_LFW_LINPUT_DINPUT_MOUSE_H_
#define INC_LFW_LINPUT_DINPUT_MOUSE_H_
/**
@file Mouse.h
@author t-sakai
@date 2009/09/08 create
*/
#include "../linputEnum.h"
#include "Device.h"

namespace linput
{
    class Mouse : public Device
    {
    public:
        static const u32 BufferSize = 8;

        Mouse();
        virtual ~Mouse();

        bool initialize(WINDOW_HANDLE hWnd, DirectInputDevice* device);
        virtual void poll();
        virtual void clear();

        inline bool isOn(MouseButton button) const;
        inline bool isClick(MouseButton button) const;

        inline s32 getDuration(MouseAxis axis) const;

        inline s32 getX() const;
        inline s32 getY() const;
        inline s32 getZ() const;
        inline s32 get(MouseAxis axis) const;

        void resize();
    private:
        Mouse(const Mouse&);
        Mouse& operator=(const Mouse&);

        inline void getMousePoint();
        inline void setOn(u32 index);
        inline void resetOn(u32 index);
        inline void setClick(u32 index);

        LONG axisDuration_[MouseAxis_Num];

        struct MousePoint
        {
            LONG value_[MouseAxis_Num-1];
        };

        WINDOW_HANDLE hWnd_;
        MousePoint mousePoint_;
        DIDEVICEOBJECTDATA devObjectData_[BufferSize];
        s32 width_;
        s32 height_;
        u16 onState_;
        u16 clickState_;
    };

    inline bool Mouse::isOn(MouseButton button) const
    {
        LASSERT(MouseButton_0<= button && button < MouseButton_Num);
        return (onState_ & (0x01U<<button)) != 0;
    }

    inline bool Mouse::isClick(MouseButton button) const
    {
        LASSERT(MouseButton_0<= button && button < MouseButton_Num);
        return (clickState_ & (0x01U<<button)) != 0;
    }

    inline s32 Mouse::getDuration(MouseAxis axis) const
    {
        LASSERT(0<= axis && axis < MouseAxis_Num);
        return axisDuration_[axis];
    }

    inline s32 Mouse::getX() const
    {
        return mousePoint_.value_[MouseAxis_X];
    }

    inline s32 Mouse::getY() const
    {
        return mousePoint_.value_[MouseAxis_Y];
    }

    inline s32 Mouse::getZ() const
    {
        return mousePoint_.value_[MouseAxis_Z];
    }

    inline s32 Mouse::get(MouseAxis axis) const
    {
        LASSERT(0<=axis && axis<(MouseAxis_Num-1));
        return mousePoint_.value_[axis];
    }
}

#endif //INC_LFW_LINPUT_DINPUT_MOUSE_H_
