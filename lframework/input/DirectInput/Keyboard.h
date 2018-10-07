#ifndef INC_LFW_LINPUT_DINPUT_KEYBOARD_H_
#define INC_LFW_LINPUT_DINPUT_KEYBOARD_H_
/**
@file Keyboard.h
@author t-sakai
@date 2009/05/13 create
*/
#include "../linputEnum.h"
#include "Device.h"

namespace linput
{
    class Keyboard: public Device
    {
    public:
        static const u32 BufferSize = 8;
        static const u32 StateSizeInBytes = 256/8;
        typedef BitSet<StateSizeInBytes> StateSet;

        Keyboard();
        virtual ~Keyboard();

        bool initialize(WINDOW_HANDLE hWnd, DirectInputDevice* device);
        virtual void poll();
        virtual void clear();

        inline bool isOn(KeyCode key) const;
        inline bool isClick(KeyCode key) const;
    private:
        Keyboard(const Keyboard&);
        Keyboard& operator=(const Keyboard&);

        StateSet onState_;
        StateSet clickState_;
        DIDEVICEOBJECTDATA devObjectData_[BufferSize];
    };

    inline bool Keyboard::isOn(KeyCode key) const
    {
        return onState_.check(key);
    }

    inline bool Keyboard::isClick(KeyCode key) const
    {
        return clickState_.check(key);
    }
}

#endif //INC_LFW_LINPUT_DINPUT_KEYBOARD_H_
