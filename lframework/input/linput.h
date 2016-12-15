#ifndef INC_LFW_LINPUT_H__
#define INC_LFW_LINPUT_H__
/**
@file linput.h
@author t-sakai
@date 2009/05/13 create
*/
#include "../Config.h"
#include <lcore/lcore.h>

#ifdef LFW_INPUT_DIRECT_INPUT
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
typedef struct HWND__* WINDOW_HANDLE;
typedef IDirectInput8 DirectInput;
typedef IDirectInputDevice8 DirectInputDevice;
#endif


#define LINPUT_SAFE_RELEASE(p) {if(p!=NULL){(p)->Release();(p)=NULL;}}

namespace linput
{
    using lcore::s8;
    using lcore::s16;
    using lcore::s32;
    using lcore::u8;
    using lcore::u16;
    using lcore::u32;
    using lcore::f32;
    using lcore::f64;

    class Device;
    class Keyboard;
    class Mouse;
    class Joystick;

    enum DeviceType
    {
        DevType_Keyboard =0,
        DevType_Mouse,
        DevType_Joystick,
        DevType_Num,
    };

    enum Error
    {
        Error_None = 0,
        Error_Init,
    };

    struct InitParam
    {
        InitParam()
            :windowHandle_(NULL)
            ,initDevices_{false,false,false}
        {
        }

        WINDOW_HANDLE windowHandle_;
        bool initDevices_[DevType_Num];
    };
}

#if defined(LFW_INPUT_DIRECT_INPUT)
#include "DirectInput/Input.h"
#endif

#endif //INC_LFW_LINPUTENUM_H__
