#ifndef INC_KEYBOARD_H__
#define INC_KEYBOARD_H__
/**
@file Keyboard.h
@author t-sakai
@date 2009/05/13 create
*/
#include "linputEnum.h"
#include "linputcore.h"

#if defined(LIME_DINPUT)
#include "DirectInput/Device.h"
#endif


namespace linput
{
    class Keyboard
    {
    public:
        Keyboard();
        ~Keyboard();

        bool initialize(Device::InitParam& param);
        void poll();

        bool isOn(KeyCode key) const;
        bool isClick(KeyCode key) const;
    private:
        inline bool isOldOn(KeyCode key) const;

        Device device_;

        u32 oldBuffer_;
        u32 currentBuffer_;
        static const u32 BUFFER_NUM = 2;
        static const s32 KEYSTATE_NUM = 256;
        char keystate_[BUFFER_NUM][KEYSTATE_NUM];
    };
}

#endif //INC_KEYBOARD_H__
